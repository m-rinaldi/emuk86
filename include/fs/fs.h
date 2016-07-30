#pragma once

#include <stdint.h>

typedef uint32_t ino_num_t;
typedef uint32_t blk_num_t;

int fs_init();
int fs_open(const char *);
int fs_close(int);
