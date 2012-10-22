/**
 * itter.h
 * obj
 * October 11, 2012
 * Brandon Surmanski
 */

#ifndef _ITTER_H
#define _ITTER_H

typedef struct itter
{
    void *(*itter_func)(struct itter*, void *, void*);
    void *itterable;
    void *value;
} Itter;

void itter_init(Itter *i, void *itterable, void *(*itter_func)(Itter *i, void *a, void *v));
void *itter_next(Itter *i);
void *itter_value(Itter *i);

#endif
