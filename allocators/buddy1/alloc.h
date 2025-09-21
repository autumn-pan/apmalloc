// Simple internal memory allocator
#ifndef ALLOC_H
#define ALLOC_H
#define HEAP_SIZE 2048
#define MAX_BLOCK_SIZE 1024
#define MIN_BLOCK_SIZE 8
#define NUM_ALLOC_LISTS 7

#include <stdint.h>
#include <stddef.h>

static uint8_t heap[HEAP_SIZE];

typedef struct linkedAlloc_t
{
    struct linkedAlloc_t* next; // ptr to linked list representation
} LinkedAlloc_t;

static LinkedAlloc_t* alloc_lists[NUM_ALLOC_LISTS];

void init_alloc();
void* alloc(uint8_t size);
void free(void *ptr);
void* mem_set(void* ptr, uint8_t value, size_t size);
void merge_alloc_blocks_at_index(uint8_t index);
void update_sleep_handlers();

#endif