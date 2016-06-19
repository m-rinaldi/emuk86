#include <bgdpool.h>

#include <ext2.h>

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define POOL_SIZE   8
bufbgd_t _[POOL_SIZE];

static void _init(bufbgd_t *bgd)
{
    bzero(bgd, sizeof(*bgd));
}

// TODO generic iterator?
// static bufbgd_t *_iterate()
// then have three different iterators on top of this one:
//      1) iterate_all()
//      2) iterate_sellect()
//      3) iterate_sellect_value()

void bgdpool_init()
{
    for (int i = 0; i < POOL_SIZE; i++)
        _init(_ + i);
}

static bufbgd_t *_get_free()
{
    for (int i = 0; i < POOL_SIZE; i++)
        if (!_[i].locked) {
            return _ + i;  
        }

    return NULL;
}


bufbgd_t *bgdpool_getbgd(uint32_t grp_num)
{
    for (int i = 0; i < POOL_SIZE; i++)
        if (_[i].valid && grp_num == _[i].num) {
            // hit
            if (_[i].locked) {
                // TODO sleep on event "this block group desc" becomes free
                return NULL;
            }
            _[i].locked = true;
            return _ + i;
        }
    
    // miss
    bufbgd_t *bufbgd;

    if (!(bufbgd = _get_free())) {
        // TODO fall asleep on event "any block group desc becomes free"
        return NULL;
    }

    bufbgd->locked = true;
    bufbgd->valid = true;
    bufbgd->num = grp_num;

    if (ext2_read_bgd(grp_num, &bufbgd->bgd)) {
        bufbgd->valid = false;
        bufbgd->locked = false;
        return NULL;
    }

    return bufbgd;
}

void bgdpool_putbgd(bufbgd_t *bgd)
{
    bgd->locked = false;
}
