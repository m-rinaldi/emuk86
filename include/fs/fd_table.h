#pragma once

#include <fs/fd_table_entry.h>

#define FD_TABLE_NUM_ENTRIES    8U

typedef struct {
    fd_table_entry_t    _[FD_TABLE_NUM_ENTRIES];
} fd_table_t;

void fd_table_init(fd_table_t *);
int fd_table_alloc_entry(fd_table_t *, const file_table_entry_t *);
int fd_table_dup_entry(fd_table_t *, int fd, int *dup_fd);
int fd_table_dealloc_entry(fd_table_t *, int fd);
