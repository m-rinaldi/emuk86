#include <ext2_bgdt.h>

#include <blkpool.h>

#include <stdio.h>

void ext2_bgd_display(const ext2_bgd_t *bgd)
{
    printf("unallocated blocks in group: %d\n", bgd->free_blocks_count);
    printf("unallocated inodes in group: %d\n", bgd->free_inodes_count);
    printf("used dirs count: %d\n", bgd->used_dirs_count);
    printf("inode table blk: %d\n", bgd->inode_table);
}
