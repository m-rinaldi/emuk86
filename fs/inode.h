#pragma once

#include <ext2.h>

#include <stdbool.h>

typedef struct st_inode {
    bool            valid;
    ino_num_t       num;

    unsigned        count;
    bool            locked;

    ext2_inode_t    dinode;
} inode_t;

static inline
void inode_lock(inode_t *ino)
{
    ino->locked = true;
}

static inline
void inode_unlock(inode_t *ino)
{
    ino->locked = false;
}

static inline
bool inode_is_locked(const inode_t *ino)
{
    return ino->locked;
}
