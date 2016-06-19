#pragma once

#include <ext2.h>
#include <inode.h>

#include <stdbool.h>

void ipool_init();
inode_t *ipool_geti(ino_num_t);
void ipool_puti(inode_t *);
