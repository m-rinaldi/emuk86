#include <x86/common.h>

.macro define_cr_get num
.globl cr\num\()_get
.align FUNCTION_ALIGNMENT
cr\num\()_get:
    movl    %cr\num, %eax
    ret 
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

// CR1 is reserved

define_cr_get 2
define_cr_set 2

define_cr_get 3
define_cr_set 3

// CR4 is not yet used

