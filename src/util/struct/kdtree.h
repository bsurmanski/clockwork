/**
 * kdtree.h
 * voronoi
 * May 9, 2012
 * Brandon Surmanski
 */

#ifndef _KDTREE_H
#define _KDTREE_H

#include <assert.h>

#include "itter.h"

typedef void* kdittr;

typedef struct kdtree {
    int dimensions;
    struct kdnode *root; 
} kdtree;

void kdtree_init(kdtree *k, int dimensions);
void kdtree_finalize(kdtree *k, void (*val_finalize(void*)));
void kdtree_insert(kdtree *k, float point[], void *val);
void *kdtree_closest(kdtree *k, float point[], float closest[]);
int kdtree_closestN(kdtree *k, float point[], int n, void **buf, float *closest[]);
//TODO:bool kdtree_remove(kdtree *k, float point[]);

//itteration and node functions
void kdtree_itter_init(kdtree *k, Itter *i);
void *kdtree_itter_value(Itter *i);
float const *kdtree_itter_position(Itter *i);
//TODO: a balance function

#endif
