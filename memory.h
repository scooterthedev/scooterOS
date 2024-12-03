#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define BLOCKS_COUNT 1024

typedef struct {
    uint32_t size;
    uint8_t used;
} memory_block_t;

void memory_init(void);
void* kmalloc(size_t size);

#endif 