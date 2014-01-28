#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct byte
{
	uint8_t value;
	bool differs;
} byte;

#define NORMAL "\033[0m"
#define DIFFERS "\033[31m"

void stdprint(char* s, ...);
void errprint(char* s, ...);
void printByte(byte, int ascii);
char* padOffset(long, int);
int longSize(unsigned long long int);

int main(int argc, char* argv[])
{
	// check for args
	if (argc < 3)
	{
		errprint("Need at least two filenames!\n");
	}
	
	char* filename1 = argv[1];
	char* filename2 = argv[2];
	
	int bufferSize = 9;
	
	// open up the two files
	FILE* f1 = fopen(filename1, "rb");
	FILE* f2 = fopen(filename2, "rb");
	
	if (f1 == NULL || f2 == NULL)
	{
		errprint("File error!\n");
	}

	// how long is each file?
	fseek(f1, 0, SEEK_END);
	fseek(f2, 0, SEEK_END);

	long long f1_size = ftell(f1);
	long long f2_size = ftell(f2);
		
	// stack allocate for simplicity
	byte f1_bytes[f1_size / bufferSize];
	byte f2_bytes[f2_size / bufferSize];
	
	int offsetLen = f1_size > f2_size ? longSize(f1_size) : longSize(f1_size);
	
	// rewind files 
	rewind(f1); rewind(f2);

	// byte buffers
	uint8_t buffer1[bufferSize];
	uint8_t buffer2[bufferSize];

	bool reading = true;

	size_t b1_read, b2_read;
	
	long long offset1 = 0, offset2 = 0;
	long long longerOffset = 0;
   	    
	while (reading)
	{
		// read bufferSize bytes from files
		b1_read = fread(&buffer1, 1, bufferSize, f1);
		b2_read = fread(&buffer2, 1, bufferSize, f2);
			
		// this is a list of pointers to bytes

	    	
		if (b1_read > 0 || b2_read > 0)
		{
			// now compare the bytes in the buffer
			int i;
			for (i = 0; i < bufferSize; i++)
			{
				// compare these bytes
				byte b1;
				byte b2;
				
				b1.value = (uint8_t) buffer1[i];
				b2.value = (uint8_t) buffer2[i];
				
				if (b1.value != b2.value || b1_read != b2_read)
				{
					b1.differs = true;
					b2.differs = true;
				}
				else
				{
					b1.differs = false;
					b2.differs = false;
				}
				
				f1_bytes[i] = b1;
				f2_bytes[i] = b2;
			}
			
			// determine which offset is what we display
			longerOffset = offset1 > offset2 ? offset1: offset2;
			
			// pad offset and print it
			char* paddedOffset = padOffset(longerOffset, offsetLen);
			fprintf(stdout, "0x0%s | ", paddedOffset);
			free(paddedOffset);
			
			offset1 += b1_read;
			offset2 += b2_read;

			// now print the bytes hex
			for (i = 0; i < b1_read; i++)
			{
				printByte(f1_bytes[i], 0);
			}
			// space pad?
			if (b1_read < bufferSize)
			{
				for (i = b1_read; i < bufferSize; i++)
				{
					printf("   ");
				}
			}
			
			printf("| ");

			// now print the bytes ascii
			for (i = 0; i < b1_read; i++)
			{
				printByte(f1_bytes[i], 1);
			}
			// space pad?
			if (b1_read < bufferSize)
			{
				for (i = b1_read; i < bufferSize; i++)
				{
					printf(" ");
				}
			}

			printf(" |-| ");
			for (i = 0; i < b2_read; i++)
			{
				printByte(f2_bytes[i], 0);
			}
			// space pad?
			if (b2_read < bufferSize)
			{
				for (i = b2_read; i < bufferSize; i++)
				{
					printf("   ");
				}
			}

			printf("| ");
			for (i = 0; i < b2_read; i++)
			{
				printByte(f2_bytes[i], 1);
			}

			printf("\n");
		}
		else
		{
			reading = false;
		}	
	}
	
	fclose(f1);
	fclose(f2);
		
	return 0;
}

void stdprint(char* s, ...)
{
	va_list args;
	va_start(args, s);

	vfprintf(stdout, s, args);
}

void errprint(char* s, ...)
{
	va_list args;
	va_start(args, s);

	vfprintf(stderr, s, args);
	exit(1);
}

void printByte(byte b, int ascii)
{
	char pad = '\0';
	
	if (ascii == 0) // hex
	{
		if (b.value <= 0xa)
		{
			pad = '0';
		}
	
		if (b.differs)
		{
			stdprint("%s%c%x ", DIFFERS, pad, b.value);
			goto end;
		}
	
		stdprint("%s%c%x ", NORMAL, pad, b.value);
	}
	else // ascii
	{
		if (!isupper(b.value) && 
		!islower(b.value) && 
		!isdigit(b.value) && 
		b.value != ' ')
		{
			stdprint("%c", '.');
			goto end;
		}
		if (b.value <= 0xa)
		{
			pad = ' ';
		}
	
		if (b.differs)
		{
			stdprint("%s%c%c", DIFFERS, pad, (char) b.value);
			goto end;
		}
	
		stdprint("%s%c%c", NORMAL, pad, (char) b.value);
		
	}
	end:
	stdprint("%s", NORMAL);
}

char* padOffset(long offset, int size)
{
	int offsetSize = longSize(offset);

	char chrOffset[offsetSize];
	sprintf(chrOffset, "%lx", offset);

	// alloc mem for result
	char* result = malloc((2 + size) * sizeof(char));
	
	// positions in result & chrOffset
	int i, j, k = 0; 
	
	// copy the offset backwards into the result
	for (i = size, j = offsetSize - 1; j >= 0; i--, j--)
	{
		result[i] = chrOffset[j];
	}
	k = i;
	// fill the rest with zeroes
	for (i = k; i >= 0; i--)
	{
		result[i] = '0';
	}

	return result;
}

int longSize(unsigned long long int number)
{
    char numSize[512];
    
    sprintf(numSize, "%llx", number);
    
    return strlen(numSize);
}

bool search(uint8_t buffer[], char* searchTerm)
{
	for (int i = 0, s = strlen(searchTerm); i < s; s++)
	{
		if (buffer[i] != searchTerm[i])
		{
			return false;
		}
	}

	return true;
}	
