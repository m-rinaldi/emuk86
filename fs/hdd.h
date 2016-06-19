#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <block.h>

int hdd_init();
int hdd_readblk(uint32_t blk_num, block_t *blk);
block_t *hdd_getblk(uint32_t blk_num);


