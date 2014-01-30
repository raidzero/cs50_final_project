//typedef enum {differs, searched, normal} byteStatus;

typedef struct byte
{
	uint8_t value;
	enum {differs, searched, normal} byteStatus;
	//bool differs;
	//bool searched;
} byte;

void StdPrint(char* s, ...);
void ErrPrint(char* s, ...);
void PrintByte(byte, int);
char* PadOffset(long, int);
int LongSize(unsigned long long int);
bool SearchBytes(uint8_t[], char*);
void MarkOffsets(FILE*, offset size, node**, char*);
