#pragma once

#include <proc/process.h>

// currently running process
extern process_t *cur_proc;

int ptable_init();
