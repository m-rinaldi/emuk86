#pragma once

#include <list.h>
#include <fs/inode.h>

#include <stdbool.h>

#define FTE_APPEND  0x1
#define FTE_WRITE   0x2

typedef struct {
    unsigned    count;
    uint8_t     mode; // append, read-only, write

    // where the next read() or write() it is expected to begin
    uint32_t offset;

    // pointer to a inode on the inode table
    inode_t *inode;

    // node on a free list
    list_node_t free_node;
} file_table_entry_t;

void file_table_init();
file_table_entry_t *file_table_alloc_entry(uint8_t mode, const inode_t *);
file_table_entry_t *file_table_dup_entry(file_table_entry_t *);
void file_table_release_entry(file_table_entry_t *fte);
