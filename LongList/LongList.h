typedef unsigned long long int offset;

// what linked lists are made of
typedef struct node
{
	offset value;
	struct node* next;
} node;

// used to create a new list, returns pointer to head
node* LongList_Create(offset);

// counts elements in list, optionally printing as it goes. returns int
int LongList_Count(node**, int);

// prepends string to list, returns void
void LongList_Prepend(node**, offset);

// appends string to list, returns void
void LongList_Append(node**, offset);

// frees all elements in a list, thereby destroying it
void LongList_Free(node**);

// returns true if string is found in list, false if not
bool LongList_Contains(node**, offset);
