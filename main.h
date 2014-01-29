typedef struct byte
{
	uint8_t value;
	bool differs;
} byte;

void StdPrint(char* s, ...);
void ErrPrint(char* s, ...);
void PrintByte(byte, int);
char* PadOffset(long, int);
int LongSize(unsigned long long int);
