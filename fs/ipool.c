#include <ipool.h>

#include <ext2.h>
#include <list.h>
#include <inode.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>

#define POOL_SIZE   8
static inode_t _[POOL_SIZE];

static list_t _free_list;

typedef bool (*imatch_func_t)(const inode_t *, ...);

static inline
void _init(inode_t *ino)
{
    bzero(ino, sizeof(*ino));

    node_init(&ino->free_node);
    // add inode to the free list of inodes
    list_insert(&_free_list, &ino->free_node);
}

void ipool_init()
{
    list_init(&_free_list);

    for (int i = 0; i < POOL_SIZE; i++)
        _init(_ + i);
}

// iterates over the pool
static inode_t *_iterate(imatch_func_t does_match, ino_num_t ino_num)
{
    for (int i = 0; i < POOL_SIZE; i++)
        if (does_match(_ + i, ino_num))
            return _ + i;

    return NULL;
}

static bool _is_ino_num(const inode_t *ino, ...)
{
    va_list ap;
    ino_num_t ino_num;
    
    va_start(ap, ino);
    ino_num = va_arg(ap, ino_num_t);
    va_end(ap);

    return ino_num == ino->num;
}

// get any inode from the free list of inodes
static inode_t *_get_free_inode()
{
    list_node_t *node;

    if ((node = list_get_head_node(&_free_list)))
        node_remove(node);

    // TODO use a macro with offsetof()
    return list_get_entry(node, inode_t);
}

static void _release_inode(inode_t *ino)
{
    inode_unlock(ino);

    if (!--ino->count)
        list_insert(&_free_list, &ino->free_node);
}

static inline
bool _is_inode_on_free_list(const inode_t *ino)
{
    // inode on the free list <--> count is zero
    return !ino->count;
}

inode_t *ipool_geti(ino_num_t ino_num)
{
    inode_t *ino;

//beginning:
    if ((ino =_iterate(_is_ino_num, ino_num))) {
        if (inode_is_locked(ino)) {
            // TODO sleep on event "this inode becomes free"
            // TODO goto beginning;
            return NULL;
        }

        if (_is_inode_on_free_list(ino))
            node_remove(&ino->free_node);

        inode_lock(ino);
        ino->count++;

        return ino;
    }
    
    // inode not found in the pool, pick up a free one
    if (!(ino = _get_free_inode()))
        return NULL;

    inode_lock(ino);
    ino->count++;

    // reset inode
    ino->num = ino_num;
    ino->valid = false;

    // perform a read of the contents of the inode on disk
    if (ext2_readi(ino_num, &ino->dinode))
        goto err;

    ino->valid = true;

    return ino; // no free inode

err:
    _release_inode(ino);
    return NULL;
}


void ipool_puti(inode_t *ino)
{
    _release_inode(ino);
}
