#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "encode.h"
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status do_encoding(EncodeInfo *encInfo)
{
    printf(":::::::ENCODING STARTED::::::!\n");
    OperationType res = open_files(encInfo);
    if(res == e_failure)
        return e_failure;
    res = check_capacity(encInfo);
    if(res == e_failure)
        return e_failure;
    res = copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res == e_failure)
        return e_failure;
    res = encode_magic_string(MAGIC_STRING,encInfo);
    if(res == e_failure)
        return e_failure;
    char *p = strstr(encInfo -> secret_fname,".txt");
    if(p == NULL)
        return e_failure;
    strcpy(encInfo -> extn_secret_file,p);
    res = encode_secret_file_extn(encInfo -> extn_secret_file,encInfo);
    if(res == e_failure)
        return e_failure;
    rewind(encInfo->fptr_secret);
    fseek(encInfo -> fptr_secret,0,SEEK_END);
    int file_size = ftell(encInfo -> fptr_secret);
    res = encode_secret_file_size(file_size,encInfo);
    if(res == e_failure)
        return e_failure;
    res = encode_secret_file_data(encInfo);
    if(res == e_failure)
        return e_failure;
    res = copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res == e_failure)
        return e_failure;
    else
        return e_success;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
   encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{  
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file =  get_file_size(encInfo -> fptr_secret);
    int len = strlen(MAGIC_STRING);
    int len_ext = 4; //".txt"
    int temp = 54 + (8 *(len + sizeof(int) + len_ext + sizeof(int) + encInfo -> size_secret_file));
    if(encInfo -> image_capacity > temp)
        return e_success;
    else
        return e_failure;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint size =  ftell(fptr);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char s[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(s,54,1,fptr_src_image);
    if(s == NULL)
        return e_failure;
    fwrite(s,54,1,fptr_dest_image);
    if(ftell(fptr_dest_image) == 54)
        return e_success;
    else
        return e_failure;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string);
    OperationType ret = encode_size_to_lsb(len,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(ret == e_failure)
        return e_failure;
    ret = encode_data_to_image(magic_string,len,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(ret == e_failure)
        return e_failure;
    return e_success;
}

Status encode_data_to_image(const char *data, long int len, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    int i;
    for(i = 0; i < len; i++)
    {
        fread(image_buffer,8,1,fptr_src_image);
        OperationType ret = encode_byte_to_lsb(data[i],image_buffer);
        if(ret == e_success)
            fwrite(image_buffer,8,1,fptr_stego_image);
        else
            return e_failure;
    }
    if(i == len)
        return e_success;
    else
        return e_failure;
}

Status encode_size_to_lsb(int data,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image_buffer[32];
    fread(image_buffer,32,1,fptr_src_image);
    for(int i = 31; i >= 0; i--)
    {
        if(data & (1<<i))
            image_buffer[31 - i] |= 0x01;
        else
            image_buffer[31 - i] &= 0xFE;
    }
    fwrite(image_buffer,32,1,fptr_stego_image);
    return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i = 7; i >= 0; i--)
    {
        if(data & (1<<i))
            image_buffer[7 - i] |= 0x01;
        else
            image_buffer[7 - i] &= 0xFE;
    }
    return e_success;

}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int len = strlen(file_extn);
    OperationType res = encode_size_to_lsb(len,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res == e_failure)
        return e_failure;
    res = encode_data_to_image(file_extn,len,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res == e_failure)
        return e_failure;
    else
        return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    int res = encode_size_to_lsb(file_size,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res)
        return e_failure;
    else
        return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo -> fptr_secret,0,SEEK_SET);
    fread(encInfo -> secret_data,encInfo -> size_secret_file,1,encInfo -> fptr_secret);
    OperationType res = encode_data_to_image(encInfo -> secret_data,encInfo -> size_secret_file,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    if(res == e_failure)
        return e_failure;
    else
        return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src))
        fwrite(&ch,1,1,fptr_dest);
    return e_success;
}

