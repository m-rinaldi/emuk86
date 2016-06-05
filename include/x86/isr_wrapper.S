.macro isr_wrapper name
.globl _\name\()_isr
.align 4
_\name\()_isr:
    pushal
    cld /* sysV ABI requires DF to be clear on function entry */
    call \name\()_isr
    popal
    iret
.endm
