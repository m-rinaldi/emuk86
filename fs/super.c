#include <super.h>

#include <ext2_super.h>
#include <stddef.h>

// singlton pattern, only working with a single superblock
static super_t _;

super_t *super_get()
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

        if (!(_._ = ext2_read_super())) {
            _.locked = false;
            return NULL;
        }

        _.valid = true;
    }

    return &_;
}

void super_put(super_t *super)
{
    _.locked = true;

    if (_.dirty) {
        // TODO write superblock back to disk
        while (1)
            ;
    }

    _.locked = false;    
}
