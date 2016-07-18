#include <minix3.h>

#include <minix3_inode.h>
#include <superblock.h>
#include <blkpool.h>

#define INODES_PER_BLOCK        (sizeof(block_t)/sizeof(minix3_inode_t))

static const superblock_t *_sb;

int minix3_init()
{
    if (!(_sb = superblock_get()))
        return 1;

    return 0;
}

static inline
blk_num_t _itable_blk_num()
{
    blk_num_t blk_num;

    // 1st block is empty, 2nd block is the superblock
    blk_num = 2;

    // blocks containing the inode bitmap
    blk_num += _sb->_->s_imap_blocks;

    // blocks containing the block bitmap
    blk_num += _sb->_->s_zmap_blocks;

    return blk_num;
}

static inline
blk_num_t _ino2blk(ino_num_t ino, unsigned *lioff)
{
    // calculate the local inode offset
    *lioff = (ino - MINIX3_ROOT_INODE_NUM) % INODES_PER_BLOCK;

    // calculate the block number containing the requested inode
    return (ino - MINIX3_ROOT_INODE_NUM) / INODES_PER_BLOCK
            + _itable_blk_num();
}

/*******************************************************************************
 * low-level function for reading the inode
 ******************************************************************************/
int minix3_readi(ino_num_t ino_num, minix3_inode_t *ino)
{
    if (ino_num < MINIX3_ROOT_INODE_NUM ||
        ino_num - MINIX3_ROOT_INODE_NUM >= _sb->_->s_ninodes)
    {
        // TODO error msg: invalid inode number
        return 1;
    }

    unsigned lioff;
    blk_num_t blk_num = _ino2blk(ino_num, &lioff);

    // fetch inode
    {
        bufblk_t *bufblk;

        if (!(bufblk = blkpool_getblk(blk_num)))
            return 1;

        *ino = lioff[(minix3_inode_t *) &bufblk->block];

        blkpool_putblk(bufblk);
    }

    return 0;
}

int minix3_writei(ino_num_t ino_num, const minix3_inode_t *ino)
{
    // TODO
    (void) ino;
    return 0;
}
