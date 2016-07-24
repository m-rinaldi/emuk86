#include <fs/superblock.h>

#include <fs/minix3_superblock.h>

#include <stddef.h>

// singlton pattern, only working with a single superblock
static superblock_t _;

superblock_t *superblock_get()
{
// beginning:
    if (_.locked) {
        // TODO sleep on event superblock becomes unlocked
        // TODO goto beginning
        while (1)
            ;
    }
  
    _.locked = true;

    if (!_.valid) {
        // read superblock from disk through the pool of block buffers

        if (!(_._ = minix3_superblock_read())) {
            _.locked = false;
            return NULL;
        }

        _.valid = true;
    }

    return &_;
}

// XXX superblock of a mounted fs will not be modified by the OS
void superblock_put(superblock_t *sb)
{
    _.locked = true;

    if (_.dirty) {
        // TODO minix3_superblock_write(sb);

        while (1)
            ;
    }

    _.locked = false;    
}
