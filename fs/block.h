#pragma once

#include <stdint.h>

// disk block
#define BLOCK_SIZE  1024
typedef struct {
    uint8_t data[BLOCK_SIZE]; 
} block_t;

typedef uint32_t blk_num_t;
