#pragma once

#include <stdbool.h>
#include <ext2_super.h>

typedef struct {
    bool            valid;
    bool            locked;
    bool            dirty;

    ext2_super_t    *_;
} super_t;

super_t *super_get();
void super_put(super_t *);
