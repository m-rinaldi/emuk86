#include <minix3_inode.h>

#include <minix3_inode.h>

#include <stdio.h>

bool minix3_inode_is_dir(const minix3_inode_t *ino)
{
    return ino->i_mode & MINIX3_S_IFDIR;
}

static const char *_file_format_str(uint16_t mode)
{
    switch (mode & 0xf000) {
    case MINIX3_S_IFREG:
        return "regular file";

    case MINIX3_S_IFDIR:
        return "directory";

    default:
        return "unknown";
    }
}

void minix3_inode_display(const minix3_inode_t *ino)
{
    printf("i_mode: %x\n", ino->i_mode);
    printf("\t%s\n", _file_format_str(ino->i_mode));
    printf("i_nlinks: %d\n", ino->i_nlinks);
    printf("i_size: %d\n", ino->i_size); 
}
