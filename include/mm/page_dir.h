#pragma once

#include <mm/common.h>

#include <stdbool.h>
#include <stdint.h>

void page_dir_init(void);
void page_dir_load(void);
bool page_dir_entry_is_present(uint_fast16_t entry_num);
int page_dir_set_entry(uint_fast16_t entry_num, uint32_t paddr);
int page_dir_automap(void);
