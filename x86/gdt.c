/*******************************************************************************
 * Global Descriptor Table
 ******************************************************************************/
#include <x86/gdt.h>

#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // ac: accesed bit, set to zero
    // the CPU sets this to 1 when the segment is accessed
    volatile unsigned int   ac      :   1;

    // rw: readable bit/writable bit

    // readable bit for code selectors
    //      write access is never allowed for code segments

    // writable bit for code selectors
    //      read access is always allowed for data segments
    unsigned int            rw      :   1;

    // dc: direction bit/conforming bit

    // direction bit for data selectors: tells the direction
    //      0 --> the segment grows up
    //      1 --> the segment grows down

    // conforming bit for code selectors
    //      0 --> code in this segment can only be executed from the ring set
    //            in <privl>
    //      1 --> code in this segment can be executed from an equal or lower
    //            privilege level. The <privl> represents the highest privilege
    //            level that is allowed to execute the segment
    unsigned int            dc      :   1;

    // ex: executable bit
    //      0 --> not executable, data selector
    //      1 --> executable, code selector
    unsigned int            ex      :   1;

    // set to one 
    unsigned int            one     :   1;

    // privl: privilege, contains the ring level
    //      0 --> highest (kernel mode)
    //      1 --> lowest (user space)
    unsigned int            privl   :   2;

    // pr: present bit, must be 1 for all valid sectors 
    unsigned int            pr      :   1;
} __attribute__((packed)) access_t;

typedef struct {
    uint16_t        limit_00_15;
    uint16_t        base_00_15;
    uint8_t         base_16_23;
    access_t        access;
    unsigned int    limit_16_19 :   4;

    // flags
    unsigned int    zero        :   2;

    // sz: size bit
    //      0 --> 16 bit protected mode
    //      1 --> 32 bit protected mode 
    unsigned int    sz          :   1;

    // gr: granularity bit
    //      0 --> <limit> is in 1 byte blocks (byte granularity)
    //      1 --> <limit> is in 4 KiB blocks (page granularity)
    unsigned int    gr          :   1;

    uint8_t         base_24_31;
} __attribute__((packed, aligned(8))) gdt_entry_t;

// GDT register
typedef struct {
    uint16_t    size;
    uint32_t    offset;
} __attribute__((packed, aligned(8))) gdtr_t;


// the GDT table
#define NUM_ENTRIES_MAX     3
static gdt_entry_t _[NUM_ENTRIES_MAX];


/*******************************************************************************
 GDTR manipulation
*******************************************************************************/

// set offset of in-memory copy of the GDTR
static inline
void _mgdtr_set_offset(gdtr_t *gdtr, uint32_t offset)
{
    // linear address of the table itself => paging applies!
    gdtr->offset = (uint32_t) offset;   
}

// set length of in-memory copy of the GDTR
static inline
int _mgdtr_set_length(gdtr_t *gdtr, uint16_t length)
{
    if (!length)
        return 1;

    if (length > NUM_ENTRIES_MAX)
        return 1;

    gdtr->size = (uint16_t) (length * sizeof(gdt_entry_t) - 1);
    return 0;
}

// load the in-memory copy of the GDTR to the actual register
static inline
void _gdtr_load(const gdtr_t *gdtr)
{
    asm volatile (
                    "lgdt (%0)\n\t"
                    : // no output
                    : "a" (gdtr)
                    : "memory"
                 );
}

// retrieve the value of the GDTR
static inline
gdtr_t gdtr_store(void)
{
    gdtr_t gdtr;

    asm volatile (
                    "sgdt %0\n\t"
                    : "=m" (gdtr) 
                 );

    return gdtr; 
}

static
int _gdtr_set(uint32_t offset, uint16_t length)
{
    gdtr_t gdtr;

    _mgdtr_set_offset(&gdtr, offset);

    if (_mgdtr_set_length(&gdtr, length))
        return 1;

    _gdtr_load(&gdtr);

    return 0;
}
/******************************************************************************/


/*******************************************************************************
 manipulation of GDT entries
*******************************************************************************/
static inline
uint32_t _gdt_entry_get_base(const gdt_entry_t *gdte)
{
    uint32_t base;

    base  = gdte->base_00_15;
    base |= (uint32_t) gdte->base_16_23 << 16;
    base |= (uint32_t) gdte->base_24_31 << 24;

    return base;
}

static inline
uint32_t _gdt_entry_get_limit(const gdt_entry_t *gdte)
{
    uint32_t limit;

    limit  = gdte->limit_00_15;
    limit |= (uint32_t) gdte->limit_16_19 << 16;

    return limit; 
}

static inline
void _gdt_entry_set_base(gdt_entry_t *gdte, uint32_t base)
{
    gdte->base_00_15 = (uint16_t) base;
    gdte->base_16_23 = (uint8_t) (base >> 16);
    gdte->base_24_31 = (uint8_t) (base >> 24);
}

