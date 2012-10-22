/**
 * octree.c
 * obj
 * August 30, 2012
 * Brandon Surmanski
 */

#include <alloca.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "util/math/vec.h"
#include "poctree.h"

#define XMASK 0x01 // x bit mask for octant index
#define YMASK 0x02 // y bit mask
#define ZMASK 0x04 // z bit mask
#define XSHIFT 0 // x bit shift for octant index
#define YSHIFT 1 // y bit shift
#define ZSHIFT 2 // z bit shift

#define ONODE_MAXVAL 16 //max number of values in an octant

typedef struct ovalue_t
{
    void *val;
    vec3 point;
} ovalue_t;

typedef struct osearch_t
{
    int len;
    int max;
    float *point;
    float *distances;
    ovalue_t **list;
} osearch_t;

typedef struct octnode_t
{
    int nvalues;
    float center[3];
    float axis_len;
    struct octnode_t *parent;
    struct octnode_t *children; // 8 children array
    ovalue_t values[ONODE_MAXVAL];
} onode_t;

static void onode_init(onode_t *n, onode_t *parent, float center[3], float axis_len);
static void onode_finalize(onode_t *n, void (*val_finalize(void*)));
static int  point_classify(onode_t *n, float point[3]);
static void onode_split(onode_t *n);
static void onode_insert(onode_t *n, float point[3], void *val);
static void onode_nearestN(onode_t *n, float point[3], osearch_t *list);

static void osearch_init(osearch_t *s, int max, float point[3], float *dist_list, void *list);
static void osearch_insert(osearch_t *list, ovalue_t *val);

// Static functions

/**
 * initialize a new octtree node
 */
static void onode_init(onode_t *n, onode_t *parent, float center[3], float axis_len)
{
    vec3_copy(center, n->center);
    n->nvalues = 0;
    n->axis_len = axis_len;
    n->parent = parent;

    int i;
    n->children = NULL;
}

/**
 * finalizes the octree node, optionally finalizing the saved value
 */
static void onode_finalize(onode_t *n, void (*val_finalize(void*)))
{
    int i;

    //Finalize any and all children nodes
    if(n->children)
    {
        for(i = 0; i < 8; i++)
        {
            onode_finalize(&n->children[i], val_finalize);
        }
    }

    // finalize values in octree if finalizer not null
    if(val_finalize)
    {
        for(i = 0; i < n->nvalues; i++)
        {
            val_finalize(n->values[i].val);
        }
    }
}

/**
 * classifies which octant a point belongs to relative to the node 'n'
 */
static int point_classify(onode_t *n, float point[3])
{
    vec3 delta;
    vec3_sub(n->center, point, delta);
    int ret = ((delta[Z] > 0) << ZSHIFT) |
              ((delta[Y] > 0) << YSHIFT) |
              ((delta[X] > 0) << XSHIFT); //set index

    if(fabs(delta[X]) > n->axis_len ||
        fabs(delta[Y]) > n->axis_len ||
        fabs(delta[Z]) > n->axis_len) 
    {
        ret = -1; //point is not inside node
    }
    return ret;
}

/**
 * used to sort closest octants. a_f and b_f represent distance, and is the sorting variable
 */
static void cond_octant_swap(int *a_n, float *a_f, int *b_n, float *b_f)
{
    int *tmp_n;
    float *tmp_f;

    if(b_f < a_f)
    {
        tmp_n = a_n;
        tmp_f = a_f;
        a_n = b_n;
        a_f = b_f;
        b_n = tmp_n;
        b_f = tmp_f;
    }
}

/**
 * produces an ordered list of children to search for
 */
static void closest_octants(onode_t *n, float point[3], int *idx, float *dist)
{
    int first = point_classify(n, point);
    float center_dist = sqrt(vec3_distsq(point, n->center));

    //octant that contains the query point
    idx[0] = first; 
    dist[0] = 0.0f;

    //octants that are incident to the octant that contains the point
    idx[1] = first ^ XMASK;
    idx[2] = first ^ YMASK;
    idx[3] = first ^ ZMASK;
    dist[1] = fabs(n->center[X] - point[X]);
    dist[2] = fabs(n->center[Y] - point[Y]);
    dist[3] = fabs(n->center[Z] - point[Z]);
    
    // sort incident octants, to trim by distance
    cond_octant_swap(&idx[1], &dist[1], &idx[2], &dist[2]);
    cond_octant_swap(&idx[1], &dist[1], &idx[3], &dist[3]);
    cond_octant_swap(&idx[2], &dist[2], &idx[3], &dist[3]);
    
    //octants that are non-incident, all are equidistant to query point
    idx[4] = first ^ XMASK ^ YMASK;
    idx[5] = first ^ XMASK ^ ZMASK;
    idx[6] = first ^ YMASK ^ ZMASK;
    idx[7] = first ^ XMASK ^ YMASK ^ ZMASK;
    dist[4] = center_dist;
    dist[5] = center_dist;
    dist[6] = center_dist;
    dist[7] = center_dist;
}

/**
 * set vec3 'ret' to the value of the node 'n's child at child_index.
 * for example, if child_index is 7 (b111), then the child octant center
 * would be at the parent nodes center + parent nodes axis length / 2
 */
static void child_center(onode_t *n, int child_index, vec3 ret)
{
    bool xflag = ((child_index & 1) != 0);
    bool yflag = ((child_index & 2) != 0);
    bool zflag = ((child_index & 4) != 0);

    // bit hack to conditionally negate without branching
    ret[X] = (xflag ^ (xflag - 1)) * (n->center[X] - n->axis_len);
    ret[Y] = (yflag ^ (yflag - 1)) * (n->center[Y] - n->axis_len);
    ret[Z] = (zflag ^ (zflag - 1)) * (n->center[Z] - n->axis_len);
}

