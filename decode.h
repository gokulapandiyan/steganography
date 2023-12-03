#ifndef DECODE_H
#define DECODE_H
#include "types.h"   //Contains user defined types

/*
 *Structure to store information required for
 *encoding secret file to source file
 *info about output and intermediate data is 
 *also stored
*/

typedef struct _DecodeInfo
{
	/*Source Image info*/
	char *src_image_fname;
	FILE *fptr_src_image;

	/*Decoded file info*/
	char dest_image_fname[20];
	FILE *dest_image_fpointer;
	uint extn_size;
	uint size_src_file;
}DecodeInfo;

/*Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/*Get File pointers for i/p and o/p files */
Status open_decoding_files(DecodeInfo *decInfo);

/*Decode the data from the source image */
Status decode_byte_from_lsb(char data, char *image_buffer);

/*Decode the magic string(DecodeInfo *decinfo)*/
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode the size from the least significant bits(LSB)*/
Status decode_size_from_lsb(char *image_buffer);

/*Decode the secret file extension from the source image */
Status decode_secret_file_extn( int size, DecodeInfo *decInfo);

/*Decode the size of the secret file extension from the source image*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*Decode the size of the secret file data from the source image*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*Decode the secret file data from the source image */
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

#endif





