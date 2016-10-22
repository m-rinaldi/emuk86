#include <proc/process.h>

#include <proc/ptable.h>
#include <string.h>
#include <stdio.h>

size_t process_set_err_msg(const char *err_msg)
{
    process_t *proc = cur_proc;
    
    size_t len = strlen(err_msg);
    len = len > PROCESS_ERR_MSG_LEN ? PROCESS_ERR_MSG_LEN : len;
    
    memcpy(proc->err_msg, err_msg, len);
    proc->err_msg[PROCESS_ERR_MSG_LEN] = '\0';

    proc->has_err_msg = true;
    
    return len;
}


void process_display_err_msg()
{
    process_t *proc = cur_proc;

    printf("%d: ", proc->pid);
    if (cur_proc->has_err_msg)
        printf("%s\n", proc->err_msg);
    else
        printf("no error message available\n", proc->err_msg);
}
