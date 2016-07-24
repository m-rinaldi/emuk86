#pragma once

#include <mm/common.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE       4096

#define WRITABLE_PAGE   true
#define RDONLY_PAGE     false

int paging_init(void);
bool paging_is_enabled(void);
int paging_map(uint32_t vaddr, uint32_t paddr, bool writable);
int paging_unmap(uint32_t vaddr);

uint32_t paging_vaddr2paddr(uint32_t vaddr);
