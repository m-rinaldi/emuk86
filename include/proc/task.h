#pragma once

#include <fs/inode.h>
#include <stdint.h>

typedef struct {
    // "general-purpose" registers
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    // index registers
    uint32_t esi;
    uint32_t edi;

    // stack pointers
    uint32_t esp;
    uint32_t ebp;

    // program counter
    uint32_t eip;

    // status register
    uint32_t eflags;

    // control register
    uint32_t cr3;
} registers_t;

typedef struct {
    // TODO registers
} task_t;
