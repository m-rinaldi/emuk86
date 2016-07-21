#pragma once

#include <fs.h>
#include <minix3_inode.h>

#define MINIX3_ROOT_INODE_NUM   1U

int minix3_init();
int minix3_readi(ino_num_t, minix3_inode_t *);
int minix3_writei(ino_num_t, const minix3_inode_t *);