#pragma once

#include <fs/inode.h>

void minix3_imap_init(unsigned num_inodes);
void minix3_imap_lock();
void minix3_imap_unlock();
int minix3_imap_seti(ino_num_t);
int minix3_imap_clri(ino_num_t);
ino_num_t minix3_imap_get_free();
