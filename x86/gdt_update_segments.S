.text
.align 4
.global gdt_update_segments
gdt_update_segments:
    push %ebp
    movl %esp, %ebp

    // reload data segment
    pop  %eax       // data segment GDT's index
    shll $3, %eax   // data segment GDT's byte-based offset
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

    // reload code segment
    pop %eax        // code segment GDT's index
    shll $3, %eax   // code segment byte-based offset
    push %ax        // new code selector
    push $cs_loaded // new address
    lret
cs_loaded:
    leave
    ret
