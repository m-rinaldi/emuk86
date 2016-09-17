#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <fs/block.h>

int hdd_init();
int hdd_readblk(uint32_t blk_num, block_t *blk);
int hdd_writeblk(uint32_t blk_num, const block_t *blk);
