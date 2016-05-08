#include <config/startup.h>

.text
.globl _startup
_startup:
    // set up the stack
    movl    $KERNEL_STACK_TOP, %esp
    jmp kmain  // kernel's entry point
