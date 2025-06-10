#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if(check_operation_type(argv) == e_encode)
    {
        EncodeInfo encInfo;
        printf("selected encoding\n");

        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validation of arguments is successful\n");

            if(do_encoding(&encInfo) == e_success)
            {
                printf("encoded successfully\n");
                
            }
            else
            {
                printf("Failed to encode the data\n");
            }
        }
        else
        {
            printf("read and validation of arguments is failed");
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo;
        printf("selected decoding\n");
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validation of arguments is successful\n");

            if(do_decoding(&decInfo) == e_success)
            {
                printf("decoded successfully\n");
                
            }
            else
            {
                printf("Failed to decode the data\n");
            }
        }
        else
        {
            printf("read and validation of arguments is failed");
        }
    }
    else
    {
        printf("Invalid option Usage\n");
    }
    
    return 0;
}
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
    










