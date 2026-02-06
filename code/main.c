/*
Name : Muhammad Mishab
Description : Main file for LSB Steganography
Date : 17 - 11 - 2025
*/



#include <stdio.h>
#include <string.h>

#include "types.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    OperationType op = check_operation_type(argc, argv);

    if (op == e_encode)
    {
        EncodeInfo encInfo;
        if (read_and_validate_encode_args(argc, argv, &encInfo) != e_success)
        {
            printf("Usage: %s -e <source.bmp> <secret_file> <stego.bmp>\n", argv[0]);
            return 1;
        }

        if (do_encoding(&encInfo) == e_success)
        {
            printf("Encoding finished OK.\n");
            return 0;
        }
        else
        {
            printf("Encoding failed.\n");
            return 1;
        }
    }
    else if (op == e_decode)
    {
        DecodeInfo decInfo;
        if (!read_and_validate_decode_args(argc, argv, &decInfo))
        {
            printf("Usage: %s -d <stego.bmp> <output_basename>\n", argv[0]);
            return 1;
        }
        if (!do_decoding(&decInfo))
        {
            printf("Decoding failed.\n");
            return 1;
        }
        return 0;
    }
    else
    {
        printf("Unsupported operation. Use -e to encode or -d to decode.\n");
        return 1;
    }
}
