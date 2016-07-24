#include <stdio.h>
#include <stdlib.h>

#include <hdd.h>

#include <fs/blkpool.h>
#include <fs/ipool.h>

#include <fs/minix3.h>
#include <fs/minix3_superblock.h>
#include <fs/minix3_inode.h>

// XXX
#include <fs/superblock.h>

#include <test.h>

static int _init()
{
    if (hdd_init())
        return 1;

    blkpool_init();
    ipool_init();

    if (minix3_init())
        return 1;
    
    return 0;
}

int main()
{

/*
    printf("sizeof(minix3_inode_t): %d\n", sizeof(minix3_inode_t));

    // initialize fs subsystem
    if (_init()) {
        fprintf(stderr, "_init() failed\n");
        return 1;
    }

    if (!minix3_superblock_read()) {
        fprintf(stderr, "minix3_superblock_read()\n");
        return 1;
    }

    minix3_superblock_display();
*/

/*
    minix3_inode_t ino;

    if (minix3_readi(4, &ino)) {
        fprintf(stderr, "minix3_readi() error\n");
        exit(1);
    }
    
    printf("\n");
    minix3_inode_display(&ino);
*/

#if 0
    inode_t *ino;
    
    if (!(ino = ipool_geti(4))) {
        fprintf(stderr, "ipool_geti() error\n");
        exit(1);
    }

    printf("\n");
    minix3_inode_display(&ino->dinode);

    {
        blk_num_t blk_num;
        unsigned blk_loff;

        if (minix3_bmap(&ino->dinode, 0, &blk_num, &blk_loff)) {
            fprintf(stderr, "minix3_bmap() error\n");
            exit(1);
        }

        printf("blk #%x\n", blk_num);
        printf("blk_loff: %d\n", blk_loff);

        char str[256];

        // fill it up with data
        {
            bufblk_t *bufblk;

            if (!(bufblk = blkpool_getblk(blk_num)))
                exit(1);
            
            strncpy(str, (char *) &bufblk->block, ino->dinode.i_size);
            str[ino->dinode.i_size] = '\0';

            blkpool_putblk(bufblk); 
        }

        printf("\ndata: <%s>\n\n", str);
    }
#endif

    if (_init()) {
        fprintf(stderr, "_init() error\n");
        exit(1);
    }

    inode_t *ino;
    if (!(ino = minix3_namei(TEST_PATH))) {
        fprintf(stderr, "minix3_namei(): file not found\n");
        exit(1);
    }

    printf("ino #%d\n", ino->num);

    return 0;
}
