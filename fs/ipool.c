#include <ipool.h>

#include <ext2.h>
#include <inode.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>

#define POOL_SIZE   8
static inode_t _[POOL_SIZE];

typedef bool (*imatch_func_t)(const inode_t *, ...);

static inline
void _init(inode_t *ino)
{
    bzero(ino, sizeof(*ino));
}

void ipool_init()
{
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

static bool _is_ino_free(const inode_t *ino, ...)
{
    return !inode_is_locked(ino);
}

static inode_t *_get_free()
{
    // dummy inode number
    return _iterate(_is_ino_free, 0);
}

inode_t *ipool_geti(ino_num_t ino_num)
{
    inode_t *ino;

    if ((ino =_iterate(_is_ino_num, ino_num))) {
        if (inode_is_locked(ino)) {
            // TODO sleep on event "this inode becomes free"
            return NULL;
        }

        inode_lock(ino);
        ino->count++;

        return ino;
    }
    
    // inode not found in the pool, pick up a free one
    if (!(ino = _get_free())) 
        return NULL;

    inode_lock(ino);

    // perform a read of the contents of the inode on disk
    if (ext2_readi(ino_num, &ino->dinode))
        goto err;

    ino->count++;

    return ino; // no free inode

err:
    inode_unlock(ino);
    return NULL;
}

void ipool_puti(inode_t *ino)
{
    if (!--ino->count)
        ino->locked = false;
}
