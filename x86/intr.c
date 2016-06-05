#include <x86/intr.h>
#include <x86/idt.h>

#define IRQ_OFFSET  0x20U
// TODO rename to intr_register_irq_handler()
int intr_register_irq(uint8_t irq_num, void (*isr)(void))
{
    return idt_set_intr_gate(IRQ_OFFSET + irq_num, isr); 
}

