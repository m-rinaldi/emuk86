OUTPUT_FORMAT("elf32-i386")
ENTRY(_startup)
STARTUP(startup.o)
OUTPUT(kernel.elf)

SECTIONS 
{
    /*          VMA                 LMA           */
    .text.start (0xc0000000)    :   AT(0x00100000) {
        startup.o(.text)
    }

    .text : ALIGN(0x1000) {
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
}

