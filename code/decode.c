/*
    decode.c
    Source file for decoding data from stego image 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "decode.h"
#include "common.h"


/* Validate args */
int read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if (argc < 4)
    {
        printf("Usage: %s -d <stego.bmp> <outputfile>\n", argv[0]);
        return 0;
    }
    if (strcmp(argv[1], "-d") != 0) return 0;

    decInfo->stego_image = argv[2];
    decInfo->output_file = argv[3];
    return 1;
}

int open_stego_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego = fopen(decInfo->stego_image, "rb");
    if (!decInfo->fptr_stego) { perror("fopen stego"); return 0; }
    return 1;
}

int skip_bmp_header(DecodeInfo *decInfo)
{
    return (fseek(decInfo->fptr_stego, 54, SEEK_SET) == 0);
}

/* decode 1 byte from 8 LSBs (reads 8 image bytes) */
int decode_byte_from_lsb(FILE *fptr, char *ch)
{
    unsigned char img_byte;
    unsigned char result = 0;
    for (int i = 0; i < 8; i++)
    {
        if (fread(&img_byte, 1, 1, fptr) != 1) return 0;
        result = (result << 1) | (img_byte & 1); /* accumulate MSB-first */
    }
    *ch = (char)result;
    return 1;
}

/* decode magic string of length 2 (MAGIC_STRING) */
int decode_magic_string(DecodeInfo *decInfo)
{
    char expected[] = MAGIC_STRING;
    int len = (int)strlen(expected);
    char decoded[16];
    for (int i = 0; i < len; i++)
    {
        if (!decode_byte_from_lsb(decInfo->fptr_stego, &decoded[i])) return 0;
    }
    decoded[len] = '\0';
    if (strcmp(decoded, expected) == 0) return 1;
    printf("Error: Magic string mismatch (found '%s')\n", decoded);
    return 0;
}

/* decode 4-byte int (little-endian) */
int decode_secret_extn_size(DecodeInfo *decInfo)
{
    char ch;
    int size = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!decode_byte_from_lsb(decInfo->fptr_stego, &ch)) return 0;
        size |= ((unsigned char)ch) << (i * 8); /* little-endian */
    }
    decInfo->extn_size = size;
    if (decInfo->extn_size >= (int)sizeof(decInfo->extn)) return 0;
    return 1;
}

int decode_secret_extn(DecodeInfo *decInfo)
{
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        if (!decode_byte_from_lsb(decInfo->fptr_stego, &decInfo->extn[i])) return 0;
    }
    decInfo->extn[decInfo->extn_size] = '\0';
    return 1;
}

int decode_secret_file_size(DecodeInfo *decInfo)
{
    char ch;
    int size = 0;
    for (int i = 0; i < 4; i++)
    {
        if (!decode_byte_from_lsb(decInfo->fptr_stego, &ch)) return 0;
        size |= ((unsigned char)ch) << (i * 8);
    }
    decInfo->secret_size = size;
    return 1;
}

int decode_secret_data(DecodeInfo *decInfo)
{
    char secret_name[256];
    snprintf(secret_name, sizeof(secret_name), "%s%s", decInfo->output_file, decInfo->extn);

    decInfo->fptr_secret = fopen(secret_name, "wb");
    if (!decInfo->fptr_secret) { perror("fopen secret out"); return 0; }

    char ch;
    for (int i = 0; i < decInfo->secret_size; i++)
    {
        if (!decode_byte_from_lsb(decInfo->fptr_stego, &ch))
        {
            fclose(decInfo->fptr_secret);
            return 0;
        }
        fwrite(&ch, 1, 1, decInfo->fptr_secret);
    }

    fclose(decInfo->fptr_secret);
    return 1;
}

/*decode */
int do_decoding(DecodeInfo *decInfo)
{
    /* 1. Open the stego image */
    if (!open_stego_file(decInfo))
    {
        printf("ERROR: Cannot open stego image\n");
        return 0;
    }

    /* 2. Skip the BMP header (first 54 bytes) */
    if (!skip_bmp_header(decInfo))
    {
        printf("ERROR: Cannot skip BMP header\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 3. Decode the MAGIC STRING */
    if (!decode_magic_string(decInfo))
    {
        printf("ERROR: Magic string mismatch\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 4. Decode the extension size (like 4 for .txt) */
    if (!decode_secret_extn_size(decInfo))
    {
        printf("ERROR: Cannot decode extension size\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 5. Decode the file extension (.txt, .c, .sh) */
    if (!decode_secret_extn(decInfo))
    {
        printf("ERROR: Cannot decode file extension\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 6. Decode the secret file size */
    if (!decode_secret_file_size(decInfo))
    {
        printf("ERROR: Cannot decode secret file size\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 7. Decode the secret data and write to output file */
    if (!decode_secret_data(decInfo))
    {
        printf("ERROR: Cannot decode secret data\n");
        fclose(decInfo->fptr_stego);
        return 0;
    }

    /* 8. Close file */
    fclose(decInfo->fptr_stego);

    printf("Decoding completed successfully!\n");
    return 1;
}

