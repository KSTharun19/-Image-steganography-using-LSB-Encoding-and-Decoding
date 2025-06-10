#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
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

// Function to vliadate the encoding 

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
   if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
   {
     encInfo->src_image_fname = argv[2];
   }
   else
   {
     return e_failure;
   }

   if(argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
   {
    encInfo->secret_fname = argv[3];
   }
   else
   {
    return e_failure;
   }

   if(argv[4] != NULL && strcmp(strstr(argv[4], "."), ".bmp") == 0)
   {
    encInfo->stego_image_fname = argv[4];
   }
   else
   {
     encInfo->stego_image_fname = "stego.bmp";
   }
   return e_success;
}

uint get_file_size(FILE *secret_file)
{
    fseek(secret_file, 0, SEEK_END);
    return ftell(secret_file);
}

//function to check the capacity of sorce image

Status check_capacity(EncodeInfo *encInfo)
{
    //size of .bmp file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    //size of secret.txt file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity > (54 + (2 + 4 + 4 + encInfo->size_secret_file)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

//function to copy the header

Status copy_bmp_header(FILE *source_fptr, FILE *dest_fptr)
{
    char header[54];
    //bring back file pointer to the first position of .bmp file

   // rewind(source_fptr);

   //another way

   fseek(source_fptr, 0, SEEK_SET);
   fread(header, sizeof(char), 54, source_fptr);
   fwrite(header, sizeof(char), 54, dest_fptr);
   return e_success;
}

//function to encode byte to lsb 

Status encode_byte_to_lsb(char data, char *image_buffer)
{
  unsigned  char mask = 1 << 7;
  for(int i=0; i<8; i++)
  {
    //get a bit from the data and OR(|) it with lsb bit of RGB data
    image_buffer[i] = (image_buffer[i] & (0xFE)) | ((data & mask) >> (7 - i));
    mask = mask >> 1;
  }
}

//fuction to encode the date to image

Status encode_data_to_image(char *data, int length, EncodeInfo *encinfo)
{
    //call to encode to byte to encode a charcater
    //fetch * bytes of RGB from .bmp for size number of times
    for(int i=0; i<length;  ++i)
    {
        //read *bytes of RGB
        fread(encinfo -> image_data, 8, sizeof(char), encinfo -> fptr_src_image);
        encode_byte_to_lsb(data[i], encinfo -> image_data);
        fwrite(encinfo -> image_data, 8, sizeof(char), encinfo -> fptr_stego_image);
    }
    return e_success;

}

//fuction to encode MAGIC STRING

Status encode_magic_string(char *magic_string, EncodeInfo *encinfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encinfo);
    return e_success;
}

//function to encode the size to lsb

Status encode_size_to_lsb(int size, char *image_buffer)
{
    uint mask = 1 << 31;
    for(int i=0; i<31; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
}

//function to encode the size of the file

Status encode_size(int size, FILE *fptr_src, FILE *fptr_stego)
{
    char str[32];
    fread(str, 32, sizeof(char), fptr_src);
    //every integer encoding should call the encode_size_to_lsb function
    encode_size_to_lsb(size, str);
    fwrite(str, 32, sizeof(char), fptr_stego);
    return e_success;
}

//function to encode the secret_file extension

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    file_extn = ".txt";
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}

//function to encode the secret file size

Status encode_secret_file_size(long int size, EncodeInfo *encinfo)
{
    char str[32];
    fread(str, 32, sizeof(char), encinfo -> fptr_src_image);
    //every integer encoding should call the encode_size_to_lsb function
    encode_size_to_lsb(size, str);
    fwrite(str, 32, sizeof(char), encinfo -> fptr_stego_image);
    return e_success; 

}

//function to encode the secret file data

Status encode_secret_file_data(EncodeInfo *encinfo)
{
    char ch;
    fseek(encinfo -> fptr_secret, 0 , SEEK_SET);
    for(int i=0; i<encinfo -> size_secret_file; i++)
    {
        //read 8 bytes from .bmp file
        fread(encinfo -> image_data, 8, sizeof(char), encinfo -> fptr_src_image);
        //read a character from secret file
        fread(&ch, 1, sizeof(char), encinfo-> fptr_secret);
        encode_byte_to_lsb(ch, encinfo -> image_data);
        fwrite(encinfo -> image_data, 8, sizeof(char), encinfo -> fptr_stego_image);
    }
    return e_success;
}

//funtion to copy the remaining data

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   char ch;
   while (fread(&ch, 1, 1, fptr_src) > 0)
   {
     fwrite(&ch, 1, 1, fptr_dest);
   }
   return e_success;
}

//function to encode the data

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("All required files are opened successfully\n");
        if(check_capacity(encInfo) ==  e_success)
        {
            printf("Possible to encode the data\n");
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("header copied successfully\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                   printf("MAGIC STRING is encode Succesfully\n");
                   if(encode_size(strlen(".txt"), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                   {
                    printf("Successfully Encoded the file Extension size\n");
                     if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                     {
                        printf("Successfully Encoded the file Extension\n");
                        if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                        {
                           printf("Successfully encoded the secret file SIZE\n");
                           if(encode_secret_file_data(encInfo) == e_success)
                           {
                            printf("Successfully encoded the secret file data\n");
                            if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                            {
                                printf("Successfully Copied the remaining data\n");
                            }
                            else
                            {
                                printf("Failed to copy the remaining data\n");
                                return e_failure;
                            }
                           }
                           else
                           {
                            printf("Failed to encode the secret file date\n");
                            return e_failure;
                           }
                        }
                        else
                        {
                           printf("Failed to encode the secret file size\n");
                           return e_failure;
                        }
                     }
                     else
                     {
                        printf("Failed to encode the extension of the file\n");
                        return e_failure;
                     }
                   }
                   else
                   {
                    printf("Failed to encode the size of extension of the secret.txt\n");
                    return e_failure;
                   }
                }
                else
                {
                   printf("Failed to encode the MAGIC STRING\n");
                   return e_failure;
                }
            }
            else
            {
                printf("Failed to copy the header\n");
                return e_failure;
            }
        }
        else
        {
           printf("Image capacity is not sufficient to encode the data\n");
        }
    }
    else
    {
        printf("Failed to open files\n");
        return e_failure;
    }
    return e_success;
}