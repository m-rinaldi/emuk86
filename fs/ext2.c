#include <ext2.h>

#include <ext2_super.h>
#include <ext2_inode.h>
#include <ext2_dir.h>
#include <ext2_bgdt.h>
#include <blkpool.h>
#include <bgdpool.h>
#include <inode.h>
#include <ipool.h>

#include <string.h>
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
 functions for transforming a filepath into an inode
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

static inline
size_t _component_len(const component_t *c)
{
    return c->end - c->start + 1;
}

static bool _component_is_str(const component_t *c, const char *str)
{
    return !strncmp(c->start, str, _component_len(c));
}

static bool _component_is_valid(const component_t *c)
{
    // the first character cannot be the component separator
    if (_is_separator(*c->start))
        return false;

    // a single-char component must not be the separator
    if (_component_is_str(c, "/"))
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
// TODO _component_is_dir() check whether the component ends at "/"

typedef struct {
    union u_ddir_entry {
        ext2_dir_entry_t    _;
        struct {
            uint8_t padding[8];
            char    name[256];
        } st_name;
    } *ddir_entry;

    // buffer block containing the directory entry
    bufblk_t            *bufblk;
} dir_entry_t;

static
int _get_dir_entry(const inode_t *ino, dir_entry_t *de, uint32_t byte_off)
{
    blk_num_t blk_num;
    uint32_t byte_loff;

    blk_num = ext2_bmap(ino, byte_off, &byte_loff);

    if (!(de->bufblk = blkpool_getblk(blk_num)))
        return 1;

    de->ddir_entry = (union u_ddir_entry *)
                            ((uint8_t *) &de->bufblk->block + byte_loff);

    return 0;
}

static void _put_dir_entry(const dir_entry_t *de)
{
    blkpool_putblk(de->bufblk);
}

static void _display_dir_entry(const dir_entry_t *de)
{
    char filename[256];
    size_t len = de->ddir_entry->_.name_len;

    strncpy(filename, de->ddir_entry->st_name.name, len);
    filename[len] = '\0';
    fprintf(stderr, "filename: <%s>\n", filename);
}

inode_t *ext2_namei(const char *filepath)
{
    inode_t *wino;  // working inode

    if (!*filepath)
        return NULL; // empty filepath

    if (_is_separator(*filepath)) {
        filepath++;
        wino = ipool_geti(ROOT_INODE_NUM); // TODO task's root dir instead
    } else
        wino = ipool_geti(ROOT_INODE_NUM); // TODO inode of task's CWD instead

    if (!wino)
        return NULL;

    component_t c;
    uint32_t byte_off = 0;

    while (*filepath) {
        filepath = _component_set(&c, filepath);

        if (!inode_is_dir(wino))
            return NULL;

        // TODO check permissions

        // XXX
        _component_display(&c);

        if (!_component_is_valid(&c))
            return NULL;

        // needed in case chroot()ed
        // TODO task's root dir instead of ROOT_INODE_NUM
        if (ROOT_INODE_NUM == wino->num && _component_is_str(&c, ".."))
            continue;

        // TODO implement function _get_dir_entry_name(component_t *c) instead
        uint32_t byte_off;
        while (byte_off < wino->dinode.size) {
            dir_entry_t de;

            if (_get_dir_entry(wino, &de, byte_off))
                return NULL;

            // byte offset for the next directory entry
            byte_off += de.ddir_entry->_.rec_len;

            // XXX
            _display_dir_entry(&de);

            _put_dir_entry(&de);
        }

        // TODO
    }

    return wino;
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

/*******************************************************************************
 * low-level functions for determining the block number from a byte offset
 ******************************************************************************/
// determines the required level of indirection from the logical block number
static inline
unsigned _blk2indlevel(blk_num_t lblk_num)
{
    switch (lblk_num) {
        case EXT2_SIND_BLK_IDX:
            return 1;

        case EXT2_DIND_BLK_IDX:
            return 2;

        case EXT2_TIND_BLK_IDX:
            return 3;
    }
    return 0;
}

blk_num_t ext2_bmap(const inode_t *ino, uint32_t byte_off, uint32_t *blk_loff)
{
    blk_num_t lblk_num;

    // calculate logical block number
    lblk_num  = byte_off / BLOCK_SIZE;

    // calculate local (to the block) byte offset
    *blk_loff = byte_off % BLOCK_SIZE;

    unsigned ind_level = _blk2indlevel(lblk_num);

    while (ind_level) {
        // TODO blocks requiring a level of indirection other than zero
    }

    return ino->dinode.block[lblk_num];
}


