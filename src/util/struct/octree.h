/**
 * octree.h
 * obj
 * August 30, 2012
 * Brandon Surmanski
 */

#ifndef _OCTREE_H
#define _OCTREE_H

typedef struct octree_t
{
    struct octnode_t *root;
} octree_t;

void octree_init(octree_t *t);
void octree_finalize(octree_t *t, void (*val_finalize(void*)));
void octree_insert(octree_t *t, float point[3], void *val);
void *octree_nearest(octree_t *t, float loc[3], float nearest[3]);
int octree_nearestN(octree_t *t, float loc[3], int n, void **buf, float *nearest[3]);

#endif
