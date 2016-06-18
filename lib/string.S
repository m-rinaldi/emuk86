#include <x86/common.h>

.text
.global bzero
.align FUNCTION_ALIGNMENT
bzero:
    pushl   %ebp
    movl    %esp, %ebp

    pushl   %edi

    movl    8(%ebp), %edi
    movl    12(%ebp), %edx

    movl    %edx, %ecx
    andl    $3, %edx
    shrl    $2, %ecx

    xorl    %eax, %eax

    // direction flag should be zero (increasing addresses)
    rep     stosl
    movl    %edx, %ecx
    rep     stosb
    
    popl    %edi

    leave
    ret     
