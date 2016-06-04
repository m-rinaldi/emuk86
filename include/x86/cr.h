#pragma once
/*******************************************************************************
 * x86 control register definitions
 ******************************************************************************/

typedef struct {
    unsigned int pe         :   1;  // protected mode enable
    unsigned int mp         :   1;  // monitor co-processor
    unsigned int em         :   1;  // emulation
    unsigned int ts         :   1;  // task switched
    unsigned int et         :   1;  // extension type
    unsigned int ne         :   1;  // numeric error
    unsigned int            :  10;  // reserved
    unsigned int wp         :   1;  // write protect (for implementing CoW)
    unsigned int            :   1;  // reserved
    unsigned int am         :   1;  // alignment mask
    unsigned int            :  10;  // reserved
    unsigned int nw         :   1;  // not-write through
    unsigned int cd         :   1;  // cache disable
    unsigned int pg         :   1;  // paging
} __attribute__((packed)) cr0_t;

// CR1 is reserved

typedef uint32_t cr2_t;

typedef struct {
    unsigned int            :   12;
    unsigned int    pdbr    :   20; // page directroy base register 
} __attribute__((packed)) cr3_t;

// TODO CR4: it contains several flags controlling advanced processor's features

/*******************************************************************************
 functions for CRx manipulation
*******************************************************************************/
cr0_t cr0_get(void);
cr2_t cr2_get(void);
cr3_t cr3_get(void);

void cr0_set(cr0_t);
void cr2_set(cr2_t);
void cr3_set(cr3_t);
