#include <ext2_inode.h>

#include <stdio.h>

static inline
const char *_mode2str(uint16_t mode)
{
    const char *str;

    switch (0xf000 & mode) {
    case EXT2_S_IFREG:
        str = "regular file";
        break;

    case EXT2_S_IFDIR:
        str = "directory";
        break;

    // TODO complete with the rest of the values for mode
    default:
        str = "unknown";
    }

    return str;
}

void ext2_inode_display(const ext2_inode_t *ino)
{
    printf("mode:   %s\n", _mode2str(ino->mode)); 
    printf("size:   %d\n", ino->size);
    printf("blocks: %d\n", ino->blocks);
}
