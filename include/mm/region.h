#pragma once

#include <stdint.h>

/*
    A region is contiguous area of the space of vitual addresses.
    In principle a region would consist of:

        1)  the size of the region
        2)  the physical addresses of each page the region consists of
        3)  an array of page tables

    However, a region will correspond to one and only one entry in the page
    directory for the sake of simplicity. This results in the following
    implications:

        a)  a region is limited to a single page table (4 MiB).
        b)  the starting virtual address a process maps a region is aligned
            to 4 MiB boundaries.

    A region knows nothing about virtual addresses, only physical addresses
    it is the responsability of a process to map a region to a particular
    virtual address

*/ 

typedef struct {
    unsigned int pd_idx     :   10;
    unsigned int num_pages  :   12;

    // TODO add page table 
} region_t;