static inline
void _gdt_entry_set_limit(gdt_entry_t *gdte, uint32_t limit)
{
    gdte->limit_00_15 = (uint16_t) limit;
    gdte->limit_16_19 = limit >> 16 & 0x0fU;
}

static inline
void _gdt_entry_set_flags(gdt_entry_t *gdte)
{
    gdte->zero = 0;
    gdte->sz = 1; // 32 bit protected mode
    gdte->gr = 1; // page granularity
}

static inline
void _gdt_entry_set_access(gdt_entry_t *gdte,
                           bool executable, unsigned int privilege)
{
    gdte->access.ac = 0; 
    gdte->access.rw = 1;    // code segment readable, data segment writable
    gdte->access.dc = 0;
    gdte->access.ex = executable;   // whether code or data segment
    gdte->access.one = 1;
    gdte->access.privl = privilege & 0x3;
    gdte->access.pr = 1;    // segment valid
}

// set a GDT entry prior to adding it to the GDT table
static
void _gdt_entry_set(gdt_entry_t *gdte,
                    uint32_t base, uint32_t limit,
                    bool executable, unsigned int privilege)
{
    _gdt_entry_set_base(gdte, base);
    _gdt_entry_set_limit(gdte, limit);
    _gdt_entry_set_access(gdte, executable, privilege); 
    _gdt_entry_set_flags(gdte);
}

static void _gdt_entry_set_code(gdt_entry_t *gdte, unsigned int privilege)
{
    _gdt_entry_set(gdte, 0x00000000, 0x000fffff, true, privilege);
}

static void _gdt_entry_set_data(gdt_entry_t *gdte, unsigned int privilege)
{
    _gdt_entry_set(gdte, 0x00000000, 0x000fffff, false, privilege);
}
/******************************************************************************/


static int _add_entry(uint16_t idx, const gdt_entry_t *gdte)
{
    if (idx >= NUM_ENTRIES_MAX)
        return 1;

    // copy entry in table
    _[idx] = *gdte;
    return 0;
}

// call this function after modifying the GDT to update the segment selectors
static void _update_segments(unsigned int code, unsigned int data)
{
    // code and data are really entry indexes in the GDT 
    code *= 8;
    data *= 8;

    asm volatile (
                    "push %0\n\t"     // load CS
                    "push $1f\n\t"
                    "lret\n"
                    "1:\n\t"            // load DS, ES, FS, GS and SS
                    "movw %1, %%ax\n\t"
                    "movw %%ax, %%ds\n\t"
                    "movw %%ax, %%es\n\t"
                    "movw %%ax, %%fs\n\t"
                    "movw %%ax, %%gs\n\t"
                    "movw %%ax, %%ss\n\t"
                    :
                    : "g" (code), "g" (data)
                    : "%ax"  
                 );
}


int gdt_init(void)
{
    gdt_entry_t gdte;

    // 1st the null descriptor
    bzero(&gdte, sizeof(gdte));
    if (_add_entry(0, &gdte))
        return 1;

    // 2nd the code segment descriptor
    _gdt_entry_set_code(&gdte, 0);
    if (_add_entry(1, &gdte))
        return 1;

    // 3rd the data segment descriptor 
    _gdt_entry_set_data(&gdte, 0);
    if (_add_entry(2, &gdte))
        return 1;
    
    if (_gdtr_set((uint32_t) &_[0], NUM_ENTRIES_MAX))
        return 1;

    {
        void gdt_update_segments(uint32_t code_idx, uint32_t data_idx);
        // update the segment selector registers
        // TODO call the gas function instead of the inline
        _update_segments(1, 2);
    }

    return 0;
}

static void _entry_display(gdt_entry_t *gdte)
{
#if 0
    printf("Base:          %x\n", gdte->base_16_23 << 24 
                | gdte->base_16_23 << 16 | gdte->base_00_15); 

    printf("Limit:         %x\n", (gdte->limit_16_19 & 0x0f) << 16 
                        | gdte->limit_00_15);
    {
        uint8_t access;
        
        memcpy(&access, &gdte->access, 8);
        printf("Access:        %x\n", access);
    }
    printf("Flags:         %x\n", gdte->gr << 3 | gdte->sz << 2 | gdte->zero);
#endif

    int i;
    uint16_t word;
    uint8_t byte;

    for (i = 0; i < 2; i++) {
        memcpy(&word, (uint16_t *) gdte + i, 2);
        printf("%04x\n", word);   
    }

    for (i = 0; i < 4; i++) {
        memcpy(&byte, (uint8_t *) gdte + i + 4, 1);
        printf("%02x\n", byte);   
    }
}

void gdt_display(void)
{
    for (int i = 0; i < NUM_ENTRIES_MAX; i++) {
        printf("--- GDT Entry %0.1x ---\n", i);    
        _entry_display(&_[i]);
    }
}
