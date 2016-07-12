#pragma once

#include <stdbool.h>

#include <ext2_bgdt.h>

typedef struct {
    bool        valid;
    bool        locked;
    uint32_t    num;
    
    ext2_bgd_t  bgd; 
} bufbgd_t;

void bgdpool_init();
bufbgd_t *bgdpool_getbgd(uint32_t grp_num);
void bgdpool_putbgd(bufbgd_t *);
