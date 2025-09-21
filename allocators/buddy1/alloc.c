#include "alloc.h"
#include <stdio.h>

// Set initial values of each memory allocation list
void init_alloc()
{

    for (int i = 0; i < NUM_ALLOC_LISTS; i++)
    {
        alloc_lists[i] = NULL;
    }

    alloc_lists[NUM_ALLOC_LISTS - 1] = (LinkedAlloc_t *)heap;
    alloc_lists[NUM_ALLOC_LISTS - 1]->next = NULL;
}


// Find the index of the smallest block that can fit any given size
int8_t find_min_block_index(uint8_t size)
{
    int8_t i = 0;

    // Check all allocation lists
    while (i < NUM_ALLOC_LISTS)
    {
        if (alloc_lists[i] == NULL)
        {
            i++;
            continue;
        }

        // The previous conditional filters out any empty indices
        if (size <= MIN_BLOCK_SIZE << i)
        {
            return i;
        }

        i++;
    }

    // Return -1 if there is not enough free memory
    return -1;
}

void* alloc(uint8_t size)
{

    if (size < MIN_BLOCK_SIZE || size > MAX_BLOCK_SIZE)
    {
        return NULL;
    }

    int8_t index = find_min_block_index(size);

    if (index == -1)
    {
        return NULL;
    }

    LinkedAlloc_t *block = alloc_lists[index];
    alloc_lists[index] = block->next;

    uint32_t block_size = 8 << index;

    // Check if the required size is less than half the size of the block


    while (size >> 1 < block_size)
    {
        // Split block if possible
        if(index == 0)
            break; 

        index--;


        // Split the block and push it to the new free list
        LinkedAlloc_t *new_block = (LinkedAlloc_t *)((uint8_t *)block + (MIN_BLOCK_SIZE << index));
        new_block->next = alloc_lists[index];
        alloc_lists[index] = new_block;

        block_size = 8 << index;
    }



    return (void*)block;
}

void free(void *ptr)
{
    if (ptr == NULL)
    {
        return; // Nothin1g to `free
    }

    // Calculate the block's index based on its size
    uint8_t *block = (uint8_t *)ptr;
    uint8_t index = 0;

    while ((uint8_t *)heap + (8 << index) <= block)
    {
        index++;
    }

    // Add the block back to the free list
    LinkedAlloc_t *freed = (LinkedAlloc_t *)block;
    freed->next = alloc_lists[index];
    alloc_lists[index] = freed;

    merge_alloc_blocks_at_index(index);
}
void merge_alloc_blocks_at_index(uint8_t index)
{
    // Get last block
    LinkedAlloc_t *block = alloc_lists[index];

    // If there's nothing to merge, return
    if (!block || !block->next)
        return;

    LinkedAlloc_t* prev = NULL;

    while (block && block->next)
    {
        if ((uint8_t*)block + (8 << index) == (uint8_t*)block->next) // Check if two blocks are adjacent in memory
        {
            // Merge the blocks
            block->next = block->next->next;

            // Promote the merged block to the next list
            LinkedAlloc_t *merged = block;
            merged->next = alloc_lists[index + 1];
            alloc_lists[index + 1] = merged;

            // If `prev` exists, link it to the next block
            if (prev)
            {
                prev->next = block->next;
            }
            else
            {
                alloc_lists[index] = block->next;
            }

            // Continue merging from the next block
            block = alloc_lists[index];
        }
        else
        {
            // Move to the next block
            prev = block;
            block = block->next;
        }
    }
}

// Set all bytes in some range in memory to some particular value
void* mem_set(void* ptr, uint8_t value, size_t size)
{
    uint8_t* p = (uint8_t*)ptr;

    for (size_t i = 0; i < size; ++i) {
        p[i] = (uint8_t)value;
    }

    return ptr;
}
