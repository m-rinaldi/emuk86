#pragma once

#include <stdint.h>

/*
 The following data structure for the ext2's block group descriptor was
 created with the help of the great work done by Dave Poirier on the ext2-doc
 project.
 A copy his book, "The Second Extended File System", which I employed, is
 available at http://www.nongnu.org/ext2-doc/ext2.html
 */

// Block Group Descriptor
typedef struct {
    uint32_t    block_bitmap;
    uint32_t    inode_bitmap;
    uint32_t    inode_table;
    uint16_t    free_blocks_count;
    uint16_t    free_inodes_count;
    uint16_t    used_dirs_count;
    uint16_t    pad;
    uint8_t     reserved[12];
} __attribute__((packed)) ext2_bgd_t;


void ext2_bgd_display(const ext2_bgd_t *);
