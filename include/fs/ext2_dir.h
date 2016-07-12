#pragma once

#include <stdint.h>

#include <ext2.h>

/*
 The following data structure for the ext2 directory entry was created with the
 help of the great work done by Dave Poirier on the ext2-doc project.
 A copy his book, "The Second Extended File System", which I employed, is
 available at http://www.nongnu.org/ext2-doc/ext2.html
 */

#define EXT2_FT_UNKNOWN     0 // unknown file type
#define EXT2_FT_REG_FILE    1 // regular file
#define EXT2_FT_DIR         2 // directory file
#define EXT2_FT_CHRDEV      3 // character device
#define EXT2_FT_BLKDEV      4 // block device
#define EXT2_FT_FIFO        5 // buffer file
#define EXT2_FT_SOCK        6 // socket file
#define EXT2_FT_SYMLINK     7 // symbolic link

typedef struct {
    ino_num_t   inode;  // a value of zero indicate that the entry is not used

    // displacement to the next directory entry from the start of the current
    // directory entry
    // directory entries must be 4-byte aligned
    // a directory entry cannot span over multiple blocks
    // if a directory entry cannot completely fit in one block, it must be
    // pushed to the next data block and the rec_len of the previous entry
    // must be properly adjusted
    uint16_t    rec_len;

    // this value must never be larger than rec_len - 8
    // i.e.: name_len + 8 <= rec_len
    uint8_t     name_len;
    uint8_t     file_type;
    uint8_t     name; 
} __attribute__((packed)) ext2_dir_entry_t;
