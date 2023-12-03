#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>

/* Function Definitions */

/* Read and validate arguments for decoding
 * Input: command line arguments and structure pointer
 * Description: In BMP Image, width is stored in offset 18,
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	if(argv[2] != NULL)
	{

		if(strcmp(strstr(argv[2],  "."), ".bmp") == 0)
		{
			decInfo -> src_image_fname = argv[2];
		}
		else
		{
			printf("Please pass argument bmp file\n");
		}
	}
	else
	{
		printf("Please pass argument bmp file\n");
		return e_failure;
	}

	if(argv[3] == NULL)
	{
		strcpy(decInfo ->dest_image_fname, "defaultFile");
	}
	else
	{
		strcpy(decInfo -> dest_image_fname, argv[3]);
	}
	return e_success;
}


/* 
 * Open Required files for decoding
 * Input: Img file pointer
 * Discription: Opens all the required files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_decoding_files(DecodeInfo *decInfo)
{
	decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r");
	//do error handling
	if(decInfo -> fptr_src_image  == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> src_image_fname);
		return e_failure;
	}
	else
	{
		rewind(decInfo ->fptr_src_image);
		fseek(decInfo ->fptr_src_image, 54, SEEK_SET);
	}
	return e_success;
}


/* 
 * Decode Magic String
 * Input: File pointer 
 * Output: Verification of magic string
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_magic_string(DecodeInfo *decInfo)
{
	char magic_string[3] = {0};
	char str[8];
	for(int i = 0; i < 2; i++)
	{
		fread(str, 1, 8, decInfo ->fptr_src_image);
		magic_string[i] = decode_byte_from_lsb(magic_string[i], str);
	}
	magic_string[3] = '\0';
	if(strcmp(magic_string, MAGIC_STRING)==0)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}


/* 
 * Decode Byte From LSB's
 * Input: Data and buffer
 * Output: 1 byte of data from 8 bytes of buffer
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_byte_from_lsb( char data, char *image_buffer)
{
	for(int i = 0; i < 8; i++)
	{
		if(image_buffer[i] &1)
		{
			data = data | (1 << (7 - i));
		}
		else
		{
			data = data & ~(1 << (7 - i));
		}
	}
	return data;
}


/* 
 * Decode Secret File Extension
 * Input: size and Image pointer
 * Output: extension of the secret file
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
	char store_file_extn[size+1];
	char str[8];
	for(int i = 0; i < size; i++)
	{
		store_file_extn[i] = 0;
		fread(str, 1, 8, decInfo -> fptr_src_image);
		store_file_extn[i] = decode_byte_from_lsb(store_file_extn[i], str);
	}
	store_file_extn[size] = '\0';
	strcat(decInfo -> dest_image_fname, store_file_extn);
	decInfo -> dest_image_fpointer = fopen(decInfo -> dest_image_fname, "w");
	if(decInfo -> dest_image_fpointer == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo -> dest_image_fname);
		return e_failure;
	}
	else
	{
		printf("Output Image file is created successfully\n");
	}
	return e_success;
}



/* 
 * Secret File Extension File Size
 * Input: Image file pointer
 * Output: Size of the file extention 
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char str[32];
	int size;
	fread(str, 32, 1, decInfo ->fptr_src_image);
	size = decode_size_from_lsb(str);
	decInfo -> extn_size = size;
	if(size == strlen(".txt"))
	{
		return e_success;
	}
	else if(size == strlen(".sh"))
	{
		return e_success;
	}
	else if(size == strlen(".c"))
	{
		return e_success;
	}
	return e_failure;
}


/* 
 * Decode size from lsb
 * Input: Image pointer and buffer
 * Output: gets byte by byte data from lsb 
 * description: Decodes bits from lsb of buffer and appends it to data
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_size_from_lsb(char *image_buffer)
{
	int size = 0;
	for(int i = 0; i < 32; i++)
	{
		if(image_buffer[i] & 1)
		{

			size = size | ( 1<< (31-i));
		}
		else
		{
			size= size & ~(1 << (31 - i));
		}
	}
	return size;
}


/* 
 * Get the secret file data size
 * Input: File pointers 
 * Output: size of the secret file
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_secret_file_data_size( DecodeInfo *decInfo)
{
	char str[32];
	fread(str, 1, 32, decInfo -> fptr_src_image);
	decInfo -> size_src_file = decode_size_from_lsb(str);
	return e_success;
}


/* 
 * Get the data
 * Input: File pointers 
 * Output: DEcoded data to be added in tha file
 * Return Value: e_success or e_failure, on file errors
 */
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
char str[8];
char secret_file_data[size];
for(int i = 0; i < size; i++)
{
	secret_file_data[i] = 0;
	fread(str, 8, 1, decInfo -> fptr_src_image);
	secret_file_data[i] = decode_byte_from_lsb(secret_file_data[i], str);
}
fwrite(secret_file_data, size, 1, decInfo -> dest_image_fpointer);
return e_success;
}


/* 
 * DO DECODING
 * Input: structure pointer
 * Output: prints all the user acknoledgement weather function fails or successful execution
 * Return Value: e_success or e_failure, on file errors
 */
Status do_decoding(DecodeInfo *decInfo)
{
	printf("Decoding Started\n");
	if(open_decoding_files(decInfo) == e_success)
	{
		printf("Opening required files \n");
	}
	else
	{
		printf("ERROR: Failed to open files\n");
		return e_failure;
	}

	if(decode_magic_string(decInfo) == e_success)
	{
		printf("Decoding magic string Done\n");
	}
	else
	{
		printf("ERROR: Failed to Decode magic string\n");
		return e_failure;
	}

	if(decode_secret_file_extn_size(decInfo) == e_success)
	{
		printf("Decoding Output File extension Done\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file extension size\n");
		return e_failure;
	}
	if(decode_secret_file_extn(decInfo-> extn_size, decInfo) == e_success)
	{
		printf("Decoded File Extension Done\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file extension\n");
		return e_failure;
	}

	if(decode_secret_file_data_size(decInfo) == e_success)
	{
		printf("Decode secret file data size Done\n");
	}
	else
	{
		printf("ERROR: Failed to decode secret file data size\n");
		return e_failure;
	}

   if(decode_secret_file_data(decInfo -> size_src_file, decInfo) == e_success)
   {
   printf("Decode secret file data Done\n");
   }
   else
   {
   printf("ERROR: Failed to decode secret file data\n");
   }
   return e_success;
}







