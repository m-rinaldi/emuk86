#include <x86/common.h>

# TODO struct's passing convention could be broken in a future GCC version
.macro define_cr_get num
.globl cr\num\()_get
.align FUNCTION_ALIGNMENT
cr\num\()_get:
    # crN_t crN_get(void)
    # GCC passes the address of the struct on the stack
    # not the caller, but the callee cleans up the stack
    movl    4(%esp), %eax
    movl    %cr\num, %edx
    movl    %edx, (%eax)
    ret     $4
.endm

.macro define_cr_set num
.globl cr\num\()_set
.align FUNCTION_ALIGNMENT
cr\num\()_set:
    movl    4(%esp), %eax
    movl    %eax, %cr\num
    ret
.endm

define_cr_get 0
define_cr_set 0

# CR1 is reserved

define_cr_get 2
define_cr_set 2

define_cr_get 3
define_cr_set 3

# CR4 is not yet used
