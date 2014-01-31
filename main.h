//typedef enum {differs, searched, normal} byteStatus;

typedef struct byte
{
	uint8_t value;
	enum {differs, searched, normal} byteStatus;
} byte;

void StdPrint(char* s, ...);
void ErrPrint(char* s, ...);
void PrintByte(byte, int);
char* PadOffset(long, int);
int LongSize(unsigned long long int);
bool SearchBytes(uint8_t[], byteArray*);
void MarkOffsets(FILE*, offset size, node**, byteArray*);
