#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <malloc.h>
#include <stdio.h>
#include "block_list.h"
#include <unistd.h>
#include <debug.h> // definition of debug_printf
#include <string.h> 


block_t *block_head = NULL;


// calls to sbrk to request more space from the OS. Sets the size of the block to the size of the requested space + the size of the block_t struct.
// Sets the is_free flag to 0. Returns a pointer to the block_t struct.
block_t *request_space(size_t s)
{
  block_t *block = sbrk(s + sizeof(block_t));
  if (block == (void *)-1)
  {
    return NULL;
  }
  block->size = s;
  block->next = NULL;
  return block;
}


/*
Allocates a block of memory of size s + the block_t struct and returns a pointer to the space in memory after the block_t struct.
*/
void *mymalloc(size_t s)
{
  debug_printf("malloc %zu bytes\n", s);

  block_t *prev_block = NULL;
  block_t *curr_block = block_head;
  while (curr_block != NULL) // loops through the linked list of block_t structs. First fit implementation.
  {
    if (curr_block->is_free == 1 && curr_block->size >= s)
    {
      break;
    }
    prev_block = curr_block;
    curr_block = curr_block->next; // sets curr_block to the next block_t struct in the linked list to keep looping through the linked list.
                                   // If we find the end of the list, curr_block will be NULL.
  }

  if (curr_block == NULL) // if we reach the end of the list, we need to request more space from the OS.
  { 

	  if (block_head == NULL){
	  block_head = request_space(s);
	  curr_block = block_head;  // so we have to make sure that we update the curr_block after we allocate block_head otherwise it would return a NULL to the user and give a segmentation fault
	  } else {
    curr_block = request_space(s); // calls to sbrk to request more space from the OS. Sets the size of the block to the size of the requested space + the size of the block_t struct.
	  }
    if (curr_block == NULL) // if we get NULL back from sbrk, we are out of memory.
    {
      return NULL; // return NULL to indicate we are out of memory.
    }
    
    if (prev_block != NULL) // if we are not at the beginning of the list, we need to set the next pointer of the previous block_t struct to the current block_t struct.
    {
      prev_block->next = curr_block;
    }
    
  }
  curr_block->is_free = 0; // sets the is_free flag to 0
  return (void *)(curr_block + 1); // returns a pointer to the open space in memory, not the block_t struct
}

void *mycalloc(size_t nmemb, size_t s)
{

  void *p = mymalloc(nmemb * s); // calls to mymalloc to allocate the space in memory.
  if (p == NULL) // if we get NULL back from mymalloc, we are out of memory.
  {
    return NULL; // return NULL to indicate we are out of memory.
  }
  memset(p, 0, nmemb * s); // sets the memory to 0.

  debug_printf("calloc %zu bytes\n", s);

  return p;
}

// sets the is_free flag to 1.
// However the ptr is refering to the open space in memory, not the block_t struct.
// so we need to subtract the size of the block_t struct from the pointer to get the block_t struct.
void myfree(void *ptr)
{
  debug_printf("Freed some memory\n");
  block_t *block = ((block_t *)ptr) - 1; // sets block to the block_t struct that is before the pointer passed in.
  block->is_free = 1; // frees the block by setting the is_free flag to 1.
}

