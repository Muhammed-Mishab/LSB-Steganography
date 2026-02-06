/*
    encode.h
    Header file for encoding data into stego image  
*/

#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"
#include <stdio.h>

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[5]; /* allow up to 4 chars + null */

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;

/* Prototypes */
OperationType check_operation_type(int argc, char *argv[]);
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);
Status do_encoding(EncodeInfo *encInfo);
Status open_files(EncodeInfo *encInfo);
Status check_capacity(EncodeInfo *encInfo);
uint get_image_size_for_bmp(FILE *fptr_image);
uint get_file_size(FILE *fptr);
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo);
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);
Status encode_secret_file_data(EncodeInfo *encInfo);
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);
Status encode_byte_to_lsb(unsigned char data, unsigned char *image_buffer);
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
Status encode_int_to_lsb(int data, unsigned char *image_buffer);

#endif
