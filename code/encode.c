/*
    encode.c
    Source file for encoding data into stego image 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "encode.h"
#include "common.h"

/* Forward declarations (already in header) */

/* Master encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("Starting encoding process...\n");

    if (open_files(encInfo) != e_success)
    {
        printf("Error: Failed to open files.\n");
        return e_failure;
    }
    printf("Files opened successfully.\n");

    if (check_capacity(encInfo) != e_success)
    {
        printf("Error: Image does not have enough capacity.\n");
        return e_failure;
    }
    printf("Capacity check passed.\n");

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf("Error: Failed to copy BMP header.\n");
        return e_failure;
    }
    printf("BMP header copied successfully.\n");

    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        printf("Error: Failed to encode magic string.\n");
        return e_failure;
    }
    printf("Magic string encoded successfully.\n");

    if (encode_secret_file_extn_size((int)strlen(encInfo->extn_secret_file), encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file extension size.\n");
        return e_failure;
    }
    printf("Secret file extension size encoded successfully.\n");

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file extension.\n");
        return e_failure;
    }
    printf("Secret file extension encoded successfully.\n");

    if (encode_secret_file_size((long)get_file_size(encInfo->fptr_secret), encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file size.\n");
        return e_failure;
    }
    printf("Secret file size encoded successfully.\n");

    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Error: Failed to encode secret file data.\n");
        return e_failure;
    }
    printf("Secret file data encoded successfully.\n");

    printf("Encoding completed successfully!\n");
    return e_success;
}

/* Get image size (width * height * 3) */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width = 0, height = 0;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(uint), 1, fptr_image);
    fread(&height, sizeof(uint), 1, fptr_image);
    return width * height * 3;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (!encInfo->fptr_src_image) { perror("fopen src_image"); return e_failure; }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (!encInfo->fptr_secret) { perror("fopen secret"); fclose(encInfo->fptr_src_image); return e_failure; }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (!encInfo->fptr_stego_image) { perror("fopen stego"); fclose(encInfo->fptr_src_image); fclose(encInfo->fptr_secret); return e_failure; }

    return e_success;
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("Image capacity: %u bytes\n", encInfo->image_capacity);

    uint secret_file_size = get_file_size(encInfo->fptr_secret);
    printf("Secret file size: %u bytes\n", secret_file_size);

    uint magic_string_len = (uint)strlen(MAGIC_STRING);
    uint extension_size = (uint)strlen(encInfo->extn_secret_file);

    /* Required bits:
       magic_string bytes + 4 bytes (extn size) + extension_size + 4 bytes(file size) + secret_file_size
       Each byte needs 8 image bytes (1 image byte per secret bit)
       Plus header (we copy 54 bytes header unchanged)
    */
    uint total_required = (magic_string_len + 4 + extension_size + 4 + secret_file_size) * 8 + 54;
    printf("Total required size: %u bytes\n", total_required);

    if (total_required <= encInfo->image_capacity) return e_success;
    return e_failure;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    long cur = ftell(fptr);
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    fseek(fptr, cur, SEEK_SET);
    if (size < 0) return 0;
    return (uint)size;
}

/* Copy BMP header (54 bytes) */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    if (fread(buffer, 1, 54, fptr_src_image) != 54) return e_failure;
    if (fwrite(buffer, 1, 54, fptr_dest_image) != 54) return e_failure;
    return e_success;
}

/* Helper to encode one byte into 8 LSBs of image_buffer */
Status encode_byte_to_lsb(unsigned char data, unsigned char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        /* place bit (7 - i) into image_buffer[i]'s LSB */
        unsigned char bit = (data >> (7 - i)) & 1;
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

/* Encode integer (4 bytes, little-endian) to 32 image bytes via image_buffer (32 bytes) */
Status encode_int_to_lsb(int data, unsigned char *image_buffer)
{
    for (int byte = 0; byte < 4; byte++)
    {
        unsigned char b = (data >> (byte * 8)) & 0xFF; /* little-endian */
        /* encode this byte into next 8 image bytes */
        for (int bit = 0; bit < 8; bit++)
        {
            unsigned char bitval = (b >> (7 - bit)) & 1;
            image_buffer[byte * 8 + bit] = (image_buffer[byte * 8 + bit] & 0xFE) | bitval;
        }
    }
    return e_success;
}

/* Encode data (sequence of bytes) to image: for each data byte read 8 bytes from src image, modify LSBs, write to stego */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    unsigned char image_buffer[8];

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, fptr_src_image) != 8) return e_failure;
        encode_byte_to_lsb((unsigned char)data[i], image_buffer);
        if (fwrite(image_buffer, 1, 8, fptr_stego_image) != 8) return e_failure;
    }
    return e_success;
}

/* Encode magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    return encode_data_to_image(magic_string, (int)strlen(magic_string),
                                encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Encode extension size (int -> 4 bytes -> 32 image bytes) */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    unsigned char buffer[32];
    if (fread(buffer, 1, 32, encInfo->fptr_src_image) != 32) return e_failure;
    encode_int_to_lsb(size, buffer);
    if (fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32) return e_failure;
    return e_success;
}

/* Encode secret file extension (byte-by-byte) */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    unsigned char buffer[8];
    int len = (int)strlen(file_extn);
    for (int i = 0; i < len; i++)
    {
        if (fread(buffer, 1, 8, encInfo->fptr_src_image) != 8) return e_failure;
        encode_byte_to_lsb((unsigned char)file_extn[i], buffer);
        if (fwrite(buffer, 1, 8, encInfo->fptr_stego_image) != 8) return e_failure;
    }
    return e_success;
}

/* Encode secret file size (4 bytes) */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    unsigned char buffer[32];
    if (fread(buffer, 1, 32, encInfo->fptr_src_image) != 32) return e_failure;
    encode_int_to_lsb((int)file_size, buffer);
    if (fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32) return e_failure;
    return e_success;
}

/* Encode secret file data (read secret file and encode byte-by-byte) */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int ch;
    unsigned char image_buffer[8];

    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8) return e_failure;
        if (encode_byte_to_lsb((unsigned char)ch, image_buffer) != e_success) return e_failure;
        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8) return e_failure;
    }

    /* copy remaining image bytes */
    return copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/* Copy rest of source image to stego */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    unsigned char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, fptr_dest) != bytes) return e_failure;
    }
    return e_success;
}

/* read_and_validate_encode_args: basic checks and populate encInfo */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    if (argc < 5) return e_failure;

    if (strstr(argv[2], ".bmp") == NULL) return e_failure;
    encInfo->src_image_fname = argv[2];

    encInfo->secret_fname = argv[3];
    char *dot = strrchr(encInfo->secret_fname, '.');
    if (dot)
    {
        strncpy(encInfo->extn_secret_file, dot, sizeof(encInfo->extn_secret_file)-1);
        encInfo->extn_secret_file[sizeof(encInfo->extn_secret_file)-1] = '\0';
    }
    else
    {
        strncpy(encInfo->extn_secret_file, ".txt", sizeof(encInfo->extn_secret_file)-1);
        encInfo->extn_secret_file[sizeof(encInfo->extn_secret_file)-1] = '\0';
    }

    if (strstr(argv[4], ".bmp") == NULL) return e_failure;
    encInfo->stego_image_fname = argv[4];

    return e_success;
}

/* Check operation type */
OperationType check_operation_type(int argc, char *argv[])
{
    if (argc < 2) return e_unknown;
    if (strcmp(argv[1], "-e") == 0) return e_encode;
    if (strcmp(argv[1], "-d") == 0) return e_decode;
    return e_unknown;
}
