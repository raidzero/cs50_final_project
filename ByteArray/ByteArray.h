typedef enum {ascii, hex} splitType;
typedef struct byteArray 
{
	uint8_t* value;
	size_t size;
} byteArray;

byteArray* SplitBytes(char*, splitType);
void ByteArray_Free(byteArray*);
