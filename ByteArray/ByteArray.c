#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ByteArray.h"
#include "../helpers.h"

#define DEBUG 0

byteArray* SplitBytes(char* input, splitType type)
{
	size_t inputLength = strlen(input);
	
	if (type == ascii)
	{
		byteArray* asciiResult = malloc(sizeof(byteArray));

		asciiResult->size = inputLength;

		uint8_t* asciiValue = malloc(inputLength * sizeof(uint8_t));

		int i;
		for (i = 0; i < inputLength; i++)
		{
			asciiValue[i] = input[i];
		}

		asciiResult->value = asciiValue;
		return asciiResult;
	}
	else if (type == hex)
	{
		byteArray* hexResult = malloc(sizeof(byteArray));

		// work on a copy since strtok mangles input
		char* s = StringDup(input);
		
		// how many spaces are in the input?
		int numSpaces = 0;
		char* tok = strtok(s, " ");
		
		while (tok != NULL)
		{
			// printf("tok: %s\n", tok);
			tok = strtok(NULL, " ");
			numSpaces++;
		}
		
		hexResult->size = (size_t) numSpaces;

		free(s);
		// printf("%d bytes found!\n", numSpaces);

		uint8_t* hexValue = malloc(numSpaces * sizeof(uint8_t));
		
		int i = 0; 
		
		// now tokenize again storing each byte in hexValue
		s = StringDup(input);
		
		// printf("tokenizing \"%s\"...\n", s);
		tok = strtok(s, " ");
		
		while (tok != NULL)
		{
			//printf("%d: %s\n", i, tok);
			sscanf(tok, "%02x", (unsigned int*) &hexValue[i++]);
			tok = strtok(NULL, " ");
		}
		free(s);

		hexResult->value = hexValue;
		/*
		printf("result->size: %d\n", (int) hexResult->size);
		for (i =0; i<hexResult->size; i++)
		{
			printf("result->value[%d]: 0x%02x\n", i, hexResult->value[i]);
		}
		*/
		
		return hexResult;

	}	
	return NULL;
}

void ByteArray_Free(byteArray* b)
{
	// printf("Free_ByteArray()!\n");
	if (b != NULL)
	{
		free(b->value);
		free(b);
	}
}

#if DEBUG == 1
int main(int argc, char* argv[])
{
	char* input = argv[1];
	char* type = argv[2];

	byteArray* res;
	
	if (strcmp(type, "ascii") == 0)
	{
		res = SplitBytes(input, ascii);
	}
	else if (strcmp(type, "hex") == 0)
	{
		res = SplitBytes(input, hex);
	}

	printf("got byteArray of size %d!\n", (int) res->size);
	
	int i = 0;
	for (i=0; i<res->size; i++)
	{
		printf("0x%02x\n", res->value[i]);
	}
	

	ByteArray_Free(res);
	printf("freed\n");

	return 0;

}
#endif
