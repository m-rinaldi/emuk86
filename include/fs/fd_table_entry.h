#pragma once

#include <stdbool.h>
#include <fs/file_table.h>

typedef struct {
    bool                valid;
    file_table_entry_t  *fte;
    uint8_t             flags;
} fd_table_entry_t;
