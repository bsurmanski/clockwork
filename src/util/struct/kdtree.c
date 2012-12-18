/**
 * kdtree.c
 * voronoi
 * May 9, 2012
 * Brandon Surmanski
 */

#include <alloca.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <alloca.h>

#include "kdtree.h"

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define MALLOC_THRESH 127 //nelements until malloc is used instead of alloca in tmp alloc for search


typedef struct kdValue 
{
    float distsq;
    struct kdnode *node;
} kdValue;

typedef struct kdSearch
{
    int found;
    int max;
    float *point;
    kdValue *list;
} kdSearch;

typedef struct kdnode {
    uint16_t dim;
    uint16_t axis;
    float *pos;
    void *val;
    struct kdnode *parent;
    struct kdnode *children[2]; 
} kdnode;

static float distanceSq(float point1[], float point2[], int dim);
static int knode_nextAxis(int axis, int dim);
static int knode_otherSide(int side);
static void kdnode_init(kdnode *k, kdnode *parent, float point[], int dim, void *val);
static void kdnode_finalize(kdnode *n, void (*val_finalize(void *)));
static void kdnode_insert(kdnode *n, float point[], void *val);
static void kdnode_closest(kdnode *n, float point[], kdValue *val);
static bool trySearchInsert(kdnode *n, kdSearch *s);
static void kdnode_closestN(kdnode *n, kdSearch *s);
static kdnode *kdnode_next(kdnode *n, kdnode *prev);

/**
 * will find the squared distance between two points. the squared distance is useful
 * to reduce the number of sqrt operation necesarry.
 */ 
static float distanceSq(float point1[], float point2[], int dim)
{
    float sqsum = 0.0f;
    int i;
    for(i = 0; i < dim; i++)
    {
        sqsum += (point1[i] - point2[i]) * (point1[i] - point2[i]);
    }
    return sqsum;
}

/**
 * gets the axis of children nodes, given the axis of the current node.
 */ 
static int knode_nextAxis(int axis, int dim)
{
    ++axis;
    if(axis >= dim)
    {
        axis = 0;
    }
    return axis;
}

/**
 * returns the opposite child side, give a side. 
 */ 
static int knode_otherSide(int side)
{
    return (side == 0 ? 1 : 0);
}

/**
 * initializer for a node
 */ 
static void kdnode_init(kdnode *n, kdnode *parent, float point[], int dim, void *val)
{
    n->parent = parent; 
    n->dim = dim;
    
    if(parent)
    {
        n->axis = knode_nextAxis(parent->axis, parent->dim);
    } else 
    {
        n->axis = 0;
    }

    n->pos = malloc(sizeof(float) * dim); 
    memcpy(n->pos, point, sizeof(float) * dim); 
    n->val = val;
    n->children[0] = 0;
    n->children[1] = 0;
}

/**
 * will finalize a node freeing any dependant memory, but will not free the node itself
 */
static void kdnode_finalize(kdnode *n, void (*val_finalize(void *)))
{        
    if(n){
        if(n->val && val_finalize){
            val_finalize(n->val);
        }
        kdnode_finalize(n->children[0], val_finalize);
        kdnode_finalize(n->children[1], val_finalize);
        free(n->children[0]);
        free(n->children[1]);
    }
}

/**
 * will recursively search for a spot for a node, and add it to the tree
 */ 
static void kdnode_insert(kdnode *n, float point[], void *val)
{
    int side = (point[n->axis] > n->pos[n->axis] ? 1 : 0);
    if(n->children[side]){
        kdnode_insert(n->children[side], point, val);
    } else {
        n->children[side] = malloc(sizeof(kdnode));
        kdnode_init(n->children[side], n, point, n->dim, val);
    }
}

/**
 * will find the closest distance to a supplied point. the distance is passed around as
 * cdistsq, the squared of the distance, to reduce the number of expensive sqrt opperations
 */
