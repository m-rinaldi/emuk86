#include <hdd.h>

#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define FS_IMAGE_PATH   "./img/minix3fs.img"

static int _fd = -1;

int hdd_init()
{
    if (-1 == (_fd = open(FS_IMAGE_PATH, O_RDWR))) {
        perror("open()");
        return 1;
    }

    return 0;
}

static inline
size_t blk2off(uint32_t blk_num)
{
    return blk_num * sizeof(block_t);
}

static int _seek(uint32_t blk_num)
{
    if (-1 == lseek(_fd, blk2off(blk_num), SEEK_SET))
        return 1;

    return 0; 
}

static int _readblk(uint32_t blk_num, block_t *blk)
{
    int bread;
    int bleft   = sizeof(*blk);
    int btotal  = 0;
    uint8_t *p  = (uint8_t *) blk;

    if (_seek(blk_num))
        return -1;

    do {
        if (-1 == (bread = read(_fd, p, bleft)))
            return -1;

        if (!bread)
            return btotal;

        bleft   -= bread;
        p       += bread;
        btotal  += bread;
    } while (bleft);

    return btotal;
}

static int _writeblk(uint32_t blk_num, const block_t *blk)
{
    int bwrite;
    int bleft   = sizeof(*blk);
    int btotal  = 0;
    const uint8_t *p  = (const uint8_t *) blk;

    if (_seek(blk_num))
        return -1;

    do {
        if (-1 == (bwrite = write(_fd, p, bleft)))
            return -1;

        bleft   -= bwrite;
        p       += bwrite;
        btotal  += bwrite;
    } while (bleft);

    return btotal;
}

int hdd_readblk(uint32_t blk_num, block_t *blk)
{
    if (sizeof(*blk) != _readblk(blk_num, blk))
         return 1;

    return 0;
}

int hdd_writeblk(uint32_t blk_num, const block_t *blk)
{
    if (sizeof(*blk) != _writeblk(blk_num, blk))
         return 1;

    return 0;
}
