#include <blkpool.h>

#include <hdd.h>

#include <string.h>

#define POOL_SIZE   8U
static bufblk_t _[POOL_SIZE];

// TODO use list.h

static inline
void _init(bufblk_t *bufblk)
{
    bzero(bufblk, sizeof(*bufblk));
}

void blkpool_init()
{
    for (unsigned i = 0; i < POOL_SIZE; i++)
        _init(_ + i);
}

static int _readblk(blk_num_t blk_num, block_t *blk)
{
    if (hdd_readblk(blk_num, blk))
        return 1;

    return 0;
}

static bufblk_t *_get_free()
{
    for (unsigned i = 0; i < POOL_SIZE; i++)
        if (!_[i].locked) {
            _[i].locked = true;
            return _ + i;
        }

    return NULL; // no free bufs
}


bufblk_t *blkpool_getblk(blk_num_t blk_num)
{
    for (unsigned i = 0; i < POOL_SIZE; i++)
        if (_[i].valid && blk_num == _[i].blk_num) {
            // TODO if locked fall asleep and retry
            
            _[i].locked = true;
            return _ + i; 
        }

    // block not found in the buffer pool

    bufblk_t *bufblk;

    if (!(bufblk = _get_free())) {
        // TODO error msg: no free buffer blocks available
        return NULL;
    }

    if (_readblk(blk_num, &bufblk->block)) {
        blkpool_putblk(bufblk);
        return NULL;
    }

    return bufblk;
}


void blkpool_putblk(bufblk_t *bufblk)
{
    bufblk->locked = false;
}

unsigned blkpool_get_num_free()
{
    unsigned count;

    for (unsigned i = 0; i < POOL_SIZE; i++)
        if (!_[i].locked)
            count++;

    return count;
}