static void kdnode_closest(kdnode *n, float point[], kdValue *val)
{
    int side = (point[n->axis] > n->pos[n->axis] ? 1 : 0);
    float node_dsq = distanceSq(point, n->pos, n->dim); 
    if(node_dsq < val->distsq || val->node == NULL)
    {
        val->distsq = node_dsq;
        val->node = n;
    }

    if(n->children[side]){
            kdnode_closest(n->children[side], point, val);
    }

    side = knode_otherSide(side);
    if(n->children[side]){
        float childAxisDist = fabs(point[n->axis] - n->pos[n->axis]);
        if(childAxisDist <= val->distsq){
            kdnode_closest(n->children[side], point, val);
        }
    }
}

static bool trySearchInsert(kdnode *n, kdSearch *s)
{
    bool ret = false;
    float node_dsq = distanceSq(s->point, n->pos, n->dim); 

    if(!s->found) //list is empty
    {
        s->list[0].distsq = node_dsq;
        s->list[0].node = n;
        s->found++;
    } else if(s->found < s->max || node_dsq < s->list[s->found-1].distsq)
    {
        int i;
        for (i = s->found; i >= 0; i--)
        {
            if(i == 0 || node_dsq > s->list[i-1].distsq)
            {
                if(s->found < s->max)
                {
                    s->found++; 
                }
                //shift over current values, to make room for new node
                memmove(&s->list[i+1], &s->list[i], (s->found - i) * sizeof(kdValue));

                s->list[i].distsq = node_dsq;
                s->list[i].node = n;
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static void kdnode_closestN(kdnode *n, kdSearch *s)
{
    int side = (s->point[n->axis] > n->pos[n->axis] ? 1 : 0);

    trySearchInsert(n, s);

    if(n->children[side]){
            kdnode_closestN(n->children[side], s);
    }

    side = knode_otherSide(side);
    if(n->children[side]){
        float childAxisDist = fabs(s->point[n->axis] - n->pos[n->axis]);
        if(s->found < s->max || childAxisDist <= s->list[s->max-1].distsq){
            kdnode_closestN(n->children[side], s);
        }
    }
}

/**
 * initializes the tree
 */ 
void kdtree_init(kdtree *k, int dimensions)
{
    k->dimensions = dimensions;
    k->root = 0;
}

/**
 * will finalize a kdtree, freeing all dependant memory such as all the nodes and the node
 * values are finalized with the method passed as 'val_finalize'. This method will not free
 * the allocation for the actial kdtree, and must be done externaly
 */
void kdtree_finalize(kdtree *k, void (*val_finalize(void*)))
{
   kdnode_finalize(k->root, val_finalize);
   free(k->root);
   k->root = 0;
}

/**
 * will find the proper position for a new node and add it to the tree
 */ 
void kdtree_insert(kdtree *k, float point[], void *val)
{
    if(!k->root){
        k->root = malloc(sizeof(kdnode));
        kdnode_init(k->root, NULL, point, k->dimensions, val);
    } else {
        kdnode_insert(k->root, point, val);
    }
}

/**
 * will return the closest point to a supplied x/y pair. If there are no points in the tree, 
 * return NULL
 */
void *kdtree_closest(kdtree *k, float point[], float closest[])
{
    if(!k || !k->root)
        return NULL;

    kdValue val = {-1, NULL};
    float distancesq = distanceSq(point, k->root->pos, k->dimensions);
    kdnode_closest(k->root, point, &val);
    if(closest)
    {
        memcpy(closest, val.node->pos, sizeof(float) * k->dimensions);
    }
    return val.node->val;
}

int kdtree_closestN(kdtree *k, float point[], int n, void **buf, float *closest[])
{
    kdSearch search;
    search.point = point;
    search.max = n;
    search.found = 0;

    if(n <= 0)
    {
        return 0;
    } else if (n < MALLOC_THRESH)
    {
        search.list = alloca(sizeof(kdValue) * n); //stack alloc quickly for small values
    } else 
    {
        search.list = malloc(sizeof(kdValue) * n); //rare malloc for large n values
    }

    kdnode_closestN(k->root, &search);

    int i;
    for(i = 0; i < search.found; i++)
    {
        if(buf)
        {
            buf[i] = search.list[i].node->val; 
        }

        if(closest)
        {
            closest[i] = search.list[i].node->pos;
        }
    }

    if(n >= MALLOC_THRESH)
    {
        free(search.list);
    }

    return search.found;
}

void kdtree_removeClosest(kdtree *k, float point[], void (*val_finalize)(void*))
{
    kdValue val = {-1, NULL};
    kdnode_closest(k->root, point, &val);
    if(val.node)
    {
        //remove found node from parent
        if(val.node->parent)
        {
            if(val.node->parent->children[0] == val.node)
            {
                val.node->parent->children[0] = NULL; 
            } else if(val.node->parent->children[1] == val.node)
            {
                val.node->parent->children[1] = NULL; 
            }
        } else if(k->root == val.node) //remove if root
        {
            k->root = NULL;
        }
        val.node->parent = NULL;
    }
    //itterate over children an re-add to kdtree
    Iterator iter;
    kdtree_iter_init(k, &iter);
    iter.value = val.node;

    kdnode *iternode;
    while((iternode = iterator_next(&iter)))
    {
        kdtree_insert(k, iternode->pos, iternode->val);
    }

    //free old node value
    if(val_finalize)
    {
        val_finalize(val.node->val);
    }
}


/**
 * itterator next function
 */
static void *kdtree_iter_next(void *k, Iterator *i)
{
    kdnode *ret = NULL;
    if(!i->value)
    {
       ret = ((kdtree*)k)->root; 
    } else
    {
        ret = kdnode_next((kdnode*)i->value, NULL);    
    }
    return ret;
}

static kdnode *kdnode_next(kdnode *n, kdnode *prev)
{
    kdnode *ret = NULL;
    if(n)
    {
        if(!prev && n->children[0])
        {
            return n->children[0]; 
        } else if(prev != n->children[1] && n->children[1])
        {
            return n->children[1]; 
        } else //already traversed children, traverse upward
        {
            ret = kdnode_next(n->parent, n);
        }
    }
    return ret;
}

void kdtree_iter_init(kdtree *k, Iterator *i)
{
    assert(k);
    assert(i);

    iterator_init(i, k, kdtree_iter_next);
    i->value = k->root;
}

void *kdtree_iter_value(Iterator *i)
{
    return ((struct kdnode*)i->value)->val;
}

float const *kdtree_iter_position(Iterator *i)
{
    return ((struct kdnode*)i->value)->pos;
}

//TODO
//

/**
 * will find the closest distance to a supplied point. the distance is passed around as
 * cdistsq, the squared of the distance, to reduce the number of expensive sqrt opperations
 */
static void kdnode_closest2(kdnode *n, float point[], float *distsq, Iterator *it)
{
    int side = (point[n->axis] > n->pos[n->axis] ? 1 : 0);
    float node_dsq = distanceSq(point, n->pos, n->dim); 
    if(node_dsq < *distsq || it->iterable == NULL)
    {
        *distsq = node_dsq;
        it->iterable = n;
    }

    if(n->children[side]){
            kdnode_closest2(n->children[side], point, distsq, it);
    }

    side = knode_otherSide(side);
    if(n->children[side]){
        float childAxisDist = fabs(point[n->axis] - n->pos[n->axis]);
        if(childAxisDist < *distsq){
            kdnode_closest2(n->children[side], point, distsq, it);
        }
    }
}

void *kdtree_closest2(kdtree *k, float point[], Iterator *it)
{
    if(!k || !k->root)
        return NULL;

    //guarentee that 'it' is a valid value 
    if(!it)
    {
        it = alloca(sizeof(Iterator));
    }
    it->iterable = k->root;
    it->value = k->root->val;
    //TODO: double check func below, and dec_func
    //it->inc_func = kdtree_nextclosest;

    float distancesq = distanceSq(point, k->root->pos, k->dimensions);
    kdnode_closest2(k->root, point, &distancesq, it);
    it->value = ((kdnode*)it->iterable)->val;
    return it->value; //TODO
}

void *kdtree_nextclosest(kdtree *k, Iterator *it);
