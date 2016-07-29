#include <stdio.h>
#include <stdlib.h>

// for the initialization
#include <hdd.h>
#include <fs/blkpool.h>
#include <fs/ipool.h>

#include <fs/minix3.h>
#include <proc/ptable.h>

static int _init()
{
    if (fs_init())
        return 1;

    if (ptable_init())
        return 1;
    
    return 0;
}

int main()
{

    if (_init()) {
        fprintf(stderr, "_init() error\n");
        exit(1);
    }

    int fd;

    for (int i = 0; i < 9; i++) {
        if (-1 == (fd = fs_open("foo/foo.txt"))) {
            fprintf(stderr, "fs_open() error\n");
            exit(1);
        }

        fprintf(stderr, "fd: %d\n", fd);
    }
    
    return 0;
}
