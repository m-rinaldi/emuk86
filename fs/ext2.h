#pragma once

#include <stdint.h>

#include <ext2_common.h>
#include <ext2_inode.h>
#include <ext2_bgdt.h>

int ext2_init();
const char *ext2_strerr();
int ext2_readi(ino_num_t, ext2_inode_t *);
int ext2_writei(ino_num_t, const ext2_inode_t *);
int ext2_read_bgd(uint32_t, ext2_bgd_t *);
