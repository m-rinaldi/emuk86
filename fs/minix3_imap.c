#include <fs/minix3_imap.h>

#include <fs/block.h>
#include <fs/inode.h>

#include <stdbool.h>

#include "blkbit.h"

#define BITS_PER_BLOCK      (8*BLOCK_SIZE)

/*
    minix3 filesystem inode bitmap:
        bit set to 0    --> corresponding inode is free
        bit set to 1    --> corresponding inode is being used
 */

static bool         _locked;
static ino_num_t    _num_inodes;
static unsigned     _num_blks; // TODO change to _imap_num_blks

// TODO change "unsigned" to "ino_num_t" since this is nothing else than a count
void minix3_imap_init(unsigned num_inodes)
{
    _num_inodes = num_inodes;
    _num_blks = num_inodes / BLOCK_SIZE;
}

void minix3_imap_lock()
{
    // TODO
    while (_locked)
        ;
    _locked = true;
}

void minix3_imap_unlock()
{
    _locked = false;
}

static int _ino2blk(ino_num_t ino_num, blk_num_t *blk_num, unsigned *bit_loff)
{
    blk_num_t blk_off;

    if ((blk_off = ino_num / BITS_PER_BLOCK) >= _num_blks)
        return 1;

    *blk_num  = MINIX3_IMAP_STARTING_BLK_NUM + blk_off;
    *bit_loff = ino_num % BITS_PER_BLOCK;

    return 0;
}

static int _set_ibit_value(ino_num_t ino_num, unsigned bit_value)
{
    blk_num_t blk_num;
    unsigned bit_loff;

    if (_ino2blk(ino_num, &blk_num, &bit_loff))
        return 1;
   
    if (_blkbit_set_value(blk_num, bit_loff, bit_value))
        return 1;

    return 0;
}

static int _get_ibit_value(ino_num_t ino_num, unsigned *bit_value)
{
    blk_num_t blk_num;
    unsigned bit_loff;
    bool bit_val;

    if (_ino2blk(ino_num, &blk_num, &bit_loff))
        return 1;
   
    if (_blkbit_get_value(blk_num, bit_loff, &bit_val))
        return 1;

    *bit_value = bit_val;

    return 0;
}

static bool _is_inode_used(ino_num_t ino_num)
{
    unsigned bit_value;

    _get_ibit_value(ino_num, &bit_value);
    return bit_value;
}

int minix3_imap_seti(ino_num_t ino_num)
{
    if (_set_ibit_value(ino_num, 1))
        return 1;

    return 0;
}

int minix3_imap_clri(ino_num_t ino_num)
{
    if (_set_ibit_value(ino_num, 0))
        return 1;

    return 0;
}

ino_num_t minix3_imap_get_free()
{
    // TODO make efficient by keeping track of the last allocated inode
    for (unsigned i = 0; i < _num_inodes; i++)
        if (!_is_inode_used(i))
            return i;                 
 
    return 0; // no free inodes
}
