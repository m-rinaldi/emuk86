#pragma once

#include <stdint.h>

typedef struct {
    uint32_t    s_ninodes;          // # of inodes
    uint16_t    s_pad0;
    uint16_t    s_imap_blocks;      // # of blocks used by inode bit map
    uint16_t    s_zmap_blocks;      // # of blocks used by block bit map
    uint16_t    s_firstdatazone;    // first zone which contains file data

    // the size of a data zone is calculated as: 1024 << s_log_zone_size
    uint16_t    s_log_zone_size;
    uint16_t    s_pad1;
    uint32_t    s_max_size;         // maximum file size in bytes
    uint32_t    s_zones;            // # of zones
    uint16_t    s_magic;            // minix fs v3 magic number
    uint16_t    s_pad2;
    uint16_t    s_blocksize;        // block size in bytes
    uint8_t     s_disk_version;
} __attribute__((packed)) minix3_superblock_t;
