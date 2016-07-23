#pragma once

#include <stdint.h>

#define MINIX3_FILENAME_LEN_MAX 60U

typedef struct {
    uint32_t    inode;
    char        name[MINIX3_FILENAME_LEN_MAX];
} __attribute__((packed)) minix3_dir_entry_t;
