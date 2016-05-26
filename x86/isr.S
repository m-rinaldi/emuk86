.macro isr num
.globl isr\num
.align 4
isr\num:
    pushal
    pushl   $\num
    subl    $32, (%esp)
    pushl   $str
    call    printf
    addl    $8, %esp
    popal
    iret
.endm

str:
    .asciz "IRQ: %02d\n"

isr 32
isr 33
isr 34
isr 35
isr 36
isr 37
isr 38
isr 39
isr 40
isr 41
isr 42 
isr 43
isr 44
isr 45
isr 46
isr 47
    
