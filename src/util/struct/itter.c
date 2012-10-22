/**
 * itter.c
 * obj
 * October 11, 2012
 * Brandon Surmanski
 */

#include <stddef.h>

#include "itter.h"


void itter_init(Itter *i, void *itterable, void *(*itter_func)(Itter *i, void *a, void *v))
{
    i->itter_func = itter_func;
    i->itterable = itterable;
    i->value = NULL;
}

void *itter_next(Itter *i)
{
    i->value = i->itter_func(i, i->itterable, i->value);
    return i->value;
}

void *itter_value(Itter *i)
{
    return i->value;
}
