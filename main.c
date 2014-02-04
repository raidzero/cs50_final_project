#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "LongList/LongList.h"
#include "ByteArray/ByteArray.h"

#include "main.h"

#define NORMAL "\033[0m"
#define DIFFERS "\033[31m"
#define SEARCHED "\033[32m"

// globals 
bool search;
typedef enum {hexSearch, asciiSearch} enum_searchType;

int main(int argc, char* argv[])
{
	// check for args
	if (argc < 3)
	{
		ErrPrint("USAGE: %s FILE1 FILE2 [-as ASCIISEARCH] [-c COLUMNS]\n", argv[0]);
	}
	
	char* searchTerm = "";
	byteArray* searchBytes;
	enum_searchType searchType = asciiSearch;

	int bufferSize = 0;
	
	search = false;
		
	char* filename1 = argv[1];
	char* filename2 = argv[2];
	
	// loop over arguments passed, starting at argv[3] - after filenames
	for (int i = 3; i < argc; i++)
	{
		if (strcmp(argv[i], "-c") ==  0)
		{
			bufferSize = atoi(argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-as") == 0)
		{
			searchTerm = argv[++i];
			searchType = asciiSearch;
			continue;
		}
		if (strcmp(argv[i], "-bs") == 0)
		{
			searchTerm = argv[++i];
			searchType = hexSearch;
			continue;
		}
	}

	if (strlen(searchTerm) > 0)
	{
		search = true;
		if (searchType == hexSearch)
		{
			searchBytes = SplitBytes(searchTerm, hex);
		}
		else
		{
			searchBytes = SplitBytes(searchTerm, ascii);
		}
		printf("searchTerm: \"%s\" (%zu bytes)\n", searchTerm, searchBytes->size);
		
	}
		
	// open up the two files
	FILE* f1 = fopen(filename1, "rb");
	FILE* f2 = fopen(filename2, "rb");
	
	if (f1 == NULL || f2 == NULL)
	{
		ErrPrint("File error!\n");
		
		if (f1)
		{
			fclose(f1);
		}
		if (f2)
		{
			fclose(f2);
		}
	}

	// how long is each file?
	fseek(f1, 0, SEEK_END);
	fseek(f2, 0, SEEK_END);

	offset f1_size = ftell(f1);
	offset f2_size = ftell(f2);
	
	int offsetLen = f1_size > f2_size ? LongSize(f1_size) : LongSize(f1_size);

	// rewind files
	rewind(f1); rewind(f2);

	// determine current window size - if it hasnt been specified already
	struct winsize ws;

	if (bufferSize == 0)
	{
		if (!ioctl(1, TIOCGWINSZ, &ws)) // thanks to the ioctl UNIX docs
		{
			// divide by 4 for each file, so lets go with 8
			bufferSize = (ws.ws_col / 8 - offsetLen);
		}
		else
		{
			// could not read terminal size. Just go with 12
			bufferSize = 12;
		}
	}

	// we need two lists for storing file offsets where found bytes live
	// null so we can tell if its been created or not
	node* f1_offsets = (node*) NULL;
	node* f2_offsets = (node*) NULL;
	
	bool reading = true;
	
	if (search)
	{
		// check files
		MarkOffsets(f1, f1_size, &f1_offsets, searchBytes);
		MarkOffsets(f2, f2_size, &f2_offsets, searchBytes);
	}
	
	// stack allocate for simplicity
	byte f1_bytes[bufferSize];
	byte f2_bytes[bufferSize];
	
	// byte buffers
	uint8_t buffer1[bufferSize];
	uint8_t buffer2[bufferSize];

	size_t b1_read, b2_read = 0;
	
	offset offset1 = 0, offset2 = 0;
	offset longerOffset = 0;

	reading = true;
	
	while (reading)
	{
		// read bufferSize bytes from files
		b1_read = fread(&buffer1, 1, bufferSize, f1);
		b2_read = fread(&buffer2, 1, bufferSize, f2);
	    
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
				
				// check diff first
				if (b1.value != b2.value)
				{
					b1.byteStatus = differs;
					b2.byteStatus = differs;
				}
				else
				{
					b1.byteStatus = normal;
					b2.byteStatus = normal;
				}
				
				// have we passed one of the file's boundaries?
				if (offset1 > f2_size)
				{
					b1.byteStatus = differs;
				}

				if (offset2 > f1_size)
				{
					b2.byteStatus = differs;
				}

				// now searched, color searched over diff
				
				if (search)
				{
					//printf("checking f1: %llu\n", offset1 + i);
					if (LongList_Contains(&f1_offsets, offset1 + i))
					{
						//printf("f1, hello\n");
						b1.byteStatus = searched;
					}
					
					//printf("checking f2: %llu\n", offset2 + i);
					if (LongList_Contains(&f2_offsets, offset2 + i))
					{
						//printf("f2, hello\n");
						b2.byteStatus = searched;
					}
				}
				
				f1_bytes[i] = b1;
				f2_bytes[i] = b2;
			}
			
			// determine which offset is what we display
			longerOffset = offset1 > offset2 ? offset1: offset2;
			
			// pad offset and print it
			char* paddedOffset = PadOffset(longerOffset, offsetLen);
			fprintf(stdout, "0x0%s | ", paddedOffset);
			free(paddedOffset);
			
			offset1 += b1_read;
			offset2 += b2_read;

			// now print the bytes hex
			for (i = 0; i < b1_read; i++)
			{
				PrintByte(f1_bytes[i], 0);
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
				PrintByte(f1_bytes[i], 1);
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
				PrintByte(f2_bytes[i], 0);
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
				PrintByte(f2_bytes[i], 1);
			}

			printf("\n");
		}
		else
		{
			reading = false;
		}	
	}
	
	if (searchBytes)
	{
		ByteArray_Free(searchBytes);
	}
	if (search)
	{
		LongList_Free(&f1_offsets);
		LongList_Free(&f2_offsets);
	}

	fclose(f1);
	fclose(f2);
		
	return 0;
}

