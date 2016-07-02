#include <ext2_super.h>

#include <blkpool.h>

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/*
    what to do with the buffer cache?
    should this module keep the superblock always "locked" and only let him
    modify the superblock?

    I would like to keep it uniformly and access the superblock through the
    buffer cache
 */

static bool _initialized;
static ext2_super_t _;
static unsigned _num_groups;
static const char *_strerr = "unknown";

#define STR_BUF_LEN     63
static char _strbuf[STR_BUF_LEN+1];

#define SUPERBLOCK_BLK_NUM  1
static int _load()
{
    bufblk_t *bufblk;

    if (!(bufblk = blkpool_getblk(SUPERBLOCK_BLK_NUM)))
        return 1;

    memcpy(&_, &bufblk->block, sizeof(_)); 

    // do not keep the buffer block corresponding to the superblock locked
    blkpool_putblk(bufblk);
    
    return 0;
}

static inline
bool _is_revision_zero()
{
    if (EXT2_GOOD_OLD_REV == _.rev_level)
        return true;

    _strerr = "revision level is not zero";
    return false;        
}

static inline
bool _is_block_size_supported()
{
    // only a block size of 1024 bytes is supported

    // block size = 1024 << log_block_size
    // block size == 1024 <--> log_block == 0
    if (_.log_block_size) {
        // block size is usually be 1k, 2k, 4k or 8k
        // however, some systems allow ever larger blocks
        // the error message is limited to 32k block size (more than enough)
        // TODO replace with snprintf
        sprintf(_strbuf, "block size is %d and not 1024 bytes",
                         0xffff & (1024 << _.log_block_size));
        _strerr = _strbuf;
        return false;
    }

    return true;
}

static inline
bool _is_inode_size_supported()
{
    // TODO check against sizeof(ext2_inode_t) instead of the magic number 128
    if (_.inode_size != 128) {
        // TODO message
        _strerr = "inode size not supported";
        return false;
    }
        

    return true;
}

static inline
bool _has_incompatible_features()
{
    if (_.feature_incompat) {
        _strerr = "file system requires some unsupported features";
        return true;
    }
    
    return false;
}

static bool _is_supported()
{
/*
    it seems that the revision field is changed to rev 1 whenever a rev 0 ext2
    file system is mounted in Linux

    if (!_is_revision_zero()) 
        return false;
*/

    if (!_is_block_size_supported())
        return false;

    if (!_is_inode_size_supported())
        return false;

    if (_has_incompatible_features()) 
        return false;


    return true;
}

static inline
bool _has_errors()
{
    if (EXT2_VALID_FS == _.state)
        return false; // file system is clean

    // TODO if error check what kind of error
    //      EXT2_ERRORS_CONTINUE
    //      EXT2_ERRORS_RO
    //      EXT2_ERRORS_PANIC

    _strerr = "file system was not cleanly unmounted";
    return true;
}

static int _calc_num_groups(unsigned *num_groups)
{
    if (!_.blocks_per_group || !_.inodes_per_group) {
        _strerr = "zero blocks per group or inodes per group";
        return 1;
    }
    
    unsigned int num_blk_grps, num_ino_grps;

    // round up integer division
    num_blk_grps = (_.blocks_count - 1) / _.blocks_per_group + 1;
    num_ino_grps = (_.inodes_count - 1) / _.inodes_per_group + 1;

    if (num_blk_grps != num_ino_grps) {
        _strerr = "inconsistent number of block groups";
        return 1;
    }

    *num_groups = num_blk_grps; 

    return 0;    
}

static int _init()
{
    if (_load())
        return 1;

    if (!_is_supported())
        return 1;

    if (_has_errors())
        return 1;

    if (_calc_num_groups(&_num_groups))
        return 1;

    return 0;
}

const char *ext2_super_strerr()
{
    return _strerr;
}

/*******************************************************************************
 * low-level function for reading the superblock
 ******************************************************************************/
ext2_super_t *ext2_read_super()
{
    if (!_initialized) {
        if (_init())
            return NULL;

        _initialized = true;
    }

    return &_;
}

/*******************************************************************************
 * low-level function for writing the superblock
 ******************************************************************************/
int ext2_write_super(ext2_super_t *super)
{
    (void) super;

    // TODO

    return 1;
}

void ext2_super_display()
{
    printf("file system: %s, ", EXT2_SUPER_MAGIC == _.magic ? "ext2" : "???" );

    {
        const char *str;

        switch (_.rev_level) {
        case EXT2_GOOD_OLD_REV:
            str = "0";
            break;

        case EXT2_DYNAMIC_REV:
            str = "1";
            break;

        default:
            str = "unkown";
        }

        printf("revision %s\n\n", str);
    }
    
    printf("state: %d\n", _.state);
    
    printf("block size:   %8d\n", 1024 << _.log_block_size);

    printf("\n");

    printf("inodes count: %8d\n", _.inodes_count);
    printf("blocks count: %8d\n", _.blocks_count);
   
    printf("\n");
 
    printf("free inodes:  %8d\n", _.free_inodes_count);
    printf("free blocks:  %8d\n", _.free_blocks_count);

    printf("\n");

    printf("blocks per group: %4d\n", _.blocks_per_group);
    printf("inodes per group: %4d\n", _.inodes_per_group);

    printf("\n");

    printf("first inode: %d\n", _.first_ino);
    printf("inode size: %4d\n", _.inode_size);
}
