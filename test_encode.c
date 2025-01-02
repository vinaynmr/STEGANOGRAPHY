#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>
int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    Dec_Info decinfo;
    OperationType res = check_operation_type(argv);
    printf("OperationType Detected\n");
    if(res == e_encode)
    {
        if(argc >= 4)
        {
            res = read_and_validate_encode_args(argv,&encInfo);
            if(res == e_success)
            {
                res = do_encoding(&encInfo);
                if(res == e_success)
                    printf(":::::::ENCODING SUCCESSFUL::::::!\n");
                else
                    printf(":::::::ENCODING FAILED::::::!\n");
            }else
                printf("\t\t\t\t\t\t:::::::VALIDATION FAILED :::::::\n");
        }else
            printf("Please give proper arguments for encoding\n");
    }
    else if(res == e_decode)
    {
        if(argc >= 3)
        {
            res = read_and_validate(argv,&decinfo);
            if(res == e_success)
            {
                res = do_decoding(&decinfo);
                if(res == e_success)
                    printf(":::::::DECODING SUCCESSFUL::::::!\n");
                else
                    printf(":::::::DECODING FAILED::::::!\n");
            }
        }else
            printf("Please give proper arguments for decoding\n");
    }
    else
        printf("check arguments which type your'e giving\n");
}
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1],"-e"))
        return e_encode;
    else if(!strcmp(argv[1],"-d"))
        return e_decode;
    else
        return e_unsupported;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    printf("\t\t\t\t\t\t:::::::VALIDATION STARTED :::::::\n");
    encInfo->src_image_fname = malloc(256 * sizeof(char));
    encInfo->secret_fname = malloc(256 * sizeof(char));
    encInfo->stego_image_fname = malloc(256 * sizeof(char));

    if (encInfo->src_image_fname == NULL || encInfo->secret_fname == NULL || encInfo->stego_image_fname == NULL) {
        printf("Memory allocation failed\n");
        return e_failure;
    }
    char *p = strstr(argv[2],".bmp");
    if(p == NULL)
        return e_failure;
    strcpy(encInfo -> src_image_fname,argv[2]);
    p = strstr(argv[3],".txt");
    if(p == NULL)    
        return e_failure;
    strcpy(encInfo -> secret_fname,argv[3]);
    if(argv[4] == NULL)
        strcpy(encInfo -> stego_image_fname,"output.bmp");
    else
        strcpy(encInfo -> stego_image_fname,argv[4]);
    printf("\t\t\t\t\t\t:::::::VALIDATION COMPLETED :::::::\n");
    return e_success;
}

Status read_and_validate(char *argv[],Dec_Info *decinfo)
{
    printf("\t\t\t\t\t\t:::::::VALIDATION STARTED :::::::\n");
    decinfo -> input_fname = malloc(256 * sizeof(char));
    decinfo -> output_fname = malloc(256 * sizeof(char));
    char *p = strstr(argv[2],".bmp");
    if(p == NULL)
        return e_failure;
    strcpy(decinfo -> input_fname,argv[2]);
    if(argv[3] == NULL)
        strcpy(decinfo -> output_fname,"secret_output.txt");
    else
        strcpy(decinfo -> output_fname,argv[3]);
    printf("\t\t\t\t\t\t:::::::VALIDATION COMPLETED :::::::\n");
    return e_success;
}
