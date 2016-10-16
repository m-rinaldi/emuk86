#pragma once

#include <stdint.h>
#include <sys/types.h>

typedef uint32_t ino_num_t;
typedef uint32_t blk_num_t;

typedef uint32_t mode_t;

int fs_init();

int fs_creat(const char *, mode_t);
int fs_open(const char *);

int fs_close(int);

int fs_read(int fd, void *buf, size_t count);
