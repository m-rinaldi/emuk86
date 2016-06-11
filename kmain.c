#include <stdint.h>
#include <stddef.h>

#include <x86/intr.h>
#include <pic.h>
#include <x86/idt.h>
#include <x86/gdt.h>
#include <console.h>
#include <stdio.h>
#include <mm/paging.h>

#define VGA_MEM_ADDR    0xb8000
#define COLS            80
#define ROWS            25
#define ATTR            0x1f    // white on blue

static inline unsigned int _xy2idx(unsigned int x, unsigned int y)
{
    return y + COLS*x;
}

static void bsod(void)
{
    volatile uint16_t *vga_mem;
    unsigned int i, j;
    
    vga_mem = (uint16_t *) VGA_MEM_ADDR;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++)
            vga_mem[_xy2idx(i, j)] =
                (uint16_t) ATTR << 8 | (0x00ff & vga_mem[_xy2idx(i, j)]);
}

static void _do_page_fault(void)
{
    volatile uint8_t *p;

    p = 0;
    *p = 113;
}

void kmain(void) __attribute__((noreturn));
void kmain(void)
{
    bsod();

    intr_disable();
    
    gdt_init();
    idt_init();
    pic_init();
    
    console_init();
    printf("emuk86\n");
    printf("------\n");

    printf("initializing paging...");
    if (paging_init()) {
        printf("paging_init() error\n");
        goto end;
    }
    printf("ok\n");
    

    //_do_page_fault();

    // XXX
    {
        volatile uint8_t *p = (uint8_t *) kmain;
        *p = 113;
    }

    printf("it souldn't reach this point\n");


    //intr_enable();
end:
    while (1)
        ;
}
