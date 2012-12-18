/**
 * list.c
 * clockwork
 * December 02, 2012
 * Brandon Surmanski
 *
 * linked List implementation
 */

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define DEFAULT_MAX 10

struct list_ent
{
    struct list_ent *next;
    struct list_ent *prev;
    char data[];
};

static size_t entry_size(List *l);
static bool isvalid_entry(List *l, struct list_ent *e);
static void invalidate_entry(List *l, struct list_ent *e);
static void invalidate_slice(List *l, struct list_ent *start, int n);
static void resize(List *l, size_t n);
static struct list_ent *findinvalid(List *l);

static size_t entry_size(List *l)
{
    return l->element_size + sizeof(struct list_ent);
}

/**
 * an invalid entry is one where there is no next, previous, or data
 */
static bool isvalid_entry(List *l, struct list_ent *e)
{
    return e->next || e->prev || l->first == e;
}

/**
 * sets an entry to an invalid state
 */
static void invalidate_entry(List *l, struct list_ent *e)
{
    memset(e, 0x00, entry_size(l)); //TODO: setting data unnecessary
}

/**
 * sets a slice of entries to an invalid state
 */
static void invalidate_slice(List *l, struct list_ent *start, int n)
{
    memset(start, 0x00, entry_size(l) * n); //TODO: setting data unnecessary
}


static void resize(List *l, size_t n)
{
    if(n < l->max)
    {
        assert(false && "List shrink not yet impl");
        //TODO: shrink
    }

    int old_max = l->max;
    l->max = n; 
    l->pool = realloc(l->pool, l->max * entry_size(l));
    invalidate_slice(l, &l->pool[old_max], l->max - old_max); //invalidate all new entries
}

/**
 * will find an invalid List entry for reuse as a future list node
 */
static struct list_ent *findinvalid(List *l)
{
    struct list_ent *found = NULL;
    int search_start = l->last_avail_index;
    if(l->length < l->max)
    {
        int i;
        for(i = search_start; i < l->max && !found; i++) //search above last index
        {
            if(!isvalid_entry(l, &l->pool[i]))
            {
                l->last_avail_index = i;
                found = &l->pool[i];
            }
        }

        for(i = 0; i < search_start && !found; i++) //search below last index
        {
            if(!isvalid_entry(l, &l->pool[i]))
            {
                l->last_avail_index = i;
                found = &l->pool[i];
            }
        }
    }
    return found;
}

void list_init(List *l, size_t element_size)
{
    l->element_size = element_size;
    l->length = 0;
    l->max = DEFAULT_MAX;
    l->last_avail_index = 0;
    l->pool = calloc(l->max, entry_size(l)); //zeroed out members are unused nodes
    l->first = NULL;
    l->last = NULL;
}

void list_finalize(List *l, void (*finalizer)(void*v))
{
    if(l->first && finalizer)
    {
        struct list_ent *iter = l->first; 
        while(iter)
        {
            finalizer(iter->data); 
            iter = iter->next;
        }
    }
    free(l->pool);
}

bool list_isempty(List *l)
{
    return l->length == 0;
}

size_t list_length(List *l)
{
    return l->length;
}

size_t list_datasize(List *l)
{
    return l->length * l->element_size;
}

/**
 * will make sure that the list has space for at least 'n' objects
 */
void list_reserve(List *l, size_t n)
{
    if(l->length + n >= l->max)
    {
        resize(l, l->length + n);
    }
}

void list_addfront(List *l, void *element)
{
    if(l->length >= l->max)
    {
        resize(l, (size_t)(l->length * 1.6f));
    }

    struct list_ent *new_front = findinvalid(l);
    assert(new_front);

    if(l->length > 0)
    {
        l->first->prev = new_front;
    } else 
    {
        l->last = new_front; 
    }

    new_front->next = l->first;
    new_front->prev = NULL;
    l->first = new_front;
    memcpy(new_front->data, element, l->element_size);
    l->length++;
}

void list_addback(List *l, void *element)
{
    if(l->length == l->max)
    {
        resize(l, (size_t)(l->length * 1.6f));
    }

    struct list_ent *new_back = findinvalid(l);
    assert(new_back);

    if(l->length > 0)
    {
        l->last->next = new_back;
    } else 
    {
        l->first = new_back;
    }

    new_back->next = NULL;
    new_back->prev = l->last;
    l->last = new_back;
    memcpy(new_back->data, element, l->element_size);
    l->length++;
}

void list_addafter(List *l, Iterator *i, void *element)
{
    if(l->length == l->max)
    {
        resize(l, (size_t)(l->length * 1.6f));
    }
    struct list_ent *new_ent = findinvalid(l);
    assert(new_ent);
    
    struct list_ent *ent = i->iterable;
    new_ent->next = ent->next;
    new_ent->prev = ent;
    memcpy(new_ent->data, element, l->element_size);
    ent->next = new_ent;
}