void StdPrint(char* s, ...)
{
	va_list args;
	va_start(args, s);

	vfprintf(stdout, s, args);
}

void ErrPrint(char* s, ...)
{
	va_list args;
	va_start(args, s);

	vfprintf(stderr, s, args);
	exit(1);
}


void PrintByte(byte b, int ascii)
{
	char pad = '\0';
	
	if (ascii == 0) // hex
	{
		if (b.value <= 0xf)
		{
			pad = '0';
		}
		
		switch (b.byteStatus)
		{
			case searched:
				StdPrint("%s%c%x ", SEARCHED, pad, b.value);
				break;
			case differs:
				StdPrint("%s%c%x ", DIFFERS, pad, b.value);
				break;
			case normal:
				StdPrint("%s%c%x ", NORMAL, pad, b.value);
				break;
		}
	
		goto end;
	}
	else // ascii
	{
		// nonprintables?
		if (!isprint(b.value) && 
		b.value != ' ')
		{
			switch (b.byteStatus)
			{
				case searched:
					StdPrint("%s.", SEARCHED);
					goto end;
				case differs:
					StdPrint("%s.", DIFFERS);
					goto end;
				case normal:
					StdPrint("%s.", NORMAL);
					break;
			}
			goto end;
		}

		if (b.value <= 0xf)
		{
			pad = ' ';
		}
		
		switch (b.byteStatus)
		{
			case searched:
				StdPrint("%s%c%c", SEARCHED, pad, (char) b.value);
				goto end;
			case differs:
				StdPrint("%s%c%c", DIFFERS, pad, (char) b.value);
				goto end;
			case normal:
				StdPrint("%s%c%c", NORMAL, pad, (char) b.value);
				goto end;
		}		
	}

	end:
	StdPrint("%s", NORMAL);
}

char* PadOffset(long offset, int size)
{
	int offsetSize = LongSize(offset);

	char chrOffset[offsetSize];
	sprintf(chrOffset, "%lx", offset);

	// alloc mem for result
	char* result = malloc(size + 1 * sizeof(char));
	
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
	
	// all strings need null terminators or they dont go to heaven
	result[size] = '\0';

	return result;
}

int LongSize(unsigned long long int number)
{
    char numSize[512];
    
    sprintf(numSize, "%llx", number);
    
    return strlen(numSize);
}

bool SearchBytes(uint8_t buffer[], byteArray* searchBytes)
{
	for (int i = 0, s = searchBytes->size; i < s; i++)
	{		
		if (buffer[i] != searchBytes->value[i])
		{
			return false;
		}
	}
	return true;
}

void MarkOffsets(FILE* fp, offset size, node** list, byteArray* searchBytes)
{
	//printf("Marking\n============================\n");
	rewind(fp);

	size_t sLen = searchBytes->size;

	size_t bytesRead = 0;
	
	// search buffer
	uint8_t searchBuffer[sLen];

	// iterate over each offset in file
	for (offset loc = 0; loc < size; loc++)
	{
		// move to loc in file
		fseek(fp, loc, SEEK_SET);

		// read a chunk of data from the file the size of what we are looking for
		bytesRead = fread(&searchBuffer, 1, sLen, fp);
		if (bytesRead == sLen)
		{
			// compare this chunk
			if (SearchBytes(searchBuffer, searchBytes))
			{
				// save the offsets in this word in the file
				for (int count = 0; count < bytesRead; count++)
				{
					offset foundLoc = loc + count;

					if (*list != NULL)
					{
						//printf("Saved offset %llu in existing list\n", foundLoc);
						LongList_Append(list, foundLoc);
					}
					else
					{
						//printf("Saved offset %llu in new list\n", foundLoc);
						*list = LongList_Create(foundLoc);
					}
				}

				// move forward in file
				loc += bytesRead;
			}
		}
		else 
		{
			// not enough bytes read
			break;
		}
	}

	//printf("============================\nDone\n");
	rewind(fp);
}
