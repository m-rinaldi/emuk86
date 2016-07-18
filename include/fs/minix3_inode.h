#pragma once

#include <stdint.h>

// i_mode - file format
#define MINIX3_S_IFSOCK     0xc000  // socket
#define MINIX3_S_IFLNK      0xa000  // symbolic link
#define MINIX3_S_IFREG      0x8000  // regular file
#define MINIX3_S_IFBLK      0x6000  // block device
#define MINIX3_S_IFDIR      0x4000  // directory
#define MINIX3_S_IFCHR      0x2000  // character device
#define MINIX3_S_IFIFO      0x1000  // fifo

// i_mode - process execution user/group override
#define MINIX3_S_ISUID      0x0800  // set process uid
#define MINIX3_S_ISGID      0x0400  // set process gid
#define MINIX3_S_ISVTX      0x0200  // sticky bit

// i_mode - access rights
#define MINIX3_S_IRUSR      0x0100  // user read
#define MINIX3_S_IWUSR      0x0080  // user write
#define MINIX3_S_IXUSR      0x0040  // user execute
#define MINIX3_S_IRGRP      0x0020  // group read
#define MINIX3_S_IWGRP      0x0010  // group write
#define MINIX3_S_IXGRP      0x0008  // group execute
#define MINIX3_S_IROTH      0x0004  // others read
#define MINIX3_S_IWOTH      0x0002  // others write
#define MINIX3_S_IXOTH      0x0001  // others execute

/*
    the minix fs v3 maps data blocks by means of:
        - 7 direct blocks
        - 1 single indirect block
        - 1 double indirect block
        - 1 triple indirect block
 */
#define MINIX3_NUM_DIR_BLKS         7U
#define MINIX3_NUM_SIND_BLK_IDX     MINIX3_NUM_DIR_BLKS
#define MINIX3_NUM_DIND_BLK_IDX     (MINIX3_SIND_BLK_IDX+1)
#define MINIX3_NUM_TIND_BLK_IDX     (MINIX3_DIND_BLK_IDX+1)

typedef struct {
    uint16_t    i_mode;
    uint16_t    i_nlinks;   // # of links to this file
    uint16_t    i_uid;      // user id of the file's owner
    uint16_t    i_gid;      // group number
    uint32_t    i_size;     // current file size in bytes
    uint32_t    i_atime;    // time of last access
    uint32_t    i_mtime;    // time of last file's data change
    uint32_t    i_ctime;    // time of last inode change
    uint32_t    i_zone[10]; // zone numbers
} __attribute__((packed)) minix3_inode_t;

void minix3_inode_display(const minix3_inode_t *);
