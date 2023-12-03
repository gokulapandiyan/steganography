#include <stdio.h>
#include <string.h>
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



/* 
 * Read and validate all datas in the files
 * Input: File pointers and Name of the files
 * Output: File pointers and extension Verification
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args( char *argv[], EncodeInfo *encInfo)
{
	//check src file is .bmp or not
	if( strcmp(strstr(argv[2], "."), ".bmp") == 0 )
	{
		/* storing source file name into structure member*/
		encInfo -> src_image_fname = argv[2];
	}
	else
	{
		printf("ERROR: Please pass a .bmp file as an argument.\n");
		return e_failure;
	}

	/*check secret file */
	if(argv[3] != NULL)
	{
		/*storing secret file name into structure memeber*/
		encInfo -> secret_fname = argv[3];

		/*storing secret file extension*/
		strcpy(encInfo -> extn_secret_file, strchr(argv[3], '.'));
	}
	else
	{
		printf("ERROR : Please pass the secret file.\n");
		return e_failure;
	}

	if(argv[4] != NULL)
	{
		if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
		{
			encInfo -> stego_image_fname = argv[4];
		}
		else
		{
			printf("Please enter only <fileName>.bmp files\n");
		}
	}
	else
	{
		encInfo -> stego_image_fname = "stego.bmp";
	} 

	return e_success;
}



/* 
 * Copy .bmp header files to destination Image
 * Input: Src Image file and Destination Image file
 * Output: Header of the Src file Image is copied to the Destination file
 * Return Value: e_success or e_failure, on file errors
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char copy[54];
	rewind(fptr_src_image);
	rewind(fptr_dest_image);
	fread(copy, 54, 1, fptr_src_image);
	fwrite(copy, 54, 1, fptr_dest_image);
	return e_success;
}



/*
 * Encode magic string to destination Image
 * Input: Destination Image file ptr and magic string
 * Output: Destination Image encoded with magic string
 * Description: .bmp Image is encoded with magic string
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}


/*
 * Encode Secret file extension size
 * Input: Destination Image file ptr and size of secret file extension
 * Output: Destination file encoded with secret file extension size
 * Description: .bmp file encoded with secret file extension file size
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	char str[32];
	fread(str, 1, 32, encInfo -> fptr_src_image);
	encode_size_to_lsb(size, str);
	fwrite(str, 1, 32, encInfo -> fptr_stego_image);
	return e_success;
}

/* 
 * Encode secret file extension
 * Input: Destination Image file ptr and Secret file extension type
 * Output: Destination Image file encoded with secret file extension
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_extn(const char * file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}



/*
 * Encode secret file size
 * Input: Destination Image file ptr and Secret file size
 * Output: Destination Image file encoded with secret file size
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
	char str[32];
	fread(str, 32, 1, encInfo ->fptr_src_image) ;
	encode_size_to_lsb(size, str);
	fwrite(str, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}



/*
 * Encode Secret file data
 * Input: Destination Image file ptr and Secret file ptr
 * Output: Destination Image file encoded with secret file data
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	fseek(encInfo -> fptr_secret, 0, SEEK_SET);
	char str[encInfo -> size_secret_file];
	fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
	encode_data_to_image(str, strlen(str), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}



/*
 * Copy Remaining Data
 * Input: Src Image File ptr and Destination Image file ptr
 * Output: Copies the remaining data from src to destination
 * Return Value: e_success or e_failure, on file errors
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char c;
	while((fread(&c, 1, 1, fptr_src)) > 0)
	{
		fwrite(&c, 1,1,fptr_dest);
	}

	return e_success;
}



/* 
 * Get the secret file size
 * Input: secret file ptr
 * Output: pointer for int contain secret file size
 * Return Value: e_success or e_failure, on file errors
 */
uint get_file_size(FILE *fptr)
{
	fseek(fptr, 0, SEEK_END);
	uint s_ize = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	return s_ize;
}



/* 
 * Check Capacity
 * Input: src Image ptr and secret file ptr
 * Description: Checks that the msg can able to store in img or not
 * Return Value: e_success or e_failure, on file errors
 */
