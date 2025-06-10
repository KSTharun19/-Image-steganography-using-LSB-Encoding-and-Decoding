#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"



Status open_decode_files(DecodeInfo *decInfo)
{
    
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }

    // Secret file
    decInfo->fptr_otput_file = fopen(decInfo->output_file_name, "w");
    // Do Error handling
    if (decInfo->fptr_otput_file == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_file_name);
       fclose(decInfo->fptr_stego_image);
    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}





// Function to vliadate the decoding

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
   if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
   {
     decInfo->stego_image_fname = argv[2];
   }
   else
   {
     return e_failure;
   }

   if(argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
   {
    decInfo->output_file_name = argv[3];
   }
   else
   {
    decInfo->output_file_name = "decoded.txt";
   }
   
   return e_success;
}

//Function to decode byte from LSB

Status decode_byte_from_lsb(char *image_buffer)
{
  char decoded_byte = 0;
  for(int i = 0; i < 8; i++)
  {
    decoded_byte = (decoded_byte << 1) | (image_buffer[i] & 1);
  }
  return decoded_byte;
}

//Function to decode data from image

Status decode_data_from_image(char *data, int length, DecodeInfo *decInfo)
{
  for(int i = 0; i < length; i++)
  {
    fread(decInfo -> secret_data, 8, sizeof(char), decInfo -> fptr_stego_image);
    data[i] = decode_byte_from_lsb(decInfo -> secret_data);
  }
  data[length] = '\0'; // NULL to terminate
  return e_success;
}

//function to deoce size from lsb
Status decode_size_from_lsb(char *image_buffer)
{
   int decode_size = 0;
    for (int i = 0; i < 32; i++)
    {
        decode_size = (decode_size << 1) | (image_buffer[i] & 1);
    }
    return decode_size;
}


// Function to decode the magic String
Status decode_magic_string(DecodeInfo *decInfo)
{
  char magic_string [strlen(MAGIC_STRING)+1];
  decode_data_from_image(magic_string, strlen(MAGIC_STRING), decInfo);
  if(strcmp(magic_string, MAGIC_STRING) == 0)
  {
    printf("Magic string is successfully Decoded\n");
    return e_success;
  }
  return e_failure;
}

//function decode the sizze of the file

Status decode_size(int *size, DecodeInfo *decInfo)
{
  char str[32];
  fread(str, 32, sizeof(char), decInfo->fptr_stego_image);
   *size =  decode_size_from_lsb(str);
   //fwrite(str, 32, sizeof(char), decInfo->fptr_otput_file);
   return e_success;
}

//fuction to decode secret file extension

Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo)
{
  
  decode_data_from_image(file_extn, MAX_FILE_SUFFIX, decInfo);
  file_extn[MAX_FILE_SUFFIX] = '\0';
  return e_success;
}

//function to decode the secret file size

 
Status decode_secret_file_size(long int *sizee, DecodeInfo *decInfo)
{
  char str[32];
   fread(str, 32, sizeof(char), decInfo->fptr_stego_image);
   *sizee = decode_size_from_lsb(str);
  // fwrite(str, 32, sizeof(char), decInfo->fptr_otput_file);
   return e_success;
}

//function to decode the secret file data

Status decode_secret_file_data(DecodeInfo *decInfo)
{
  char ch;
    fseek(decInfo -> fptr_otput_file, 0 , SEEK_SET);
    char *buffer = malloc(decInfo->decode_file_size);
    if(!buffer)
    {
      fprintf(stderr, "ERROR: Memmory allocation failed\n");
      return e_failure;
    }
    for(int i=0; i<decInfo -> decode_file_size; i++)
    {
        //read 8 bytes from .bmp file
        fread(decInfo -> secret_data, 8, sizeof(char), decInfo -> fptr_stego_image);
        
        buffer[i] = decode_byte_from_lsb(decInfo -> secret_data);
       
    }
    fwrite(buffer, 1, decInfo->decode_file_size, decInfo->fptr_otput_file);
    free(buffer);
    return e_success;
}

//Function to perform decoding

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
      printf("All required for decoding is opened successfully\n");

      //Move the Stego file pointer to the 54th bye position
      if(fseek(decInfo -> fptr_stego_image, 54, SEEK_SET) == 0)
      {
        printf("Stego_image file pointer is moved to 54th Byte successfully\n");

      if(decode_magic_string(decInfo) == e_success)
      {
        printf("Magic string is decoded successfully\n");
         
            int extn_size;
            if(decode_size(&extn_size, decInfo) == e_success)
            {
              printf("Successfully decoded the size of the secret_file extension\n");
             if(decode_secret_file_extn(decInfo->exten_decode_file,decInfo) == e_success)
             {
              printf("Successfully decoded the secret_file extension\n");
               if(decode_secret_file_size(&decInfo->decode_file_size, decInfo)== e_success)
               {
                 printf("Size of the secret_file is Successfully decoded\n");
                 if(decode_secret_file_data(decInfo) == e_success)
                 {
                  printf("Successfully decoded the seceret_file data\n");
                 }
                 else
                 {
                  printf("failed to decode the secret_file data\n");
                  return e_failure;
                 }
               }
               else
               {
                 printf("Failed to decode the size of secret_file\n");
                 return e_failure;
               }
             }
             else
             {
               return e_failure;
             }
        }
         else
         {
          printf("Fialed to decode the size of secret_file extension");
          return e_failure;
         }

      }
      else
      {
        printf("Failed to decode the magic String\n");
        return e_failure;
      }
      }
      else
      {
        printf("Unable to move the file pointer\n");
        return e_failure;
      }
    }
    else
    {
      printf("Failed to oepn the required files\n");
      return e_failure;
    }

    return e_success;
}



