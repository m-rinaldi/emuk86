#include <minix3_superblock.h>

#include <blkpool.h>

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>


static bool _initialized;
static minix3_superblock_t _;

#define SUPERBLOCK_BLK_NUM      1U
#define SUPERBLOCK_MAGIC_V3     0x4d5aU

static int _load()
{
    bufblk_t *bufblk;

    if (!(bufblk = blkpool_getblk(SUPERBLOCK_BLK_NUM))) {
        // TODO error message
        return 1;
    }

    memcpy(&_, &bufblk->block, sizeof(_));

    // do not keep the buffer block corresponding to the superblock locked
    blkpool_putblk(bufblk);

    return 0;
}

static int _init()
{
    if (_load())
        return 1;

    return 0;
}

/*******************************************************************************
 * low-level function for reading the superblock
 ******************************************************************************/
minix3_superblock_t *minix3_superblock_read()
{
    if (!_initialized) {
        if (_init())
            return NULL;

        _initialized = true;
    }

    return &_;    
}

void minix3_superblock_display()
{
    printf("s_ninodes: %d\n", _.s_ninodes);
    printf("s_imap_blocks: %d\n", _.s_imap_blocks);
    printf("s_zmap_blocks: %d\n", _.s_zmap_blocks);
    printf("s_firstdatazone: %d\n", _.s_firstdatazone);
    printf("s_log_zone_size: %d\n", _.s_log_zone_size);
    printf("\tdata zone size: %d bytes\n", 1024 << _.s_log_zone_size);
    printf("s_max_size: %d bytes\n", _.s_max_size);
    printf("s_zones: %d\n", _.s_zones);
    printf("s_magic: %x\n", _.s_magic);
    printf("s_blocksize: %d bytes\n", _.s_blocksize);
    printf("s_disk_version: %d\n", _.s_disk_version);
}
