#pragma once

#include <list.h>
#include <fs.h>
#include <minix3_inode.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct st_inode {
    bool            valid;
    ino_num_t       num;

    unsigned        count;
    bool            locked;
    bool            dirty;

    minix3_inode_t  dinode;     // actual data

    list_node_t     free_node;
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

static inline
bool inode_is_dir(const inode_t *ino)
{
    // TODO this function is not independent from file system
    return ino->dinode.i_mode & MINIX3_S_IFDIR;
}
