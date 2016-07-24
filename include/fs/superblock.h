#pragma once

#include <stdbool.h>
#include <fs/minix3_superblock.h>

typedef struct {
    bool    valid;
    bool    locked;
    bool    dirty;

    // actual superblock
    const minix3_superblock_t   *_;
} superblock_t;

superblock_t *superblock_get();
void superblock_put(superblock_t *);