Status check_capacity(EncodeInfo *encinfo)
{
	encinfo -> image_capacity = get_image_size_for_bmp(encinfo -> fptr_src_image);
	encinfo -> size_secret_file = get_file_size(encinfo -> fptr_secret);
	if(encinfo -> image_capacity - 54 >= ((strlen(MAGIC_STRING)+4+4+4+encinfo ->size_secret_file)*8))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}



/* Encode Byte to LSB's
 * Input: buffer and data
 * Output: data replaced in lsb of buffer
 * Description: each buffer contain 8 byte and replaced with 1 byte of data in lsb
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i = 7; i >= 0; i--)
	{
		image_buffer[7-i] = ((unsigned)(data & (1 << (i))) >> (i)) | ((image_buffer[7-i] & 0xFE));
	}
}


/* 
 * Encode data to image
 * Input: data ,size, src file ptr, Dest.file ptr
 * Output: Data from secret file will be encoded to dest file
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_data_to_image( const char * data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	//create char arr of 8 bytes
	char image[8];
	//loop according to the length of the data trying to encode
	for(int i = 0; i < size; i++)
	{
		//read 8 bytes of data from source image
		fread(image, 8, 1, fptr_src_image);
		//call encode byte to lsb 
		encode_byte_to_lsb(data[i], image);
		//Write the encoded arr to stego file
		fwrite(image, 8, 1, fptr_stego_image);
	}
}


/*
 * Encode size to lsb
 * Input: size and buffer
 * Output: encodes the secret file size to lsb
 * Return Value: e_success or e_failure, on file errors
 */
Status encode_size_to_lsb(int size, char * image_buffer)
{
	for(int i = 31; i >= 0; i--)
	{
		image_buffer[31-i] = ((unsigned)(size & (1 << (i))) >> (i)) | ((image_buffer[31-i] & 0xFE));
	}
}	



/*
 * DO ENCODING
 * Input: structure pointer
 * Output: prints all the user acknoledgement weather function fails or successful execution
 * Return Value: e_success or e_failure, on file errors
 */
Status do_encoding(EncodeInfo *encInfo)
{
	printf("Encoding Started\n");
	if(open_files(encInfo) == e_success)
	{
		printf("Open files successful.\n");
	}			
	else
	{
		printf("ERROR : Failed to Open files.\n");
		return e_failure;
	}

	if(check_capacity(encInfo) == e_success)
	{
		printf("Checking Capacity successful.\n");
	}
	else
	{
		printf("ERROR : Failed To Check Capacity\n");
		return e_failure;
	}

	if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
	{
		printf("copied bmp file successful.\n");
	}
	else
	{
		printf("ERROR : Failed to copy .bmp files\n");
		return e_failure;
	}

	if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
	{
		printf("Encode magic string successful.\n");
	}
	else
	{
		printf("ERROR : Failed to encode magic string\n");
		return e_failure;
	}

	if( encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
	{
		printf("Encode secret file extension size successful.\n");
	}
	else
	{
		printf("ERROR : Failed to encode secret file extension size\n");
	}

	if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
	{
		printf("Encode secret file extension successful.\n");
	}
	else
	{
		printf("ERROR : Failed to encode secret file extension\n");
	}

	if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
	{
		printf("Encoded secret file size successful.\n");
	}
	else
	{
		printf("ERROR : Failed to encode secret file size\n");
		return e_failure;
	}

	if(encode_secret_file_data(encInfo) == e_success)
	{
		printf("Encoded secret file data successful.\n");
	}
	else
	{
		printf("ERROR : Failed to encode  secret file data\n");
		return e_failure;
	}

	if(copy_remaining_img_data(encInfo ->fptr_src_image, encInfo ->fptr_stego_image) == e_success)
	{
		printf("Copied remaining data successful.\n");
	}
	else
	{
		printf("ERROR : Failed to copy remaining data\n");
		return e_failure;
	}
	return e_success;
}

