#pragma once

#include <ext2.h>
#include <list.h>

#include <stdbool.h>

#define ROOT_INODE_NUM  2

typedef struct st_inode {
    list_node_t     free_node;

    bool            valid;
    ino_num_t       num;

    unsigned        count;
    bool            locked;
    bool            dirty;

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

static inline
bool inode_is_dir(const inode_t *ino)
{
    return ino->dinode.mode & EXT2_S_IFDIR;
}
