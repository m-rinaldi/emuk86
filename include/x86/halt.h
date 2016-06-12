#pragma once

static inline
void halt(void)
{
    asm volatile ("# halt()\n\thlt\n\t" : : : "memory");
}
