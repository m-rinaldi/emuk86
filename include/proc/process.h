#pragma once

#include <fs/inode.h>
#include <fs/fd_table.h>
#include <proc/task.h>
#include <proc/types.h>
#include <list.h>

typedef struct {
    pid_t           pid;

    inode_t         *iprog; // inode of the program's binary
    inode_t         *icrd;  // inode of the "current root directory"
    inode_t         *icwd;  // inode of the "current working directory"

    fd_table_t      fdt;

    task_t          task;

    // TODO private region table
    // TODO private page directory

    list_node_t     free;
    list_node_t     active;
} process_t;
