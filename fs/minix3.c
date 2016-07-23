#include <minix3.h>

#include <superblock.h>
#include <blkpool.h>
#include <ipool.h>
#include <minix3_dir_entry.h>

// XXX
#include <stdio.h>

#define INODES_PER_BLOCK        (BLOCK_SIZE/sizeof(minix3_inode_t))

static const superblock_t *_sb;

int minix3_init()
{
    if (!(_sb = superblock_get()))
        return 1;

    return 0;
}

static inline
blk_num_t _itable_blk_num()
{
    blk_num_t blk_num;

    // 1st block is empty, 2nd block is the superblock
    blk_num = 2;

    // blocks containing the inode bitmap
    blk_num += _sb->_->s_imap_blocks;

    // blocks containing the block bitmap
    blk_num += _sb->_->s_zmap_blocks;

    return blk_num;
}

static inline
blk_num_t _ino2blk(ino_num_t ino, unsigned *lioff)
{
    // calculate the local inode offset
    *lioff = (ino - MINIX3_ROOT_INODE_NUM) % INODES_PER_BLOCK;

    // calculate the block number containing the requested inode
    return (ino - MINIX3_ROOT_INODE_NUM) / INODES_PER_BLOCK
            + _itable_blk_num();
}

/*******************************************************************************
 * low-level function for reading the inode
 ******************************************************************************/
int minix3_readi(ino_num_t ino_num, minix3_inode_t *ino)
{
    if (ino_num < MINIX3_ROOT_INODE_NUM ||
        ino_num - MINIX3_ROOT_INODE_NUM >= _sb->_->s_ninodes)
    {
        // TODO error msg: invalid inode number
        return 1;
    }

    unsigned lioff;
    blk_num_t blk_num = _ino2blk(ino_num, &lioff);

    // fetch inode
    {
        bufblk_t *bufblk;

        if (!(bufblk = blkpool_getblk(blk_num)))
            return 1;

        *ino = lioff[(minix3_inode_t *) &bufblk->block];

        blkpool_putblk(bufblk);
    }

    return 0;
}

int minix3_writei(ino_num_t ino_num, const minix3_inode_t *ino)
{
    // TODO
    (void) ino;
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
        case MINIX3_SIND_BLK_IDX:
            return 1;

        case MINIX3_DIND_BLK_IDX:
            return 2;

        case MINIX3_TIND_BLK_IDX:
            return 3;
    }

    return 0; // no indirection
}

#define BLKADDRS_PER_BLOCK  (BLOCK_SIZE/sizeof(blk_num_t))

