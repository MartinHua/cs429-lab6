#include <stdlib.h>
#include <stdio.h>
#include "defines.h"

//You should declare the structures in the header file
//this is a start of a linked list. You're free to ignore this and build your own
//or extend this
typedef struct Node Node;
#define F 0
#define A 1

struct Node {
    Node* next;
    //some other fields...
    size_t size;
    size_t offset;
    int state;
};


void* mm_best_fit_malloc (size_t size);
void* mm_first_fit_malloc (size_t size);
void* mm_malloc (size_t size);
void  mm_free (void* ptr);
void  mm_print_heap_status (FILE*);
void  mm_defragment();