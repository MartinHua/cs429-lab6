#include "my_mem.h"
#include <stdlib.h>
#include <stdio.h>


/* 
 * You CAN'T allocate a global static array in global memory, for example,
 * you CAN'T do node[50], you HAVE to use pointers, as many as you want. You may
 * need more than one linked list, I (your TA) use only one list but keep/maintain information about the nodes 
 * in a specific way.
 * 
 * Feel free to use a different data structure, single/doubly linked lists, trees, w/e, all up to you.
 * 
 * Also, to make sure you do it correctly; all structures that you use for bookkeeping, you will use
 * the system's malloc, not yours. Only user's data will reside in the heap you are managing.
 */

//example: head of a list of nodes
Node* head;

//TODO: other pointers you may need to use here...
Node* cur;
Node* pre;
Node* best;
Node* freeNode;
int best_size;

//This is your "heap". you will reserve/allocate and manage memory from this block.
//if you run out of memory or can't allocate any more, you should "throw" a
//segmentation fault (your turn to get back to those pesky segfaults that always
//bother you) and exit the program with an error (there's an example below)
//You won't need to extend/increase this heap.
//Note: In real OS, what happens when you do allocate more than what you have?
//      well, run "man sbrk" on your terminal, you'll learn how to use it in the OS class
byte_ptr heap;

//just a toggle flag to initialize only once
static char mm_inited = 0;

void my_mminit()
{
    heap = (byte_ptr) malloc (HEAP_SIZE);
    if (heap == 0) {
        printf("ERROR during mm init malloc\n");
        exit(1);
    }
    mm_inited = 1;

    //TODO: initialize your data structure(s) here
    Node* n = malloc(sizeof(Node));
    n->next = NULL;
    n->offset = 0;
    n->size = HEAP_SIZE;
    n->state = F;
    head = n;
}

//you shouldn't need to change this, but feel free to do so if you want
//it's just a switch to choose allocation policy during compile time
//and an init that only runs once
void* mm_malloc(size_t size) 
{
    if (!mm_inited) 
        my_mminit();
 
    #ifdef BESTFIT
        return mm_best_fit_malloc(size);
    #elif FIRSTFIT
        return mm_first_fit_malloc(size);
    #else
        printf("memory system policy undefined\n");
        exit(1);
    #endif
}

/*
 *
 * This is where you will write your code.
 * 
 */

void mm_free(void* ptr)
{
    //TODO: do your work here

    //if ptr is invalid (does not point to the start
    //of a memory block you allocated, emulate a SEGFAULT:
    if (ptr == 0) {
        printf("Segmentation fault.\n");
        exit(1);
    }

    pre = NULL;
    cur = head;
    while (cur != NULL && heap + cur->offset < ptr) {
        pre = cur;
        cur = cur->next;
    }
    if (cur == NULL || heap + cur->offset > ptr || cur->state == F) {
        printf("Segmentation fault.\n");
        exit(1);
    }
    cur->state = F;
    if (pre != NULL && pre->state == F) {
        cur = pre;
    }
    while (cur->next != NULL) {
        if (cur->next->state == A) {
            break;
        }
        freeNode = cur->next;
        cur->size += freeNode->size;
        cur->next = freeNode->next;
        cur = cur->next;
        free(freeNode);
    }
}

void* mm_best_fit_malloc (size_t size) 
{
    //TODO: .. some code ..
    //again, to make sure you do this correctly, if you need to allocate some node in a linked list or whatever
    //structure you use, use malloc, eg., Node* n = malloc(sizeof(Node))
    //...some code..
    cur = head;
    best = NULL;
    best_size = HEAP_SIZE + 1;
    while (cur != NULL) {
        if (cur->state == F && cur->size >= size) {
            if (cur->size == size) {
                cur->state = A;
                return heap + cur->offset;
            } else if (cur->size < best_size){
                best = cur;
                best_size = best->size;
            }
        }
        cur = cur->next;
    }
    if (best != NULL) {
        Node* n = malloc(sizeof(Node));
        n->next = best->next;
        n->offset = best->offset + best->size - size;
        n->size = size;
        n->state = A;

        best->next = n;
        best->size -= size;
        return heap + n->offset;
    }
    printf("Segmentation fault.\n");
    exit(1);
    //return a pointer to some address in the allocated 32MB heap, for example, the first time someone
    //calls mm_malloc, you will likely: "return heap+HEAP_SIZE-size", because there's only one free block in the heap (the
    //entire heap) and you're returning a pointer to the rightmost 'size' bytes of the heap that the user can then use
    //however it wants, the same way one can use malloc

    //your return value HAS TO BE a pointer inside the heap: [heap, heap+size)

    
    /*******************************
     ******  THE MALLOC BELOW IS JUST A PLACEHOLDER SO THAT main CAN RUN.
     ******  YOUR TASK IS TO RESERVE MEMORY FROM byte_ptr heap; AND ONLY USE
     ******  malloc FOR METADATA LIKE LISTS, ETC. YOU SHOULD DEFINITELY DELETE
     ******  THIS LINE WHEN YOU'RE DOING THIS LAB
    *******************************/ 
    return malloc(size);
}

void* mm_first_fit_malloc (size_t size) 
{
    /*
     *   TODO: Your code here
     */ 
    cur = head;
    while (cur != NULL) {
        if (cur->state == F && cur->size >= size) {
            if (cur->size == size) {
                cur->state = A;
                return heap + cur->offset;
            } else {
                Node* n = malloc(sizeof(Node));
                n->next = cur->next;
                n->offset = cur->offset + cur->size - size;
                n->size = size;
                n->state = A;

                cur->next = n;
                cur->size -= size;
                return heap + n->offset;
            }
        }
        cur = cur->next;
    }
    printf("Segmentation fault.\n");
    exit(1);
    /*******************************
     ******  THE MALLOC BELOW IS JUST A PLACEHOLDER SO THAT main CAN RUN.
     ******  YOUR TASK IS TO RESERVE MEMORY FROM byte_ptr heap; AND ONLY USE
     ******  malloc FOR METADATA LIKE LISTS, ETC. YOU SHOULD DEFINITELY DELETE
     ******  THIS LINE WHEN YOU'RE DOING THIS LAB
    *******************************/ 
    return malloc(size);
}

//this is the function that will be used to grade you, make sure to print in the
//correct format according to the pdf.
void mm_print_heap_status(FILE* fout)
{
    /*
     *   TODO: Your code here
     */
    cur = head;
    while (cur->next != NULL) {
        fprintf(fout, "%lu%s ", cur->size, cur->state == F ? "F" : "A");
        cur = cur->next;
    }
    fprintf(fout, "%lu%s\n", cur->size, cur->state == F ? "F" : "A");
    /*
     *   Your code here. Because you are writing a file or maybe stdout (your terminal), instead of using printf, use fprintf
     *   It has the exact same syntax, but the first parameter is fout. This function assumes the file already exists. 
     *   Check test.c if you want to see how this works. You shouldn't worry about it, just use fprintf as the example below.
     *   eg: fprintf(fout, "some string %d  %s\n", parameter1, parameter2)
     */ 

}

void  mm_defragment()
{
    /*
     *   TODO: Your code here
     */

    /*
     * Push all occupied blocks to the right, and make one big free block on the left.
     */ 
}


//Maybe creating linked list functionalities in functions is a good idea
//void add_node(Node* head, Node* new_node)
//if you do, declare those on the header file and put their code here.