/*
    decode.h
    Header file for decoding data from stego image 
*/
#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

typedef struct DecodeInfo
{
    char *stego_image;
    char *output_file;

    FILE *fptr_stego;
    FILE *fptr_secret;

    int extn_size;
    char extn[10]; /* allow a bit more than 5 for safety */
    int secret_size;

} DecodeInfo;

/* prototypes */
int read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);
int do_decoding(DecodeInfo *decInfo);
int open_stego_file(DecodeInfo *decInfo);
int skip_bmp_header(DecodeInfo *decInfo);
int decode_byte_from_lsb(FILE *fptr, char *ch);
int decode_magic_string(DecodeInfo *decInfo);
int decode_secret_extn_size(DecodeInfo *decInfo);
int decode_secret_extn(DecodeInfo *decInfo);
int decode_secret_file_size(DecodeInfo *decInfo);
int decode_secret_data(DecodeInfo *decInfo);

#endif 
