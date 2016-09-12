#pragma once

#include <fs/blkpool.h>
#include <stdint.h>

static
int _blkbit_set_value(blk_num_t starting_blk, unsigned bit_num, bool bit_val)
{
    bufblk_t *bufblk;
    uint8_t byte;
    unsigned bit_pos;
    
    if (!(bufblk = blkpool_getblk(starting_blk)))
        return 1;

    // read the byte containing the requested bit
    byte = ((uint8_t *) &bufblk->block)[bit_num / 8];
    bit_pos = bit_num % 8;

    // set/clear requested bit in byte
    if (bit_val)
        byte |= 1U << bit_pos;
    else
        byte &= ~((uint8_t) 1U << bit_pos);

    // write back the correspoding byte
    ((uint8_t *) &bufblk->block)[bit_num / 8] = byte;

    blkpool_putblk(bufblk);

    return 0;
}

static
int _blkbit_get_value(blk_num_t starting_blk, unsigned bit_num, bool *bit_val)
{
    bufblk_t *bufblk;
    uint8_t byte;
    unsigned bit_pos;
    
    if (!(bufblk = blkpool_getblk(starting_blk)))
        return 1;

    // read the byte containing the requested bit
    byte = ((uint8_t *) &bufblk->block)[bit_num / 8];
    bit_pos = bit_num % 8;

    // get requested bit in byte
    *bit_val = byte &= 1U << bit_pos;

    blkpool_putblk(bufblk);

    return 0;
}
