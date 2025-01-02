#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAG_SIZE 100
#define EXTEN_LEN 4
#define DATA_LEN 500

typedef struct _DecodeInfo
{
    //encoded image
    char *input_fname;
    FILE *fp_input;

    //decoding image
    int magic_string_len;
    char magic_string[MAG_SIZE];
    int extn_len;
    char extn[EXTEN_LEN];
    int data_len;
    char data[DATA_LEN];
    char *output_fname;
    FILE *fp_output;
}Dec_Info;

//to validate command line arguments
Status read_and_validate(char *argv[],Dec_Info *decinfo);

//decoding function
Status do_decoding(Dec_Info *decinfo);

//open the required file pointers
Status open_files_for_decode(Dec_Info *decinfo);

//skip_header and craete pointer for file
Status skip_header(FILE *fp_input);

//to decode magic string and magic string length
Status decode_magic_string(Dec_Info *decinfo);

//to decode extension length and extension data
Status decode_extension(Dec_Info *decinfo);

//to decode data from encoded image to output file
Status decode_data(Dec_Info *decinfo);

//to decode size(int) from encoded image
int decode_size_from_lsb(FILE *fp);

//to decode data char by char from encoded image
Status decode_data_from_image(int len,char *data,FILE *fp_input,FILE *fp_output);

//to decode data(string) from encoded image
char decode_byte_from_lsb(char *data,int i);

#endif
