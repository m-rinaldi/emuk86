#include <minix3.h>

#include <inode.h>
#include <superblock.h>
#include <blkpool.h>

#define INODES_PER_BLOCK        (BLOCK_SIZE/sizeof(minix3_inode_t))

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

/*******************************************************************************
 * low-level functions for determining the block number from a byte offset
 ******************************************************************************/

// determines the required level of indirection from the logical block number
static inline
unsigned _blk2indlevel(blk_num_t lblk_num)
{
    switch (lblk_num) {
        case MINIX3_SIND_BLK_IDX:
            return 1;

        case MINIX3_DIND_BLK_IDX:
            return 2;

        case MINIX3_TIND_BLK_IDX:
            return 3;
    }

    return 0; // no indirection
}

#define BLKADDRS_PER_BLOCK  (BLOCK_SIZE/sizeof(blk_num_t))

int minix3_bmap(const minix3_inode_t *ino, uint32_t byte_off,
                blk_num_t *blk_num, unsigned *blk_loff)
{
    blk_num_t lblk_num;

    // calculate logical block number
    lblk_num  = byte_off / BLOCK_SIZE;

    // calculate local (to the block) byte offset
    *blk_loff = byte_off % BLOCK_SIZE;

    unsigned ind_level = _blk2indlevel(lblk_num);

    // block to take at first
    *blk_num = ino->i_zone[lblk_num + ind_level];

    // blocks requiring a level of indirection other than zero
    for (lblk_num -= MINIX3_NUM_DIR_BLKS;
         ind_level; // requires indirection?
         lblk_num /= BLKADDRS_PER_BLOCK, ind_level--)
    {
        unsigned blkaddr_off = lblk_num & (BLKADDRS_PER_BLOCK - 1);

        // pick up a block containing block addresses/numbers
        {
            bufblk_t *bufblk;

            if (!(bufblk = blkpool_getblk(*blk_num)))
                return 1;

            // get block number of the next indirection level (if any)
            *blk_num = blkaddr_off[(blk_num_t *) &bufblk->block];

            blkpool_putblk(bufblk);
        }
    }

    return 0;
}
