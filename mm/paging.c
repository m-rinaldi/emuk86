#include <mm/paging.h>

#include <mm/page_dir.h>
#include <mm/page_tables.h>
#include <x86/cr.h>

#include <stddef.h>

static inline
uint_fast16_t _vaddr2pd_idx(uint32_t vaddr)
{
    return vaddr >> 22;
}

static inline
uint_fast16_t _vaddr2pt_idx(uint32_t vaddr)
{
    return vaddr >> 12 & 0x3ff;
}

static int _setup_identity(void)
{
    uint32_t addr;

    for (addr = 0; addr + PAGE_SIZE-1 <= PADDR_MAX; addr += PAGE_SIZE)
        if (paging_map(addr, addr, true))
            return 1;

    return 0;
}

// TODO implemente this function set the CR3
// TODO uint32_t _set_page_dir_paddr()

static inline
uint32_t _get_page_dir_paddr(void)
{
    // CR3 contains the page directory's physical address
    return cr3_get().pdbr;
}

#if 0
static inline
void _set_page_dir_paddr(uint32_t paddr)
{
    cr3_t cr3 = cr3_get();
    cr3.pdbr = paddr; 
}
#endif

static inline
void _enable_paging(void)
{
    cr0_t cr0 = cr0_get();
    cr0.pg = 1;
    cr0_set(cr0);
}

bool paging_is_enabled(void)
{
    return cr0_get().pg;
}

static inline
bool _page_is_mapped(uint32_t page_num)
{
    // assume that the first page is never mapped at this point
    return paging_vaddr2paddr(page_num << 12);
}

static inline
void _enable_write_protect(void)
{
    cr0_t cr0 = cr0_get();
    cr0.wp = 1;
    cr0_set(cr0);
}

static bool _is_page_writable(uint32_t page_num)
{
    uint32_t *page_table;
    uint_fast16_t pt_idx;

    // from the given vaddr we build another vaddr consisiting of:
    // vaddr' = < 0xffc,  pd_idx, 0 >

    page_table = (uint32_t *) (0x3ffU << 22 | (0x3ff & page_num >> 10) << 12);
    pt_idx = (uint16_t) (0x3ff & page_num);

    return page_table[pt_idx] & 0x2; 
}

// XXX
#include <stdio.h>

#define NUM_PAGES_MAX           (1 << 20)   // 1M pages
static int _keep_config(void)
{
    extern uint32_t KERNEL_START[], KERNEL_END[], KERNEL_STACK_BOTTOM[],
        KERNEL_STACK_TOP[], KERNEL_BSS_END[];
    printf("\nKERNEL_START: %08x\n", KERNEL_START);
    printf("KERNEL_BSS_END: %08x\n", KERNEL_BSS_END);
    printf("KERNEL_STACK_BOTTOM: %08x\n", KERNEL_STACK_BOTTOM);
    printf("KERNEL_STACK_TOP: %08x\n", KERNEL_STACK_TOP);

    // TODO map VGA memory to the higher half
    {
        paging_map(0xb8000, 0xb8000, true);   
    }

    // do not copy the last entry in the page dir, since it doesn't
    // really have a page table backing it up
    // the "automap" page dir entry will be set up later 
    for (size_t page_num = (size_t) KERNEL_START >> 12;
                                        page_num <= (uint32_t) KERNEL_END >> 12;
                                        page_num++)
        if (_page_is_mapped(page_num)) {
            bool writable = _is_page_writable(page_num);
            uint32_t vaddr = page_num << 12;

            // XXX            
            printf("allocating page: %05x : %s\n", page_num,
                   writable ? "W" : "R");

            if (paging_map(vaddr, paging_vaddr2paddr(vaddr), writable))
                return 1;
        }
    return 0;
}

