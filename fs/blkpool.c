/*******************************************************************************
 This file implements a buffer cache by taking advantage of the concept of
 "temporal locality" (and not "spatial locality")
*******************************************************************************/
#include <fs/blkpool.h>

#include <list.h>
#include <fs/iterator.h>
#include <hdd.h>

#include <string.h>

#define POOL_SIZE   8U
static bufblk_t _[POOL_SIZE];

static list_t _free_list;

typedef void (* iterate_func_t)(bufblk_t *);
typedef bool (* iterate_sel_func_t)(const bufblk_t *, blk_num_t);

DEFINE_ITERATE(_, POOL_SIZE, bufblk_t, iterate_func_t)
DEFINE_ITERATE_SEL(_, POOL_SIZE, bufblk_t, iterate_sel_func_t, blk_num_t)

static inline
void _init(bufblk_t *bufblk)
{
    bzero(bufblk, sizeof(*bufblk));

    node_init(&bufblk->free_node);
    // add buffer block to the free list of buffers
    list_insert(&_free_list, &bufblk->free_node);
}

void blkpool_init()
{
    list_init(&_free_list);

    _iterate(_init);
}

static bool _is_blk_num(const bufblk_t *bufblk, blk_num_t blk_num)
{
    return blk_num == bufblk->num;
}

static int _readblk(blk_num_t blk_num, block_t *blk)
{
    if (hdd_readblk(blk_num, blk))
        return 1;

    return 0;
}

// get the least recently used buffer block from the free list of buffer blocks
static bufblk_t *_get_free()
{
    list_node_t *node;

    if (!(node = list_get_head_node(&_free_list)))
        return NULL; // no free buffers

    node_remove(node);
    return node_get_container(node, bufblk_t, free_node);
}

static inline
bool _is_bufblk_on_free_list(const bufblk_t *bufblk)
{
    // buffer block on the free list <--> not locked
    return !bufblk->locked;
}

bufblk_t *blkpool_getblk(blk_num_t blk_num)
{
    bufblk_t *bufblk;

//beginning:
    if ((bufblk = _iterate_sel(_is_blk_num, blk_num))) {
        if (bufblk->locked) {
            // TODO sleep on event "this buffer block becomes free"
            while (1)
                ;
            //goto beginning;
        }

        if (_is_bufblk_on_free_list(bufblk))
            node_remove(&bufblk->free_node);

        bufblk->locked = true;
        return bufblk;
    }

    // buffer block not found in the pool, pick up a free one
    if (!(bufblk = _get_free())) {
        // TODO msg "no free buffer blocks available"
        return NULL;
    }

    bufblk->locked = true;

    // reset buffer block
    bufblk->num = blk_num;
    bufblk->valid = false;

    // perform a read of the contents of the block on disk
    if (_readblk(blk_num, &bufblk->block)) {
        blkpool_putblk(bufblk);
        return NULL;
    }

    bufblk->valid = true;
    return bufblk;
}


void blkpool_putblk(bufblk_t *bufblk)
{
    bufblk->locked = false;
    list_insert(&_free_list, &bufblk->free_node);
}

unsigned blkpool_get_num_free()
{
    unsigned count;

    for (unsigned i = 0; i < POOL_SIZE; i++)
        if (!_[i].locked)
            count++;

    return count;
}

bufblk_t *blkpool_get_any()
{
    // TODO
    return NULL;
}

// TODO blkpool_flush()
