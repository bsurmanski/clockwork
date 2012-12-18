/**
 * kdtree.h
 * voronoi
 * May 9, 2012
 * Brandon Surmanski
 */

#ifndef _KDTREE_H
#define _KDTREE_H

#include <assert.h>

#include "iterator.h"

typedef void* kditerator;

typedef struct kdtree {
    int dimensions;
    struct kdnode *root; 
} kdtree;

void kdtree_init(kdtree *k, int dimensions);
void kdtree_finalize(kdtree *k, void (*val_finalize(void*)));
void kdtree_insert(kdtree *k, float point[], void *val);
void *kdtree_closest(kdtree *k, float point[], float closest[]);
int kdtree_closestN(kdtree *k, float point[], int n, void **buf, float *closest[]);
void kdtree_removeClosest(kdtree *k, float point[], void (*val_finalize)(void*));

//itteration and node functions
void kdtree_iter_init(kdtree *k, Iterator *i);
void *kdtree_iter_value(Iterator *i);
float const *kdtree_iter_position(Iterator *i);
//TODO: a balance function
//
//TODO: replace other function with iterable
void *kdtree_closest2(kdtree *k, float point[], Iterator *it);
void *kdtree_nextclosest(kdtree *k, Iterator *it);

#endif
