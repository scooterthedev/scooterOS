#include "memory.h"

memory_block_t memory_blocks[BLOCKS_COUNT];

void memory_init() {
    for(int i = 0; i < BLOCKS_COUNT; i++) {
        memory_blocks[i].size = BLOCK_SIZE;
        memory_blocks[i].used = 0;
    }
}

void* kmalloc(size_t size) {
    for(int i = 0; i < BLOCKS_COUNT; i++) {
        if(!memory_blocks[i].used && memory_blocks[i].size >= size) {
            memory_blocks[i].used = 1;
            return (void*)(BLOCK_SIZE * i);
        }
    }
    return NULL;
} 