/**
 * splits an octant, moving it's values to newly created children
 */
static void onode_split(onode_t *n)
{
    if(!n->children) // node must not already be split
    {
        n->children = malloc(sizeof(onode_t) * 8);

        vec3 newcenter;
        vec3 axis_offset;

        float child_alen = n->axis_len / 2.0f;

        int i;
        for(i = 0; i < 8; i++) //create new children
        {
            child_center(n, i, newcenter);
            onode_init(&n->children[i], n, newcenter, child_alen);
        }

        //NOTE: may crash if all points are equal (infinite depth)
        //may fix by not allowing duplicates, or if allowing duplicates,
        //then use a linked list to allow multiple values associated
        for(i = 0; i < n->nvalues; i++) //distribute values among children
        {
            int node_idx = point_classify(n, n->values[i].point);
            onode_insert(&n->children[node_idx], n->values[i].point, n->values[i].val);
        }
    }
}

/**
 * inserts a value into the octree node (recursively until a leaf is found)
 */
static void onode_insert(onode_t *n, float point[3], void *val)
{
    if(n->nvalues >= 8)
    {
        onode_split(n);
        n->nvalues = 0;
    }

    if(n->children)
    {
        int index = point_classify(n, point);
        onode_insert(&n->children[index], point, val);
    } else 
    {
        ovalue_t *newvalue = &n->values[n->nvalues];
        vec3_copy(point, newvalue->point);
        newvalue->val = val;
        n->nvalues++;
    }
}

/**
 * will insert a value into the search list (if it is of better fitness
 * than the worst currently in the list, or the list is not full).
 */
static void osearch_insert(osearch_t *list, ovalue_t *val)
{
    assert(list->max > 0);

    float val_dist = vec3_distsq(list->point, val->point);
    if(list->len < list->max || val_dist < list->distances[list->len - 1])
    {
        if(list->len < list->max)
        {
            list->len++;
        }

        int i;
        for(i = list->len - 1; i > 0 && val_dist < list->distances[i-1]; i--)
        {
            list->distances[i] = list->distances[i-1];
            list->list[i] = list->list[i-1];
        }

        list->distances[i] = val_dist;
        list->list[i] = val;
    }
}

/**
 * finds the nearest N values within this octant (recursively)
 */
static void onode_nearestN(onode_t *n, float point[3], osearch_t *search)
{
    if(n->children)
    {
        float dists[8];
        int idx[8];
        closest_octants(n, point, idx, dists);

        int i;
        for(i = 0; i < 8 && (!search->len || dists[i] < search->distances[i]); i++)
        {
            onode_nearestN(&n->children[idx[i]], point, search);
        }
    } else
    {
        int i;
        for(i = 0; i < n->nvalues; i++)
        {
            osearch_insert(search, &n->values[i]);
        }
    }
}

/*
 *
 * Global functions
 *
 */

/**
 * initialize the octree to default value
 */
void octree_init(octree_t *t)
{
    assert(t);

    t->root = malloc(sizeof(onode_t));
    vec3 center = {0.0f, 0.0f, 0.0f};
    onode_init(t->root, NULL, center, 2.0f); 
}

/**
 * finalize the octree, and optionally finalize the values found in the octree
 */
void octree_finalize(octree_t *t, void (*val_finalize(void*)))
{
    assert(t);
    assert(t->root);

    onode_finalize(t->root, val_finalize);
    free(t->root);
    t->root = NULL;
}


/**
 * insert a value into the octree.
 */
void octree_insert(octree_t *t, float loc[3], void *val)
{
    if(point_classify(t->root, loc) == -1)
    {
        //TODO: allow the octtree to expand
        assert(false && "Octree expansion not yet impl :(\n");
    }

    onode_insert(t->root, loc, val);
}

/**
 * initialize private searching structure 'osearch'
 */
static void osearch_init(osearch_t *s, int max, float point[3], float *dist_list, void *list)
{
    s->len = 0;
    s->max = max;
    s->point = point;
    s->distances = dist_list;
    s->list = list;
}

/**
 * returns the nearest point and corrisponding value to the query point
 */
void *octree_nearest(octree_t *t, float loc[3], float nearest[3])
{
    assert(t);
    assert(t->root);

    osearch_t search;
    void *search_list = alloca(sizeof(ovalue_t*));
    float *distances = alloca(sizeof(float));
    osearch_init(&search, 1, loc, distances, search_list);
    
    onode_nearestN(t->root, loc, &search);

    void *ret = NULL;
    
    if(search.len > 0)
    {
        ovalue_t *near = search.list[0];
        vec3_copy(near->val, nearest);
        ret = near->val;
    }
    return ret;
}

/**
 * returns the N nearest points and corrisponding value to the query point
 */
int octree_nearestN(octree_t *t, float loc[3], int n, void **buf, float *nearest[3])
{
    assert(t);
    assert(t->root);

    osearch_t search;
    void *search_list = alloca(sizeof(ovalue_t*) * n);
    float *distances = alloca(sizeof(float) * n);
    osearch_init(&search, n, loc, distances, search_list);

    onode_nearestN(t->root, loc, &search);

    int i;
    for(i = 0; i < search.len ; i++)
    {
        buf[i] = search.list[i]->val;
        memcpy(nearest[i], search.list[i]->point, sizeof(float) * 3);
    }

    return search.len;
}
