#include <fs/minix3_zmap.h>

#include <fs/block.h>

#include <stdbool.h>

#include "blkbit.h"

#define BITS_PER_BLOCK      (8*BLOCK_SIZE)

/*
    in minix filesystem terminology a block which is able to contain file's
    data is called "zone"

    minix3 filesystem zone bitmap:
        bit set to 0    --> corresponding zone is free
        bit set to 1    --> corresponding zone is being used
 */

static bool         _locked;

static blk_num_t    _starting_blk_num;
// total number of zones on filesystem
static blk_num_t    _fs_num_zones;

// number of disk blocks containing the block bitmap
static unsigned     _zmap_num_blks;

void minix3_zmap_init(blk_num_t starting_blk_num, blk_num_t fs_num_zones)
{
    _fs_num_zones = fs_num_zones;
    _zmap_num_blks = _fs_num_zones / BLOCK_SIZE;
    _starting_blk_num = starting_blk_num;
}

void minix3_zmap_lock()
{
    // TODO
    while (_locked)
        ;
    _locked = true;
}

void minix3_zmap_unlock()
{
    _locked = false;
}

// locates the corresponding block bit in the bitmap
static
int _zone2blk(blk_num_t zone_num, blk_num_t *blk_num, unsigned *bit_loff)
{
    blk_num_t blk_off;

    if ((blk_off = zone_num / BITS_PER_BLOCK) >= _fs_num_zones)
        return 1;

    *blk_num  = _starting_blk_num + blk_off;
    *bit_loff = zone_num % BITS_PER_BLOCK;

    return 0;
}

static int _set_zbit_value(blk_num_t zone_num, unsigned bit_value)
{
    blk_num_t blk_num;
    unsigned bit_loff;

    if (_zone2blk(zone_num, &blk_num, &bit_loff))
        return 1;
   
    if (_blkbit_set_value(blk_num, bit_loff, bit_value))
        return 1;

    return 0;
}

static int _get_zbit_value(blk_num_t zone_num, unsigned *bit_value)
{
    blk_num_t blk_num;
    unsigned bit_loff;
    bool bit_val;

    if (_zone2blk(zone_num, &blk_num, &bit_loff))
        return 1;

    if (_blkbit_get_value(blk_num, bit_loff, &bit_val))
        return 1;

    *bit_value = bit_val;

    return 0;
}

static bool _is_zone_used(blk_num_t zone_num)
{
    unsigned bit_value;

    _get_zbit_value(zone_num, &bit_value);
    return bit_value;
}

int minix3_zmap_setz(blk_num_t zone_num)
{
    if (_set_zbit_value(zone_num, 1))
        return 1;

    return 0;
}

int minix3_zmap_clrz(blk_num_t zone_num)
{
    if (_set_zbit_value(zone_num, 0))
        return 1;

    return 0;
}

blk_num_t minix3_zmap_get_free()
{
    // TODO make efficient by keeping track of the last allocated zone
    for (blk_num_t i = 0; i < _fs_num_zones; i++)
        if (!_is_zone_used(i))
            return i;                 
 
    return 0; // no free zones
}

blk_num_t minix3_zmap_get_free_count()
{
    blk_num_t count = 0;

    for (blk_num_t i = 0; i < _fs_num_zones; i++)
        if (!_is_zone_used(i))
            count++;

    return count;
}
