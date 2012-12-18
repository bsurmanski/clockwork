/**
 * varray.c
 * microship
 * August 17 2011
 * Brandon Surmanski
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "varray.h"

#define OFFSET(a,sz,b) ((void*)(((char*)(a)) + ((sz) * (b))))

static void grow(Varray *array);
static void shrink(Varray *array);
static int find_element(Varray *array, void* element);

static void grow(Varray *array)
{
    array->max = ceil(array->max * 1.6f);
    array->data = realloc(array->data, array->max * array->element_size);
}

static void shrink(Varray *array)
{
    array->max = (int) ceil(array->max / 2.0);
    array->data = realloc(array->data, array->max * array->element_size);
}

/*
static int find_element(Varray *array, void* element)
{
    int ret = -1;
    int i;
    for (i = 0; i < array->length; i++){
        if (memcmp(varray_get(array, i), element, array->element_size) == 0){
            ret = i;
            break;
        }
    }
    return ret;
}*/

void varray_init(Varray *a, size_t element_size)
{
    int max = 10;
    a->element_size = element_size;
    a->length = 0;
    a->max = max;
    a->data = malloc(max*element_size);
}

void varray_finalize(Varray *array, void (*finalizer)(void *v))
{
    if(finalizer)
    {
        int i;
        for(i = 0; i < array->length; i++)
        {
            finalizer(OFFSET(array, array->element_size, i));
        }
    }

    free(array->data);
}

bool varray_isempty(Varray *array)
{
    return array->length == 0;
}

size_t varray_length(Varray *array)
{
    return array->length;
}

size_t varray_datasize(Varray *array)
{
    return array->length * array->element_size;
}

void varray_reserve(Varray *array, size_t n)
{
    if(array->length + n < array->max)
    {
        array->max = array->length + n;
        array->data = realloc(array->data, array->max * array->element_size);
    }
}

void varray_add(Varray *array, void* element)
{
    assert(element);

    if (array->length + 1 == array->max)
    {
        grow(array);
    }

    memcpy(OFFSET(array->data, array->length, array->element_size), element, array->element_size);
    ++(array->length);
}

void varray_remove(Varray *array, int i, void (*finalizer)(void *v))
{
    void* rem_element = OFFSET(array, array->element_size, i);
    finalizer(rem_element);
    memcpy(rem_element, OFFSET(rem_element, array->element_size, 1), array->element_size * (array->length - i));
    --(array->length);
    if (array->max > 10 && array->length < array->max/3)
        shrink(array);
}

const void const* varray_get(Varray *array, int index)
{
    return OFFSET(array->data, array->element_size, index);
}

void varray_tobuffer(Varray *array, void *buf)
{
    memcpy(buf, array->data, array->length * array->element_size);
}

const void const* varray_dataptr(Varray *array)
{
    return array->data;
}

void varray_clear(Varray *array)
{
    array->max = 10;
    array->length = 0;
    array->data = realloc(array->data, array->max * array->element_size);
}
