#include <proc/ptable.h>

#include <fs/minix3.h>
#include <fs/ipool.h>

process_t *cur_proc;

#define NUM_ENTRIES     8U
static process_t    _[NUM_ENTRIES];

// TODO static list_t   _free;
// TODO static list_t   _active;

int ptable_init()
{
    cur_proc = _;

    if (!(cur_proc->icrd = ipool_geti(MINIX3_ROOT_INODE_NUM)))
        return 1;

    inode_unlock(cur_proc->icrd);
    cur_proc->icwd = cur_proc->icrd;

    fd_table_init(&cur_proc->fdt);

    return 0;
}

/* TODO in order to assign a PID to a newly created process:
        1) get a free entry
        2) get the offset of the entry in the process table (a O(1) operation)
        3) set the PID to the obtained offset 
*/
