#pragma once

#include <inode.h>

#include <stdbool.h>

void ipool_init();
inode_t *ipool_geti(ino_num_t);
int ipool_puti(inode_t *);
