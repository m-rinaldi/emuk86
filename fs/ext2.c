#include <ext2.h>

#include <ext2_super.h>
#include <ext2_inode.h>
#include <ext2_bgdt.h>
#include <blkpool.h>
#include <bgdpool.h>
#include <inode.h>

#define ROOT_INODE  2

static const char *_strerr = "unknown";

static struct {
    const ext2_super_t *super;
} _;

int ext2_init()
{
    if (ext2_super_init()) {
        _strerr = ext2_strerr();
        return 1;
    }

    _.super = ext2_super_get();

    return 0;
}

const char *ext2_strerr()
{
    return _strerr;
}

static inline
unsigned _ino2grp(ino_num_t ino_num)
{
    return (ino_num - 1) / (_.super->inodes_per_group);
}

static inline
unsigned _ino2lidx(ino_num_t ino_num)
{
    return (ino_num - 1) % (_.super->inodes_per_group);
}

/*******************************************************************************
 * low-level function for reading an inode from disk
 ******************************************************************************/
int ext2_readi(ino_num_t ino_num, ext2_inode_t *ino)
{
    #define INODES_PER_BLOCK    (sizeof(block_t)/sizeof(ext2_inode_t))
    bufbgd_t *bufbgd;

    // read the corresponding block group descriptor
    // determine which block group contains the requested inode
    unsigned grp_num = _ino2grp(ino_num);

    if (!(bufbgd = bgdpool_getbgd(grp_num)))
        goto err;
   
    // XXX 
    ext2_bgd_display(&bufbgd->bgd);

    // determine the local index inside the table
    unsigned lidx = _ino2lidx(ino_num);

    // determine the block containing the inode
    uint32_t iblk = bufbgd->bgd.inode_table + lidx / INODES_PER_BLOCK;

    // pick up the block
    {
        bufblk_t *bufblk;

        if (!(bufblk = blkpool_getblk(iblk)))
            goto rel_bufbgd;
   
        // copy the requested inode
        *ino = (lidx % INODES_PER_BLOCK)[(ext2_inode_t *) &bufblk->block];

        blkpool_putblk(bufblk);
    }

    bgdpool_putbgd(bufbgd);
    return 0;

rel_bufbgd:
    bgdpool_putbgd(bufbgd);

err:
    return 1;
    #undef INODES_PER_BLOCK
}

/*******************************************************************************
 * low-level function for reading a block group descriptor from disk
 ******************************************************************************/
int ext2_read_bgd(uint32_t grp_num, ext2_bgd_t *bgd)
{
    // for 1k-blocks the block group descriptor table begins at block 2
    static const blk_num_t bgdt_blk = 2;

    bufblk_t *bufblk;
    unsigned blk_num;
    unsigned lidx;

    // determine the block where the block group descriptor is
    blk_num = bgdt_blk + grp_num * sizeof(ext2_bgd_t) / sizeof(block_t);

    if (!(bufblk = blkpool_getblk(blk_num)))
        return 1;
   
    // local index of the block group descriptor inside the block
    lidx = grp_num * sizeof(ext2_bgd_t) % sizeof(block_t);
   
    // copy the block group descriptor 
    *bgd = lidx[(ext2_bgd_t *) &bufblk->block];

    blkpool_putblk(bufblk);
    
    return 0;
}

blk_num_t ext2_bmap(inode_t *ino, uint32_t boff, uint32_t *blk_loff)
{
    // TODO add ouput byte offset into block
    return 0;
}
