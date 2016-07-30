#include <fs/fd_table.h>

#include <fs/file_table.h>
#include <proc/ptable.h>
#include <string.h>

static inline void _init_entry(fd_table_entry_t *fdte)
{
    bzero(fdte, sizeof(*fdte));    
}

void fd_table_init(fd_table_t *fdt)
{
    for (unsigned i = 0; i < FD_TABLE_NUM_ENTRIES; i++)
        _init_entry(fdt->_ + i);
}

static inline
fd_table_t *_get_fdt_cur_proc()
{
    return &cur_proc->fdt;
}

int fd_table_alloc_entry(fd_table_t *fdt, const file_table_entry_t *fte)
{
    if (!fdt)
        fdt = _get_fdt_cur_proc();

    for (unsigned i = 0; i < FD_TABLE_NUM_ENTRIES; i++)
        if (!fdt->_[i].active) {
            fdt->_[i].active = true;
            fdt->_[i].fte = (file_table_entry_t *) fte;

            // TODO set flags
            fdt->_[i].flags = 0;

            return i;
        }

    return -1; // no free entries
}

int fd_table_dup_entry(fd_table_t *fdt, int fd)
{
    if (!fdt)
        fdt = _get_fdt_cur_proc();

    if (!fdt->_[fd].active)
        return 1;

    int dup_fd;
    file_table_entry_t *fte = file_table_dup_entry(fdt->_[fd].fte);

    if (-1 == (dup_fd = fd_table_alloc_entry(fdt, fte))) {
        file_table_release_entry(fdt->_[fd].fte);
        return 1;
    }

    return dup_fd;
}

int fd_table_dealloc_entry(fd_table_t *fdt, int fd)
{
    if (!fdt)
        fdt = _get_fdt_cur_proc();

    if ((unsigned)fd >= FD_TABLE_NUM_ENTRIES)
        return 1;

    file_table_release_entry(fdt->_[fd].fte);
    fdt->_[fd].active = false;

    return 0;
}
