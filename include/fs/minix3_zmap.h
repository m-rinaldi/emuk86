#pragma once

#include <fs/block.h>

void minix3_zmap_init(blk_num_t starting_blk_num, blk_num_t fs_num_zones);
int minix3_zmap_clrz(blk_num_t);
int minix3_zmap_setz(blk_num_t);
blk_num_t minix3_zmap_get_free();
blk_num_t minix3_zmap_get_free_count();
