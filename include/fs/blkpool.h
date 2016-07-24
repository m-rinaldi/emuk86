#pragma once

#include <fs/block.h>

#include <stdbool.h>

typedef struct {
    bool        valid;
    bool        locked;
    blk_num_t   blk_num;
    block_t     block; 
} bufblk_t;


void blkpool_init();
bufblk_t *blkpool_getblk(blk_num_t);
void blkpool_putblk(bufblk_t *);
unsigned blkpool_get_num_free();
