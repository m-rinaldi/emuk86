#pragma once

#include <fs/block.h>
#include <list.h>
#include <stdbool.h>

typedef struct {
    bool        valid;
    bool        locked;

    // TODO add dirty flag
    blk_num_t   num;
    block_t     block;

    list_node_t free_node;
} bufblk_t;


void blkpool_init();
bufblk_t *blkpool_getblk(blk_num_t);
bufblk_t *blkpool_get_any();
void blkpool_putblk(bufblk_t *);
unsigned blkpool_get_num_free();
