#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>


#define node_get_container(node, type, member)  \
                                ((type *)       \
                                (_node2uint8_t(node) - offsetof(type, member)))

typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
} list_node_t;

// type checking for the first parameter of node_get_container() macro
static inline
uint8_t *_node2uint8_t(const list_node_t *node)
{
    return (uint8_t *) node;
}

/*******************************************************************************
 operations on nodes
 ******************************************************************************/
static inline
void node_init(list_node_t *node)
{
    node->prev = node->next = node;
}

static inline
void node_remove(list_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static inline
list_node_t *node_get_next(const list_node_t *node)
{
    return node->next;
}

static inline
list_node_t *node_get_prev(const list_node_t *node)
{
    return node->prev;
}

// TODO node_insert_before()
// TODO node_insert_after()
/******************************************************************************/

typedef struct {
    // note the difference betwen "list head" and "head node"
    //      "list head" is the dummy node
    //      "head node" is the first node 
    list_node_t head; // list head
} list_t;

/*******************************************************************************
 operations on lists
 ******************************************************************************/
static inline
void list_init(list_t *lst)
{
    // empty list
    node_init(&lst->head);
}

static inline
bool list_is_empty(const list_t * lst)
{
    /**********************************
                list empty
                    ^
                    |
                    v
    lst->head.prev == &lst->head
                    ^
                    |
                    v
    lst->head.next == &lst->head

    ***********************************/
    return lst->head.prev == &lst->head;
}

// is node the first node of the list?
static inline
bool list_is_node_head(const list_t *lst, const list_node_t *node)
{
    return lst->head.next == node;
}

// is node the last node of the list?
static inline
bool list_is_node_tail(const list_t *lst, const list_node_t *node)
{
    return lst->head.prev == node;
}

static inline
bool list_is_dummy_node(const list_t *lst, const list_node_t *node)
{
    return &lst->head == node;
}

// for iterating over the list
static inline
bool list_is_iteration_over(const list_t *lst, const list_node_t *node)
{
    return list_is_dummy_node(lst, node);
}

// insert node at the end of the list
static inline
void list_insert(list_t *lst, list_node_t *node)
{
    // node becomes the new tail
    node->prev = lst->head.prev;
    node->next = &lst->head;

    // update the previous tail
    lst->head.prev->next = node;

    // update the dummy node
    lst->head.prev = node; 
}

static inline
list_node_t *list_get_head_node(const list_t *lst)
{
    if (list_is_empty(lst))
        return NULL;

    return lst->head.next;
}

static inline
list_node_t *list_get_tail_node(const list_t *lst)
{
    if (list_is_empty(lst))
        return NULL;

    return lst->head.prev;
}