int minix3_bmap(const minix3_inode_t *ino, uint32_t byte_off,
                blk_num_t *blk_num, unsigned *blk_loff)
{
    blk_num_t lblk_num;

    // calculate logical block number
    lblk_num  = byte_off / BLOCK_SIZE;

    // calculate local (to the block) byte offset
    *blk_loff = byte_off % BLOCK_SIZE;

    unsigned ind_level = _blk2indlevel(lblk_num);

    // block to take at first
    *blk_num = ino->i_zone[lblk_num + ind_level];

    // blocks requiring a level of indirection other than zero
    for (lblk_num -= MINIX3_NUM_DIR_BLKS;
         ind_level; // requires indirection?
         lblk_num /= BLKADDRS_PER_BLOCK, ind_level--)
    {
        unsigned blkaddr_off = lblk_num & (BLKADDRS_PER_BLOCK - 1);

        // pick up a block containing block addresses/numbers
        {
            bufblk_t *bufblk;

            if (!(bufblk = blkpool_getblk(*blk_num)))
                return 1;

            // get block number of the next indirection level (if any)
            *blk_num = blkaddr_off[(blk_num_t *) &bufblk->block];

            blkpool_putblk(bufblk);
        }
    }

    return 0;
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

static bool _component_matches_str(const component_t *c, const char *str)
{
    if (strlen(str) != _component_len(c))
        return false;

    return !strncmp(c->start, str, _component_len(c));
}

static bool _component_is_valid(const component_t *c)
{
    // the first character cannot be the component separator
    if (_is_separator(*c->start))
        return false;

    // a single-char component must not be the separator
    if (_component_matches_str(c, "/"))
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
    minix3_dir_entry_t  *_;

    // buffer block containing the directory entry
    bufblk_t            *bufblk;
} dir_entry_t;

static int
_get_dir_entry(const minix3_inode_t *ino, dir_entry_t *de, uint32_t byte_off)
{
    blk_num_t blk_num;
    unsigned byte_loff;

    if (minix3_bmap(ino, byte_off, &blk_num, &byte_loff))
        return 1;

    if (!(de->bufblk = blkpool_getblk(blk_num)))
        return 1;

    de->_ = (minix3_dir_entry_t *)
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
    size_t len = MINIX3_FILENAME_LEN_MAX;

    strncpy(filename, de->_->name, len);
    filename[len] = '\0';
    fprintf(stderr, "filename: <%s>\n", filename);
}

// TODO explanation comment
static inode_t *_get_dir_entry_inode(inode_t *dir_ino, component_t *c)
{
    dir_entry_t de;
    bool must_check_is_dir = false;

    // check whether the component has a trailing '/'
    if (_is_separator(*c->end)) {
        // if so, remove the trailing '/', but keep track of it
        c->end--;
        must_check_is_dir = true;
    }

    // iterate over the directory entries until requested component is found
    for (uint32_t byte_off = 0;
         byte_off < dir_ino->dinode.i_size;
         byte_off += sizeof(minix3_dir_entry_t))
    {
        if (_get_dir_entry(&dir_ino->dinode, &de, byte_off))
            return NULL;

        // XXX
        _display_dir_entry(&de);

        {
            char name[MINIX3_FILENAME_LEN_MAX+1];
            memcpy(name, de._->name, MINIX3_FILENAME_LEN_MAX);
            name[MINIX3_FILENAME_LEN_MAX] = '\0';

            if (!_component_matches_str(c, name)) {
                _put_dir_entry(&de);
                continue;
            }
        }

        // component match
        {
            inode_t *ino;
            ino_num_t ino_num = de._->inode;
            _put_dir_entry(&de);

            if (ino_num == dir_ino->num) {
                // handle the case in which the requested inode is the same
                // as the inode of the directory being traversed
                ino = dir_ino;
                ino->count++;
            } else
                if (!(ino = ipool_geti(ino_num)))
                    return NULL;

            if (must_check_is_dir && !minix3_inode_is_dir(&dir_ino->dinode)) {
                ipool_puti(ino);
                return NULL;
            }

            return ino;
        }

    }

    // entry not found in directory
    return NULL;
}

inode_t *minix3_namei(const char *filepath)
{
    inode_t *wino;  // working inode

    if (!*filepath) {
        // TODO error msg // empty filepath
        return NULL;
    }

    if (_is_separator(*filepath)) {
        filepath++;
        // TODO task's root dir instead
        wino = ipool_geti(MINIX3_ROOT_INODE_NUM);
    } else
        // TODO inode of task's CWD instead
        wino = ipool_geti(MINIX3_ROOT_INODE_NUM);

    if (!wino)
        return NULL;

    component_t c;

    while (*filepath) {
        filepath = _component_set(&c, filepath);

        if (!inode_is_dir(wino))
            return NULL;

        // TODO check permissions

        // XXX
        _component_display(&c);

        if (!_component_is_valid(&c)) {
            // TODO error msg
            return NULL;
        }

        // TODO task's root dir instead of MINIX3_ROOT_INODE_NUM
        // needed in case chroot()ed
        if (MINIX3_ROOT_INODE_NUM == wino->num &&
            _component_matches_str(&c, ".."))
        {
            continue;
        }

        // update working inode
        {
            inode_t *new_wino;

            if (!(new_wino = _get_dir_entry_inode(wino, &c))) {
                // TODO error msg: "file or directory not found"
                return NULL;
            }

            ipool_puti(wino);
            wino = new_wino;
        }

    }

    return wino;
}
