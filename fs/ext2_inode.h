#pragma once

#include <stdint.h>

/*
 The following data structure for the ext2's inode was created with the
 help of the great work done by Dave Poirier on the ext2-doc project.
 A copy his book, "The Second Extended File System", which I employed, is
 available at http://www.nongnu.org/ext2-doc/ext2.html
 */

// mode - file format
#define EXT2_S_IFSOCK       0xc000  // socket
#define EXT2_S_IFLNK        0xa000  // symbolic link
#define EXT2_S_IFREG        0x8000  // regular file
#define EXT2_S_IFBLK        0x6000  // block device
#define EXT2_S_IFDIR        0x4000  // directory
#define EXT2_S_IFCHR        0x2000  // character device
#define EXT2_S_IFIFO        0x1000  // fifo
// mode - process execution user/group override
#define EXT2_S_ISUID        0x0800  // set process uid
#define EXT2_S_ISGID        0x0400  // set process gid
#define EXT2_S_ISVTX        0x0200  // sticky bit
// mode - access rights
#define EXT2_S_IRUSR        0x0100  // user read
#define EXT2_S_IWUSR        0x0080  // user write
#define EXT2_S_IXUSR        0x0040  // user execute
#define EXT2_S_IRGRP        0x0020  // group read
#define EXT2_S_IWGRP        0x0010  // group write
#define EXT2_S_IXGRP        0x0008  // group execute
#define EXT2_S_IROTH        0x0004  // others read
#define EXT2_S_IWOTH        0x0002  // others write
#define EXT2_S_IXOTH        0x0001  // others execute

// flags
// there are flags for compression and journalin that are not defined here
#define EXT2_SECRM_FL       0x00000001  // secure deletion
#define EXT2_UNRM_FL        0x00000002  // record for undelete
#define EXT2_COMPR_FL       0x00000004  // compressed file
#define EXT2_SYNC_FL        0x00000008  // synchronous updates
#define EXT2_IMMUTABLE_FL   0x00000010  // immutable file
#define EXT2_APPEND_FL      0x00000020  // append only
#define EXT2_NODUMP_FL      0x00000040  // do not dump/delete file
#define EXT2_NOATIME_FL     0x00000080  // do not update atime
#define EXT2_BTREE_FL       0x00001000  // b-tree format directory
#define EXT2_INDEX_FL       0x00001000  // hash indexed directory
#define EXT2_IMAGIC_FL      0x00002000  // AFS directory
#define EXT2_RESERVED_FL    0x80000000  // reserved for ext2 library


#define EXT2_NUM_DIR_BLKS   12  // number of direct blocks pointers
#define EXT2_SIND_BLK_IDX   EXT2_NUM_DIR_BLKS     // (single) indirect block idx
#define EXT2_DIND_BLK_IDX   (EXT2_SIND_BLK_IDX+1) // doubly-indirect block idx
#define EXT2_TIND_BLK_IDX   (EXT2_DIND_BLK_IDX+1) // triply-indirect block idx 

typedef struct {
    uint8_t     frag;
    uint8_t     fsize;
    uint16_t    reserved0;
    uint16_t    uid_high;
    uint16_t    gid_high;
    uint32_t    reserved1;
} __attribute__((packed)) ext2_osd2_t;

typedef struct {
    uint16_t    mode;
    uint16_t    uid;

    // in revision 0 this is the size of the file in bytes
    // in revision 1 or higher, this field represents (only for regualr files)
    // the lower 32-bit of the file size
    // the upper 32-bit is located in the dir_acl field
    uint32_t    size; 
    uint32_t    atime; 
    uint32_t    ctime; 
    uint32_t    mtime; 
    uint32_t    dtime; 
    uint16_t    gid;
    uint16_t    links_count;

    // number of 512-bytes blocks reserved to contain the data of this inode,
    // regardless of whether these blocks are used or not
    // this index represent 512-bytes blocks and not file system blocks,
    // so it can't be directly used as an index to the inode's block array
    uint32_t    blocks;
    uint32_t    flags;
    uint32_t    osd1;
    uint32_t    block[15];
    uint32_t    generation;
    uint32_t    file_acl;
    uint32_t    dir_acl;
    uint32_t    faddr;
    ext2_osd2_t osd2;
} __attribute__((packed)) ext2_inode_t;


void ext2_inode_display(const ext2_inode_t *);
