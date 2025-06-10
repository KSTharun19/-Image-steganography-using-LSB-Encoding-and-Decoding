#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /*Output file Info*/
    char *output_file_name;
    FILE *fptr_otput_file;

    /*Decoding Data BUffers*/
    char exten_decode_file[MAX_FILE_SUFFIX];
    long decode_file_size;
    char secret_data[MAX_SECRET_BUF_SIZE];

} DecodeInfo;

/* Decoding function prototypes */

/* Read and validate  decoding arguments*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


/* Perform the decoding*/
Status do_decoding(DecodeInfo *decInfo);


/* Get file pointers for input and output files */
Status open_decode_files(DecodeInfo *decInfo);


/* Decode the magic string for validation */
Status decode_magic_string(DecodeInfo *decInfo);

/* Function to decode the file size */
Status decode_size(int *size, DecodeInfo *decInfo);

/*Function to decode the secret file extension*/
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);


/* Decode the secret file extension */
Status decode_secret_file_size(long int *sizee, DecodeInfo *decInfo);


/* Decode the secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode data from the image*/
Status decode_data_from_image(char *data, int size, DecodeInfo *decInfo);


/* Decode a byte from the LSB of image data array*/
Status decode_byte_from_lsb(char *image_buffer);


/* Decode size from LSB */
Status decode_size_from_lsb(char *image_buffer);


#endif /* DECODE_H */








