/**
 * list.h
 * clockwork
 * December 02, 2012
 * Brandon Surmanski
 *
 * linked list implementation
 */

#ifndef _LIST_H
#define _LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "iterator.h"

struct list_ent;

typedef struct List
{
    size_t element_size;
    size_t length;
    size_t max;
    unsigned int last_avail_index;
    struct list_ent *pool; ///< pool of entries, raw memory of storage
    struct list_ent *first;///< first entry of list
    struct list_ent *last; ///< last entry of list
} List;

void list_init(List *l, size_t element_size);
void list_finalize(List *l, void (*finalizer)(void*v));

bool list_isempty(List *l);
size_t list_length(List *l);
size_t list_datasize(List *l);
void list_reserve(List *l, size_t n);
void list_addfront(List *l, void *element);
void list_addback(List *l, void *element);
void list_addbefore(List *l, Iterator *i, void *element);
void list_addafter(List *l, Iterator *i, void *element);
void list_remove(List *l, Iterator *i);
void *list_get(List *l, Iterator *i);
void *list_first(List *l, Iterator *ret);
void *list_last(List *l, Iterator *ret);
void *list_next(void *l, Iterator *i); //serves as iterator function
void *list_prev(void *l, Iterator *i); //serves as iterator function
void list_movebefore(List *l, Iterator *before, Iterator *from);
void list_moveafter(List *l, Iterator *after, Iterator *from);
void list_tobuffer(List *l, void *buf);
void list_clear(List *l, void (*finalizer)(void *v));

#endif