void list_addbefore(List *l, Iterator *i, void *element)
{
    if(l->length == l->max)
    {
        resize(l, (size_t)(l->length * 1.6f));
    }
    struct list_ent *new_ent = findinvalid(l);
    assert(new_ent);
    
    struct list_ent *ent = i->iterable;
    new_ent->prev = ent->prev;
    new_ent->next = ent;
    memcpy(new_ent->data, element, l->element_size);
    ent->prev = new_ent;
}

void list_remove(List *l, Iterator *i)
{
    struct list_ent *ent = i->iterable;
    struct list_ent *next = ent->next;
    struct list_ent *prev = ent->prev;

    if(next)
    {
        next->prev = prev;
    } else
    {
        l->last = prev;
    }

    if(prev)
    {
        prev->next = next;
    } else
    {
        l->first = next;
    }

    invalidate_entry(l, ent);
    l->length--;
}

void *list_get(List *l, Iterator *i)
{
    struct list_ent *ent = i->iterable;
    return ent->data;
}

void *list_first(List *l, Iterator *it)
{
    void *ret = NULL;

    if(l->first)
    {
        if(it)
        {
            it->iterable = l->first;
            it->value = &l->first->data;
            it->inc_func = list_next;
            it->dec_func = list_prev;
        }
        ret = &l->first->data;
    } else if (it)
    {
        it->iterable = NULL;
        it->value = NULL;
        it->inc_func = NULL;
        it->dec_func = NULL;
    }

    return ret;
}

void *list_last(List *l, Iterator *it)
{
    void *ret = NULL;

    if(l->last)
    {
        if(it)
        {
            it->iterable = l->last;
            it->value = &l->last->data;
            it->inc_func = list_next;
            it->dec_func = list_prev;
        }
        ret = &l->last->data;
    } else if (it)
    {
        it->iterable = NULL;
        it->value = NULL;
        it->inc_func = NULL;
        it->dec_func = NULL;
    }

    return ret;
}

void *list_next(void *lst, Iterator *i)
{
    void *ret = NULL;
    struct list_ent *ent = i->iterable; 
    ent = ent->next;
    if(ent)
    {
        i->iterable = ent;
        i->value = &ent->data;
        ret = &ent->data;
    } else 
    {
        i->value = NULL;
    }
    return ret;
}

void *list_prev(void *l, Iterator *i)
{
    struct list_ent *ent = i->iterable; 
    ent = ent->prev;
    i->iterable = ent;
    i->value = ent->data;
    return ent->data;
}

void list_movebefore(List *l, Iterator *before, Iterator *from)
{
    struct list_ent *before_ent = before->iterable;
    struct list_ent *ent = from->iterable;

    //knit up neighbors of 'ent' before moving
    if(ent->next)
    {
        ent->next->prev = ent->prev; 
    } else 
    {
        l->last = ent->prev;
    }

    if(ent->prev)
    {
        ent->prev->next = ent->next;
    } else 
    {
        l->first = ent->next;
    }

    //splice in ent where it needs to go
    ent->prev = before_ent->prev;
    ent->next = before_ent;

    // knit previous to ent, or make it the front
    if(before_ent->prev)
    {
        before_ent->prev->next = ent;
    } else 
    {
        l->first = ent;
    }

    // knit up loose ent of where we are inserting before
    before_ent->prev = ent;
}

void list_moveafter(List *l, Iterator *after, Iterator *from)
{
    struct list_ent *after_ent = after->iterable;
    struct list_ent *ent = from->iterable;

    // knit neighbors of 'ent' before moving
    if(ent->next)
    {
        ent->next->prev = ent->prev; 
    } else 
    {
        l->last = ent->prev;
    }

    if(ent->prev)
    {
        ent->prev->next = ent->next;
    } else 
    {
        l->first = ent->next;
    }

    //splice in ent where it needs to go
    ent->prev = after_ent;
    ent->next = after_ent->next;

    // knit next to ent, or make it the back
    if(after_ent->next)
    {
        after_ent->next->prev = ent;
    } else 
    {
        l->first = ent;
    }

    //knit up loose connection of neighbor
    after_ent->next = ent;
}

void list_tobuffer(List *l, void *b)
{
    char *buf = b;
    Iterator it;
    list_first(l, &it);

    int i;
    for(i = 0; i < l->length; i++)
    {
        assert(it.value && "corrupted list, length misrepresents list");
        memcpy(&buf[i * l->element_size], iterator_value(&it), l->element_size);
        iterator_next(&it);
    }
}

void list_clear(List *l, void (*finalizer)(void *v))
{
    l->length = 0;
    l->max = DEFAULT_MAX;
    l->last_avail_index = 0;
    l->pool = realloc(l->pool, l->max * entry_size(l));
    l->first = NULL;
    l->last = NULL;
}
