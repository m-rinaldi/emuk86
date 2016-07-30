#include <fs/fs.h>

#include <hdd.h>
#include <fs/minix3.h>
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

// TODO add flags
int fs_open(const char *filepath)
{
    inode_t *ino;

    if (!(ino = minix3_namei(filepath)))
        return -1;

    inode_unlock(ino);

    // allocate file table entry
    file_table_entry_t *fte;

    if (!(fte = file_table_alloc_entry(0, ino)))
        goto err_rel_ino;

    // allocate file descriptor table entry
    {
        int fd;

        // TODO fd_table_alloc_entry(NULL, fte)
        if (-1 == (fd = fd_table_alloc_entry(&cur_proc->fdt, fte)))
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
    // TODO fd_table_dealloc_entry(NULL, fd);
    if (fd_table_dealloc_entry(&cur_proc->fdt, fd))
        return 1;

    return 0;
}