int paging_init(void)
{
    page_tables_init();
    page_dir_init();

    // if paging was enabled by the bootloader, then copy the 
    // current configuration
    if (paging_is_enabled()) {
        if (_keep_config())
            return 1;

        // map the last page dir entry to itself
        if (page_dir_automap())
            return 1;

        // XXX
        printf("about to load PD\n");

        // now that the page dir and page tables are set up, load the page dir
        page_dir_load();

        _enable_write_protect();

        return 0;
    }

    if (_setup_identity())
        return 1;

    // map the last page dir entry to itself
    if (page_dir_automap())
        return 1;

    _enable_paging();

    return 0;
}

static inline
void _reset_mmu_cache(void)
{
    asm volatile
        (
            "# _reset_mmu_cache()\n\t"
            "movl %%cr3, %%eax\n\t"
            "movl %%eax, %%cr3\n\t" 
            :   // no output
            :   // no input
            :   "eax"
        );
}


int paging_unmap(uint32_t vaddr)
{
    uint_fast16_t pd_idx;
    uint_fast16_t pt_idx;

    if (!_is_addr_page_aligned(vaddr))
        return 1;
    
    // localize the entry in the page directory
    pd_idx = _vaddr2pd_idx(vaddr);

    // localize the entry in the pointed page table
    pt_idx = _vaddr2pt_idx(vaddr);
    
    if (!page_dir_entry_is_present(pd_idx))
        return 0;
    
    if (page_tables_clear_entry(pd_idx, pt_idx))
        return 1;

    _reset_mmu_cache();
    return 0;
}

int paging_map(uint32_t vaddr, uint32_t paddr, bool writable)
{
    uint_fast16_t pd_idx;
    uint_fast16_t pt_idx;    

    if (!_is_addr_page_aligned(vaddr) || !_is_addr_page_aligned(paddr))
        return 1;

    // out of for-use-available physical memory
    // TODO fix possible overflow, add: padd > UINT32_MAX - PAGE_SIZE
    if (paddr + PAGE_SIZE-1 > PADDR_MAX)
        return 1;

    // localize the entry in the page directory
    pd_idx = _vaddr2pd_idx(vaddr);

    // localize the entry in the pointed page table
    pt_idx = _vaddr2pt_idx(vaddr);

    if (!page_dir_entry_is_present(pd_idx)) {
        // set the corresponding entry in the page directory
        uint32_t pt_vaddr, pt_paddr;

        // get the paddr of the page table this page dir entry will point at
        if (!(pt_vaddr = page_tables_get_vaddr(pd_idx)))
            return 1;

        // no identity paging is assumed
        pt_paddr = paging_vaddr2paddr(pt_vaddr);
        
        if (page_dir_set_entry(pd_idx, pt_paddr))
            return 1;
    }

    // TODO pass writable
    if (page_tables_set_entry(pd_idx, pt_idx, paddr, writable))
        return 1;

    _reset_mmu_cache();

    return 0;
}

// for the currently working page directory
// the page directory may be other than the local structure
// this function is for the page directory currently being used by the HW
uint32_t paging_vaddr2paddr(uint32_t vaddr)
{
    /*
        vaddr  = < pd_idx, pt_idx, phy >
        
        from the given vaddr we build another vaddr consisiting of:
        vaddr' = < 0xffc,  pd_idx, pt_idx >
    */
    uint32_t *pd, *pt;
    uint_fast16_t pd_idx = vaddr >> 22;
    uint_fast16_t pt_idx = vaddr >> 12 & 0x3ff;

    pd = (uint32_t *) (0xffc00000 | 0x3ff << 12);
    if (!(pd[pd_idx] & 1))
        return 0; // corresponding page directory entry not present

    pt = (uint32_t *) (0xffc00000 | (uint32_t) pd_idx << 12);
    if (!(pt[pt_idx] & 1))
        return 0; // page table entry not present
        
    return (uint32_t) ((pt[pt_idx] & ~0xfffU) | (vaddr & 0xfffU));    
}
