/**
 * voctree.h
 * obj
 * October 05, 2012
 * Brandon Surmanski
 */

#ifndef _VOCTREE_H
#define _VOCTREE_H

struct svo_block;

typedef struct Voctree 
{
    int nblocks;
    struct svo_block *blocks;
} Voctree;

void voctree_init(Voctree *v, int x, int y, int z);
void voctree_finalize(Voctree *v);

#endif
