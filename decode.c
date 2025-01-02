#include<stdio.h>
#include "decode.h"
#include<string.h>
#include "types.h"

Status do_decoding(Dec_Info *decinfo)
{
    printf("Decoding started!\n");
    OperationType ret = open_files_for_decode(decinfo);
    if(ret == e_failure)
        return e_failure;
    ret = skip_header(decinfo -> fp_input);
    if(ret == e_failure)
        return e_failure;
    ret  = decode_magic_string(decinfo);
    if(ret == e_failure)
        return e_failure;
    ret = decode_extension(decinfo);
    if(ret == e_failure)
        return e_failure;
    ret = decode_data(decinfo);
    if(ret == e_failure)
        return e_failure;
}
Status open_files_for_decode(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::OPEN FILES STARTED ::::::::\n");
    decinfo -> fp_input = fopen(decinfo -> input_fname,"r");
    if(decinfo -> fp_input == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->input_fname);
        return e_failure;
    }
    decinfo -> fp_output = fopen(decinfo -> output_fname,"w");
    if(decinfo -> fp_output == NULL)
    {
        perror("fopen");
        fprintf(stderr,"Error: Unable to open file %s\n",decinfo -> output_fname);
        return e_failure;
    }
    printf("\t\t\t\t\t\t:::::::OPEN FILES COMPLETED ::::::::\n");
    return e_success;
}

Status skip_header(FILE *fp)
{
    fseek(fp,54,SEEK_SET);
    int offset = ftell(fp);
    printf("offset = %d\n",offset);
    if(offset == 54)
        return e_success;
    else
        return e_failure;
}

Status decode_magic_string(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::MAGIC STRING DECODE STARTED ::::::::\n");
    decinfo -> magic_string_len = decode_size_from_lsb(decinfo -> fp_input);
    printf("length of magic string: %d\n",decinfo -> magic_string_len);
    OperationType ret = decode_data_from_image(decinfo -> magic_string_len,decinfo -> magic_string,decinfo -> fp_input,decinfo -> fp_output);
    printf("magic string = %s\n",decinfo -> magic_string);
    printf("\t\t\t\t\t\t:::::::MAGIC STRING DECODE COMPLETED ::::::::\n");
    if(ret == e_failure)
        return e_failure;
    else
        return e_success;
}

int decode_size_from_lsb(FILE *fp)
{
    char buffer[32];
    int len;
    fread(buffer,32,1,fp);
    for(int i = 31; i >= 0; i--)
    {
        if(buffer[31 - i] & 0x01)
            len |= (1<<i);
        else
            len &= (~(1<<i));
    }
    return len;
}

Status decode_data_from_image(int len,char *data,FILE *fp_input,FILE *fp_output)
{
    char buffer[8];
    int i;
    for(i = 0;i < len; i++)
    {
        fread(buffer,8,1,fp_input);
        data[i] = decode_byte_from_lsb(buffer,i);
    }
    data[i] = '\0';
    printf("string = %s\n",data);
    if(i == len)
        return e_success;
    else
        return e_failure;
}

char decode_byte_from_lsb(char *data,int i)
{
    char ch;
    for(int j = 0; j < 8; j++)
    {
        if(data[j] & 0x01)
            ch |= (1<<(7 - j));
        else
            ch &= ~(1<<(7 - j));
    }
    return ch;
}

Status decode_extension(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::EXTENSION DECODE STARTED ::::::::\n");
    decinfo -> extn_len = decode_size_from_lsb(decinfo -> fp_input);
    printf("file extn size = %d\n",decinfo -> extn_len);
    OperationType ret = decode_data_from_image(decinfo -> extn_len,decinfo -> extn,decinfo -> fp_input,decinfo -> fp_output);
    printf("\t\t\t\t\t\t:::::::EXTENSION DECODE COMPLETED ::::::::\n");
    if(ret == e_success)
        return e_success;
    else
        return e_failure;
}

Status decode_data(Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::DATA DECODE STARTED ::::::::\n");
    decinfo -> data_len = decode_size_from_lsb(decinfo -> fp_input);
    printf("secret data size = %d\n",decinfo -> data_len);
    OperationType ret = decode_data_from_image(decinfo -> data_len,decinfo -> data,decinfo -> fp_input,decinfo -> fp_output);
    if(ret == e_success)
    {
        fwrite(decinfo -> data,decinfo -> data_len,1,decinfo -> fp_output);
        printf("\t\t\t\t\t\t:::::::DATA DECODE COMPLETED ::::::::\n");
        return e_success;
    }else
        return e_failure;
}
