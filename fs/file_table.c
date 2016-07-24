#include <fs/file_table.h>

#include <fs/ipool.h>
#include <fs/iterator.h>

#define NUM_ENTRIES     8U

static file_table_entry_t _[NUM_ENTRIES];
static list_t _free_list;

typedef void (* iterate_func_t)(file_table_entry_t *);

DEFINE_ITERATE(_, NUM_ENTRIES, file_table_entry_t, iterate_func_t)

static void _init(file_table_entry_t *fte)
{
    bzero(fte, sizeof(*fte));
    
    node_init(&fte->free_node);
    // add file table entry to the list of free entries
    list_insert(&_free_list, &fte->free_node);
}

void file_table_init()
{
    list_init(&_free_list);

    _iterate(_init);
}

static file_table_entry_t *_alloc_entry()
{
    list_node_t *node;

    if (!(node = list_get_head_node(&_free_list))) {
        // TODO error msg "no free file talbe entries"
        return NULL;
    }

    node_remove(node);
    return node_get_container(node, file_table_entry_t, free_node);
}

file_table_entry_t *file_table_alloc_entry(uint8_t mode, inode_t *ino)
{
    file_table_entry_t *fte;

    if (!(fte = _alloc_entry()))
        return NULL;

    // initialize just allocted file table entry
    fte->count  = 0;
    fte->offset = 0;
    fte->mode   = mode;
    fte->inode  = ino;

    return fte;
}

file_table_entry_t *file_table_dup_entry(file_table_entry_t *fte)
{
    fte->count++;
    return fte;
}

static void _dealloc_entry(file_table_entry_t *fte)
{
    ipool_puti(fte->inode);
    list_insert(&_free_list, &fte->free_node);
}

void file_table_release(file_table_entry_t *fte)
{
    if (!--fte->count)
        _dealloc_entry(fte);
}
