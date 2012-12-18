/**
 * iterator.c
 * obj
 * October 11, 2012
 * Brandon Surmanski
 */

#include <stddef.h>

#include "iterator.h"


void iterator_init(Iterator *i, void *iterable, Iter_func *inc)
{
    i->inc_func = inc;
    i->dec_func = NULL;
    i->iterable = iterable;
    i->value = NULL;
}

void iterator_setincfunc(Iterator *i, Iter_func *f)
{
    i->inc_func = f;
}
void iterator_setdecfunc(Iterator *i, Iter_func *f)
{
    i->dec_func = f;
}

void iterator_setiterable(Iterator *i, void *v)
{
    i->iterable = v;
}

void *iterator_next(Iterator *i)
{
    i->value = i->inc_func(i->iterable, i);
    return i->value;
}

void *iterator_value(Iterator *i)
{
    return i->value;
}

