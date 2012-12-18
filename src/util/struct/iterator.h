/**
 * iterator.h
 * obj
 * October 11, 2012
 * Brandon Surmanski
 */

#ifndef _ITTER_H
#define _ITTER_H

struct iterator;

typedef void *(Iter_func)(void *iterable, struct iterator *i);

typedef struct iterator
{
    Iter_func *inc_func;
    Iter_func *dec_func;
    void *iterable;
    void *value;
} Iterator;


void iterator_init(Iterator *i, void *iterable, Iter_func *inc);
void iterator_setincfunc(Iterator *i, Iter_func *f);
void iterator_setdecfunc(Iterator *i, Iter_func *f);
void iterator_setiterable(Iterator *i, void *v);
void *iterator_next(Iterator *i);
void *iterator_value(Iterator *i);

#endif
