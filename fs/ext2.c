#include <ext2.h>

#include <ext2_super.h>
#include <ext2_inode.h>
#include <ext2_bgdt.h>
#include <blkpool.h>
#include <bgdpool.h>
#include <inode.h>
#include <ipool.h>

// XXX
#include <stdio.h>

#define ROOT_INODE  2

static const char *_strerr = "unknown";

static struct {
    const ext2_super_t *super;
} _;

int ext2_init()
{
    if (ext2_super_init()) {
        _strerr = ext2_strerr();
        return 1;
    }

    _.super = ext2_super_get();

    return 0;
}

const char *ext2_strerr()
{
    return _strerr;
}

static inline
unsigned _ino2grp(ino_num_t ino_num)
{
    return (ino_num - 1) / (_.super->inodes_per_group);
}

static inline
unsigned _ino2lidx(ino_num_t ino_num)
{
    return (ino_num - 1) % (_.super->inodes_per_group);
}

/*******************************************************************************
 for transforming a filepath into an inode
*******************************************************************************/
typedef struct {
    const char *start, *end;
} component_t;

static inline
bool _is_eoi(char c)
{
    return '\0' == c;
}

static inline
bool _is_separator(char c)
{
    return '/' == c;
}

static bool _component_is_valid(const component_t *c)
{
    // the first character cannot be the component separator
    if (_is_separator(*c->start))
        return false;

    // a single-char component must not be the separator
    if (c->start == c->end)
        return false;

    return true;
}

static void _component_display(const component_t *c)
{
    char str[256];
    unsigned len = c->end - c->start + 1;

    strncpy(str, c->start, len);
    str[len] = '\0';
    printf("<%s> valid: %s\n", str, _component_is_valid(c) ? "yes" : "no");
}

static const char *_component_set(component_t *c, const char * const filepath)
{
    if (!*filepath)
        return filepath;

    for (c->start = c->end = filepath; !_is_eoi(c->end[1]); c->end++)
        if (_is_separator(c->end[1])) {
            if (_is_eoi(c->end[2])) {
                c->end++;   // perserve the '/' at the end of the pathname
                return c->end + 1;
            }
            return c->end + 2;
        }
    // end of string found
    return c->end + 1;
}

inode_t *ext2_namei(const char *filepath)
{
    inode_t *wino;  // working inode

    if (!*filepath)
        return NULL; // empty filepath

    if (_is_separator(*filepath)) {
        filepath++;
        wino = ipool_geti(ROOT_INODE_NUM);
    } else
        wino = ipool_geti(ROOT_INODE_NUM); // TODO inode of task's CWD instead

    component_t c;

    while (*filepath) {
        filepath = _component_set(&c, filepath);
        //_component_display(&c);

        if (!_component_is_valid(&c))
            return NULL;

        // TODO
    }

    return NULL;
}

/*******************************************************************************
 * low-level function for reading an inode from disk
 ******************************************************************************/
int ext2_readi(ino_num_t ino_num, ext2_inode_t *ino)
{
    #define INODES_PER_BLOCK    (sizeof(block_t)/sizeof(ext2_inode_t))
    bufbgd_t *bufbgd;

    // read the corresponding block group descriptor
    // determine which block group contains the requested inode
    unsigned grp_num = _ino2grp(ino_num);

    if (!(bufbgd = bgdpool_getbgd(grp_num)))
        goto err;
   
    // XXX 
    ext2_bgd_display(&bufbgd->bgd);

    // determine the local index inside the table
    unsigned lidx = _ino2lidx(ino_num);

    // determine the block containing the inode
    uint32_t iblk = bufbgd->bgd.inode_table + lidx / INODES_PER_BLOCK;

    // pick up the block
    {
        bufblk_t *bufblk;

        if (!(bufblk = blkpool_getblk(iblk)))
            goto rel_bufbgd;
   
        // copy the requested inode
        *ino = (lidx % INODES_PER_BLOCK)[(ext2_inode_t *) &bufblk->block];

        blkpool_putblk(bufblk);
    }

    bgdpool_putbgd(bufbgd);
    return 0;

rel_bufbgd:
    bgdpool_putbgd(bufbgd);

err:
    return 1;
    #undef INODES_PER_BLOCK
}

/*******************************************************************************
 * low-level function for reading an inode from disk
 ******************************************************************************/
int ext2_writei(ino_num_t ino_num, const ext2_inode_t *ino)
{
    // TODO
    (void) ino_num;
    (void) ino;
    return 0;
}

/*******************************************************************************
 * low-level function for reading a block group descriptor from disk
 ******************************************************************************/
int ext2_read_bgd(uint32_t grp_num, ext2_bgd_t *bgd)
{
    // for 1k-blocks the block group descriptor table begins at block 2
    static const blk_num_t bgdt_blk = 2;

    bufblk_t *bufblk;
    unsigned blk_num;
    unsigned lidx;

    // determine the block where the block group descriptor is
    blk_num = bgdt_blk + grp_num * sizeof(ext2_bgd_t) / sizeof(block_t);

    if (!(bufblk = blkpool_getblk(blk_num)))
        return 1;
   
    // local index of the block group descriptor inside the block
    lidx = grp_num * sizeof(ext2_bgd_t) % sizeof(block_t);
   
    // copy the block group descriptor 
    *bgd = lidx[(ext2_bgd_t *) &bufblk->block];

    blkpool_putblk(bufblk);
    
    return 0;
}

blk_num_t ext2_bmap(inode_t *ino, uint32_t boff, uint32_t *blk_loff)
{
    // TODO add ouput byte offset into block
    return 0;
}
