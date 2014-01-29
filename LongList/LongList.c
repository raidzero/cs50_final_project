/*
 * Linked list implementation that does not use global pointers
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LongList.h"

#define TESTING 0

// returns pointer to a new list (first element)
node* LongList_Create(offset value)
{
	// make a new node 
	node* head = malloc(sizeof(node));
   
	head->value = value;
	head->next = NULL;
	
	return head;
}

// prints the elements in the list, returns size
int LongList_Count(node** head, int print)
{
	// make a pointer used for traversing
	node* selector = *head;
	
	// make a pointer used to keep track of the previous node
	node* prev = *head;
	
	int count = 0;
	// now just go through, printing values until next is null
	do
	{		
		// remember what was before this
		prev = selector;
		count++;

		if (print == 1)
		{
			printf("%d: 0x%llx\n", count, selector->value);
		}
		
		// update selector to point the next node
		selector = selector->next;
	}
	while (prev->next != NULL);

	return count;
}

// prepend an element and return pointer to new head
void LongList_Prepend(node** head, offset value)
{
	node* new_head = malloc(sizeof(node));
	
	new_head->value = value;
	new_head->next = *head;
	
	// update the pointer to point here
	*head = new_head;
}

// add element to end, returns vois
void LongList_Append(node** head, offset value)
{
	// intermediate pointers
	node* selector = *head;
	node* prev = *head;
	
	// make the new end node
	node* new_end = malloc(sizeof(node));
	new_end->value = value;
	new_end->next = NULL;

	// go to the end
	do 
	{
		if (selector->next == NULL)
		{
			// add the new end of list
			selector->next = new_end;
			return;
		}
		prev = selector;
		selector = selector->next;
	}
	while (prev->next != NULL);
}

// frees all pointers in a list, from end to start
void LongList_free(node** head)
{
	// set up intermediate pointers
	node* selector = *head;
	node* prev = *head;

	// loop through, freeing the previous and moving to next 
	do 
	{
		prev = selector;
		// printf("freeing %p\n", prev);	
		if (prev != NULL)
		{
		    free(prev);
		}
		selector = selector->next;
	}    	
	while (selector != NULL);
}

bool LongList_Contains(node** head, offset s)
{
    node* selector = *head;
    node* prev = *head;
	
	if (selector->value == s)
	{
		return true;
	}
	
    //LongList_Count(head, 1);
	do 
	{
		prev = selector;
		if (prev != NULL && selector->value)
		{
		    if (selector->value == s)
		    {
				return true;
		    }
		}
		selector = selector->next;	    
	}    	
	while (selector != NULL);
    
    return false;
}

#if TESTING == 1
// lets test this thing
int main(void)
{
	// printf("Welcome!\n");	
	
	// set up new list
	node* first = LongList_Create(0x104);
    node* second = LongList_Create(0x506);
    
    // test prepend/append
	LongList_Append(&first, 0x832674);
	LongList_Prepend(&first, 0x198237);
	LongList_Append(&second, 0x1111);
	LongList_Prepend(&second, 0xdeadbeef);

	// test prepend/append
	LongList_Append(&first, 0x138123);
	LongList_Prepend(&first, 0x9876543);

	// print the lists
	printf("\n%d elements in first list.\n", LongList_Count(&first, 1));
    printf("\n%d elements in second list.\n\n", LongList_Count(&second, 1));
    
    printf("second contains 0xdeadbeef? ");
    bool contains = Longlist_Contains(&second, 0xdeadbeef);
    if (contains)
    {
        printf("Yes!\n");
    }
    else
    {
        printf("No!\n");
    }
  	// free then
	LongList_Free(&first);
    LongList_Free(&second);
}  
#endif
