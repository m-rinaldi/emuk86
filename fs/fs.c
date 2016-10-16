#include <fs/fs.h>

#include <hdd.h>
#include <fs/minix3.h>
#include <fs/minix3_dir_entry.h>
#include <fs/blkpool.h>
#include <fs/ipool.h>
#include <fs/fd_table.h>
#include <fs/file_table.h>
#include <proc/ptable.h>

int fs_init()
{
    if (hdd_init())
        return 1;

    blkpool_init();
    ipool_init();

    if (minix3_init())
        return 1;    

    file_table_init();

    return 0;
}

static int _create_dir_entry(inode_t *ino, minix3_dir_entry_t *de)
{
    // TODO
    return 0;
}

int fs_creat(const char *filepath, mode_t mode)
{
    inode_t *ino, *pdir_ino = NULL;

    // TODO no difference between inexistent and lack of permissions
    //      but to date permission are not being check
    if ((ino = minix3_namei(filepath, &pdir_ino))) {
        // file already exists
        // TODO check permission access
        // TODO truncate file
    } else if (pdir_ino) { // no file exists yet
        if (!(ino = minix3_alloci()))
            goto err_rel_pdir_ino;

        // create directory entry
        minix3_dir_entry_t de;
        if (_create_dir_entry(pdir_ino, &de))
            goto err_rel_ino;

        ipool_puti(pdir_ino); pdir_ino = NULL;
    } else // minix3_namei() failed
        goto err;

    // allocate a file table entry for this file
    file_table_entry_t *fte;
    if (!(fte = file_table_alloc_entry(mode, ino)))
        goto err_rm_de;

    // allocate a file descriptor for this file in current process
    int fd;
    if (-1 == (fd = fd_table_alloc_entry(NULL, fte)))
        goto err_rel_fte;

    inode_unlock(ino);
    return fd;

err_rel_fte:
    file_table_release_entry(fte);

err_rm_de:
    // TODO remove directory entry

err_rel_ino:
    ipool_puti(ino);

err_rel_pdir_ino:
    if (pdir_ino)
        ipool_puti(pdir_ino);

err:
    return -1;

}

// TODO add flags
int fs_open(const char *filepath)
{
    inode_t *ino;

    if (!(ino = minix3_namei(filepath, NULL)))
        return -1;

    // TODO make inode_unlock() "blockable"
    inode_unlock(ino);

    // allocate file table entry
    file_table_entry_t *fte;

    if (!(fte = file_table_alloc_entry(0, ino)))
        goto err_rel_ino;

    // allocate file descriptor table entry
    {
        int fd;

        if (-1 == (fd = fd_table_alloc_entry(NULL, fte)))
            goto err_rel_fte;

        return fd;
    }

err_rel_fte:
    file_table_release_entry(fte);

err_rel_ino:
    ipool_puti(ino);
    
    return -1;
}

int fs_close(int fd)
{
    if (fd_table_dealloc_entry(NULL, fd))
        return 1;

    return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
    file_table_entry_t *fte;

    if (!(fte = fd_table_get_fte(NULL, fd)))
        return -1;

    // TODO get inode from the file table entry and lock it

    size_t bread = 0;
    size_t bleft = count;
    while (bleft > 0) {

        // already at the end of the file
        if (!(fte->offset < fte->inode->dinode.i_size))
            break;

        // determine block number and bytes to be read
        uint32_t blk_num;
        unsigned loff;
        size_t subcount;
        {
            if (minix3_bmap(&fte->inode->dinode, fte->offset, &blk_num, &loff))
                return -1;


            // number readable bytes left in block
            unsigned blk_bleft;
            {
                uint32_t lblk_max = fte->inode->dinode.i_size / sizeof(block_t);
                bool is_last_blk = lblk_max == fte->offset / sizeof(block_t);

                if (is_last_blk) {
                    blk_bleft = fte->inode->dinode.i_size % sizeof(block_t);
                    blk_bleft -= loff;
                } else
                    blk_bleft = sizeof(block_t) - loff;
            }

            // bytes to be read in block
            subcount = blk_bleft > bleft ? bleft : blk_bleft;
        }

        // perform the copy of the data
        {
            bufblk_t *bufblk;

            // support for file holes (blk_num == 0)
            // boot block will be never requested throughout this interface
            if (!blk_num) {
                if (!(bufblk = blkpool_get_any()))
                    return -1;

                bufblk->valid = false;
                bufblk->num = -1U;
                bzero(&bufblk->block, sizeof(bufblk->block));
            }
            else if (!(bufblk = blkpool_getblk(blk_num)))
                    return -1;

            memcpy((uint8_t *)buf + bread,
                   (uint8_t *)&bufblk->block + loff, subcount);

            blkpool_putblk(bufblk);
        }

        // update offset in the file table
        fte->offset += subcount;

        // update counters
        bleft -= subcount;
        bread = count - bleft;
    }

    // TODO unlock inode
    // TODO handle returning from an error by unlocking the inode

    return bread;
}

int fs_write(int fd, void *buf, size_t count)
{
    file_table_entry_t *fte;

    if (!(fte = fd_table_get_fte(NULL, fd)))
        return -1;

    // the inode should be kept locked for the whole fs_write() because
    // it may change the inode when allocating new blocks
    inode_t *ino;

    ino = fte->inode;
    inode_lock(ino);

    size_t bwrite = 0;  // bytes written
    size_t bleft = count;
    while (bleft > 0) {

        // determine block number and bytes to be written
        uint32_t blk_num;
        unsigned loff;
        size_t subcount;
        {
            if (minix3_bmap(&fte->inode->dinode, fte->offset, &blk_num, &loff))
                return -1;

            // number writable bytes left in current block
            unsigned blk_bleft = sizeof(block_t) - loff;

            // buffer data bytes to be written in block
            subcount = blk_bleft > bleft ? bleft : blk_bleft;
        }

        // perform the copy of the data
        {
            bufblk_t *bufblk;

            // TODO implement the different levels of indirection
            if (!(bufblk = blkpool_getblk(blk_num)))
                goto err;

            memcpy((uint8_t *)&bufblk->block + loff,
                   (uint8_t *)buf + bwrite, subcount);

            blkpool_putblk(bufblk);
        }

        // update offset in the file table
        fte->offset += subcount;

        // update counters
        bleft -= subcount;
        bwrite = count - bleft;
    }

    inode_unlock(ino);
    return bwrite;

err:
    inode_unlock(ino);
    return -1;
}
