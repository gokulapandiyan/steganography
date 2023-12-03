/*
 * NAME 		: GOKULA PANDIYAN A
 * DATE 		: 27/10/2023
 * DISCRIPTION 	: Stegnography
 * INPUT  - 1	: ./a.out -e beautiful.bmp secret.txt
 * OUTPUT - 1	: stego.bmp encoded with secret.txt
 * INPUT  - 2 	: ./a.out -d stego.bmp
 * OUTPUT - 2	: defaultFile.txt decoded from stego.bmp
 */
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;


	if(check_operation_type(argv) == e_encode)
	{
		printf("Selected encoding\n");

		/*CLA count validation */
		if(argc >= 4 && argc <=5)
		{

			/*Checking CLA arguments is get passed in proper extension */
			if(read_and_validate_encode_args(argv, &encInfo) == e_success)
				printf("Read and validate Done \n");
			else
			{
				printf("Error : Read and validate failed \n");
				return e_failure;
			}
			if(do_encoding(&encInfo) == e_success)
				printf("Encoding success\n");
			else
			{
				printf("Encoding failed\n");
				return e_failure;
			}
		}
	}
//Check if the operation type is decoding
	else if(check_operation_type(argv) == e_decode)
	{
		printf("Selected decoding\n");
		if(argc >=3 && argc <=4)
		{
			if(read_and_validate_decode_args(argv, &decInfo) == e_success)
			{
				printf("Read and validate done\n");
			}
			else
			{
				printf("Read and validate failed\n");
			}
			//to perform decoding
			if(do_decoding(&decInfo) == e_success)
			{
				printf("Decoding Done Successfully\n");
			}
			else
			{
				printf("Decoding failed\n");
			}
		}
	}
	else
	{
		printf("Error : Unsupported Operation\n");
	}
	return 0;
}

/* Function Definitions */

/* Operation type
 * Input: Command line argument
 * Description: Checks and routes the entire program to respected way
 * Return Value: e_encode , e_decode or e_unsupported
 */
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



