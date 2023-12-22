#ifndef BLOCK_LIST_H
#define BLOCK_LIST_H

typedef struct block_list 
{
    int size;
    int is_free;
    struct block_list *next;
} block_t;

#endif /* ifndef _BLOCK_LIST_H */



