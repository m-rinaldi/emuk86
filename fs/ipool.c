#include <ipool.h>

#include <ext2.h>
#include <list.h>
#include <inode.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>

#define POOL_SIZE   8
static inode_t _[POOL_SIZE];

static list_t _free_list;

// TODO define a macro DEFINE_ITERATE(array, len, type)
//                     DEFINE_ITERATE(_, POOL_SIZE, inode_t)
typedef void (* iterate_func_t)(inode_t *);
static void _iterate(iterate_func_t func_do)
{
    for (int i = 0; i < POOL_SIZE; i++)
        func_do(_ + i);
}

// TODO define a macro DEFINE_ITERATE_SEL(array, len, type)
//                     DEFINE_ITERATE_SEL(_, POOL_SIZE, inode_t)
// iterates over the pool for selecting an element
typedef bool (*iterate_sel_func_t)(const inode_t *, ino_num_t);
static inode_t *_iterate_sel(iterate_sel_func_t does_match, ino_num_t ino_num)
{
    for (int i = 0; i < POOL_SIZE; i++)
        if (does_match(_ + i, ino_num))
            return _ + i;

    return NULL;
}

static inline
void _init(inode_t *ino)
{
    bzero(ino, sizeof(*ino));

    node_init(&ino->free_node);
    // add inode to the free list of inodes
    list_insert(&_free_list, &ino->free_node);
}

void ipool_init()
{
    list_init(&_free_list);

    _iterate(_init);
}


static bool _is_ino_num(const inode_t *ino, ino_num_t ino_num)
{
    return ino_num == ino->num;
}

// get any inode from the free list of inodes
static inode_t *_get_free_inode()
{
    list_node_t *node;

    if (!(node = list_get_head_node(&_free_list)))
        return NULL;

    node_remove(node);
    return node_get_container(node, inode_t, free_node);
}

static int _release_inode(inode_t *ino)
{
    if (!--ino->count) {
        if (ino->valid && ino->dirty) {
            // write inode back to disk
            inode_lock(ino);

            if (ext2_writei(ino->num, &ino->dinode)) {
                inode_unlock(ino);
                ino->valid = false;
                // TODO put at the head of the list instead
                list_insert(&_free_list, &ino->free_node);
                return 1;
            }

            ino->dirty = false;
        }

        list_insert(&_free_list, &ino->free_node);
    }

    inode_unlock(ino);
    return 0;
}

static inline
bool _is_inode_on_free_list(const inode_t *ino)
{
    // inode on the free list <--> count is zero
    return !ino->count;
}

inode_t *ipool_geti(ino_num_t ino_num)
{
    inode_t *ino;

//beginning:
    if ((ino =_iterate_sel(_is_ino_num, ino_num))) {
        if (inode_is_locked(ino)) {
            // TODO sleep on event "this inode becomes free"
            // XXX
            while (1)
                ;
            // TODO goto beginning;
        }

        if (_is_inode_on_free_list(ino))
            node_remove(&ino->free_node);

        ino->count++;
        return ino;
    }
    
    // inode not found in the pool, pick up a free one
    if (!(ino = _get_free_inode()))
        return NULL;

    inode_lock(ino);
    ino->count++;

    // reset inode
    ino->num = ino_num;
    ino->valid = false;

    // perform a read of the contents of the inode on disk
    if (ext2_readi(ino_num, &ino->dinode))
        goto err;

    ino->valid = true;

    return ino; // no free inode

err:
    _release_inode(ino);
    return NULL;
}


int ipool_puti(inode_t *ino)
{
    return _release_inode(ino);
}
