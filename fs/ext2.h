#pragma once

#include <stdint.h>

#include <ext2_common.h>
#include <ext2_inode.h>
#include <ext2_bgdt.h>
#include <block.h>
#include <inode.h>

int ext2_init();
const char *ext2_strerr();
inode_t *ext2_namei(const char *filepath);
int ext2_readi(ino_num_t, ext2_inode_t *);
int ext2_writei(ino_num_t, const ext2_inode_t *);
int ext2_read_bgd(uint32_t, ext2_bgd_t *);
int ext2_bmap(const inode_t *, uint32_t, blk_num_t *blk_num, uint32_t *blk_off);
