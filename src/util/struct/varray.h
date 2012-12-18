/**
 * varray.h
 * microship
 * August 17 2011
 * Brandon Surmanski
 */

#ifndef _VARRAY_H
#define _VARRAY_H

#include <stddef.h>
#include <stdbool.h>

/*structs*/

typedef struct Varray {
    size_t element_size;
    size_t length;
    size_t max;
    void* data;
} Varray;

/*functions*/
void varray_init(Varray *v, size_t element_size);
void varray_finalize(Varray *array, void (*finalizer)(void *v));

bool varray_isempty(Varray *array);
size_t varray_length(Varray *array);
size_t varray_datasize(Varray *array);
void varray_reserve(Varray *array, size_t n);
void varray_add(Varray *array, void* element);
void varray_remove(Varray *array, int i, void (*finalizer)(void *v));
const void const* varray_get(Varray *array, int index);
void varray_tobuffer(Varray *array, void *buf);
const void const* varray_dataptr(Varray *array);
void varray_clear(Varray *array);

#endif
