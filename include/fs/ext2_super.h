#pragma once

#include <stdint.h>

/*
 The following data structure for the ext2's superblock was created with the
 help of the great work done by Dave Poirier on the ext2-doc project.
 A copy his book, "The Second Extended File System", which I employed, is
 available at http://www.nongnu.org/ext2-doc/ext2.html
 */

// magic
#define EXT2_SUPER_MAGIC        0xef53

// state
#define EXT2_VALID_FS           1
#define EXT2_ERROR_FS           2

// errors
#define EXT2_ERRORS_CONTINUE    1   // continue as if nothing happend
#define EXT2_ERRORS_RO          2   // remount read-only
#define EXT2_ERRORS_PANIC       3   // cause a kernel panic

// creator os
#define EXT2_OS_LINUX           0
#define EXT2_OS_HURD            1
#define EXT2_OS_MASIX           2
#define EXT2_OS_FREEBSD         3
#define EXT2_OS_LITES           4

#define EXT2_GOOD_OLD_REV       0
#define EXT2_DYNAMIC_REV        1

// feature_incompat
#define EXT2_FEATURE_INCOMPAT_COMPRESSION   0x0001  // compression is used
// directory entries contain a type field
#define EXT2_FEATURE_INCOMPAT_FILETYPE      0x0002
// file system needs to replay its journal
#define EXT2_FEATURE_INCOMPAT_RECOVER       0x0004
// file system uses a journal device

// rev_level
#define EXT2_FEATURE_INCOMPAT_JOURNAL_DEV   0x0008
#define EXT2_FEATURE_INCOMPAT_META_BG       0x0010


typedef struct {
    uint32_t    inodes_count;
    uint32_t    blocks_count; 
    uint32_t    r_blocks_count; // blocks reserved for the superuser
    uint32_t    free_blocks_count;
    uint32_t    free_inodes_count;
    uint32_t    first_data_block;
    uint32_t    log_block_size; // block size = 1024 << log_block_size

    // log_frag_size >  0       --> fragment size = 1024 << log_frag_size
    // log_frag_size <= 0       --> fragment size = 1024 >> -log_frag_size
    uint32_t    log_frag_size;
    uint32_t    blocks_per_group;
    uint32_t    frags_per_group;
    uint32_t    inodes_per_group;
    uint32_t    mtime; // time of the last time the file system was mounted
    uint32_t    wtime; // time of the last write access to the file system
    uint16_t    mnt_count; // mounts since the last check
    uint16_t    max_mnt_count; // mounts allowed before a check
    uint16_t    magic; // identifier for the ext2 fs: 0xef53

    // EXT2_VALID_FS    --> unmonted cleanly
    // EXT2_ERROR_FS    --> errors detected
    uint16_t    state;

    // EXT2_ERRORS_CONTINUE --> continue as if nothing happened
    // EXT2_ERRORS_RO       --> remount as read-only file system
    // EXT2_ERRORS_PANIC    --> cause a kernel panic
    uint16_t    errors;
    uint16_t    minor_rev_level;
    uint32_t    last_check;
    uint32_t    check_interval;
    uint32_t    creator_os;

    // EXT2_GOOD_OLD_REV    --> revision 0
    // EXT2_DYNAMIC_REV     --> revision 1: variable inode sizes, xattr ...
    uint32_t    rev_level;    
    uint16_t    def_resuid; // default user id for reserved blocks
    uint16_t    def_resgid; // default group id for reserved blocks
    
    // EXT2_DYNAMIC_REV specific
    uint32_t    first_ino; // first inode usable for standard files
    uint16_t    inode_size; // in revision 0, this value is fixed to 128
    uint16_t    block_group_nr; // block group number hosting this superblock
    uint32_t    feature_compat;     // optional features present
    
    // =========================
    // required features present
    // =========================
    // the file system implementation should refuse to mount the file system
    // if any of the indicated feature is unsopported

    // an implementation not supporting these features would be unable to
    // properly use the file system 
    uint32_t    feature_incompat;

    // rest of EXT2_DYNAMIC_REV specific stuff
    uint8_t     dynamic_rev[164];

    uint8_t     unused[760];
        
} __attribute__((packed)) ext2_super_t;

ext2_super_t *ext2_read_super();
const char *ext2_super_strerr();
void ext2_super_display();
