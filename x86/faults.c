#include <x86/faults.h>

#include <vga.h>
#include <console.h>

void exception_handler(void)
{
    vga_set_bsod();
    console_puts_err("emuk86: hardware exception!");

    while (1)
        ; 
}
