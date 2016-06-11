OUTPUT_FORMAT("elf32-i386")
ENTRY(_startup)
STARTUP(startup.o)

SECTIONS 
{
    /*          VMA                 LMA           */
    .text.start (0xc0000000)    :   AT(0x00100000) {
        startup.o(.text)
    }

    .text : ALIGN(0x1000) {
        KERNEL_START        = .;
        KERNEL_TEXT_START   = .;
            *(.text)
        KERNEL_TEXT_END     = .;
    }

    .data : ALIGN(0x1000) {
        KERNEL_DATA_START   = .;
            *(.data)
        KERNEL_DATA_END     = .;
    }

    .bss : ALIGN(0x1000) {
        KERNEL_BSS_START    = .;
            *(.bss)
        KERNEL_BSS_END      = .;

    }
  
    /* make sure a free page is left in between to detect illegal accesses */
    . += 4K;
    
    /* the kernel stack */
    .stack : ALIGN(0x1000) {
        KERNEL_STACK_START  = .;
        KERNEL_STACK_BOTTOM = .;
            . += 4K;
        KERNEL_STACK_END    = .;
        KERNEL_STACK_TOP    = .;
    }
    KERNEL_END              = .;
}

