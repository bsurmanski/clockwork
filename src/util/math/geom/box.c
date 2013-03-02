/**
 * box.c
 * clockwork
 * January 26, 2013
 * Brandon Surmanski
 */

#include <string.h>
#include <math.h>

#include "box.h"

/* **********
 * Shared
 * **********/

/**
 * minimum translation distance to move A and B out from colliding. 
 * this is for a single dimension (and translation is relative to A)
 * assumes that A and B collide
 */
static float mtd(float apos, float adim, float bpos, float bdim)
{
    float ret;
    float d1 = apos - (bpos + bdim/2.0f); // distance of A's 'left' point to B's center
    float d2 = (apos + adim) - (bpos + bdim/2.0f); //distance of A's 'right' point to B's center
    if(fabs(d1) > fabs(d2))
    {
        ret = (bpos + bdim) - apos;
    } else
    {
        ret = bpos - (apos + adim);
    }
    return ret;
}

/**
 * checks whether a single dimenison overlaps
 * if all dimension of a box overlap, then the box is colliding
 */
static int overlaps(float apos, float adim, float bpos, float bdim)
{
    return ((apos + adim > bpos) && (apos + adim < bpos + bdim)) || 
        ((bpos + bdim > apos) && (bpos + bdim < apos + adim));
}

/* **********
 * box2
 * **********/

void box2_init(Box2 *b, float pos[2], float dim[2])
{
    box2_setpositionv(b, pos);
    box2_setdimensionv(b, dim);
}

void box2_setcenterv(Box2 *b, float p[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        b->pos[i] = p[i] - b->dim[i]/2.0f;
    }
}

void box2_setpositionv(Box2 *b, float p[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        b->pos[i] = p[i];
    }
}

void box2_setdimensionv(Box2 *b, float dims[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        b->dim[i] = dims[i];
    }
}

void box2_movev(Box2 *b, float dv[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        b->pos[i] += dv[i];
    }
}

void box2_scalev(Box2 *b, float scale[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        b->dim[i] *= scale[i];
    }
}

bool box2_collides(Box2 *a, Box2 *b)
{
    bool ret = true;
    
    int i;
    for(i = 0; i < 2; i++)
    {
        if(!overlaps(a->pos[i], a->dim[i], b->pos[i], b->dim[i]))
        {
            ret = false;
            break;
        }
    }
    return ret;
}

void box2_mtv(Box2 *a, Box2 *b, float dv[2])
{
    int i;
    for(i = 0; i < 2; i++)
    {
        dv[i] = mtd(a->pos[i], a->dim[i], b->pos[i], b->dim[i]);
    }
}


/* **********
 * box3
 * **********/

void box3_init(Box3 *b, float pos[3], float dim[3])
{
    box3_setpositionv(b, pos);
    box3_setdimensionv(b, dim);
}

void box3_setpositionv(Box3 *b, float p[3])
{
    memcpy(b->pos, p, sizeof(float) * 3);
}

void box3_setcenterv(Box3 *b, float p[3])
{
    int i;
    for(i = 0; i < 3; i++)
    {
        b->pos[i] = p[i] - b->dim[i]/2.0f; 
    }
}

void box3_setdimensionv(Box3 *b, float dims[3])
{
    memcpy(b->dim, dims, sizeof(float) * 3);
}

void box3_movev(Box3 *b, float dv[3])
{
    b->pos[0] = b->pos[0] + dv[0];
    b->pos[1] = b->pos[1] + dv[1];
    b->pos[2] = b->pos[2] + dv[2];
}

void box3_scalev(Box3 *b, float scale[3])
{
    b->dim[0] *= scale[0];
    b->dim[1] *= scale[1];
    b->dim[2] *= scale[2];
}

bool box3_collides(Box3 *a, Box3 *b)
{
    bool ret = true;
    int i;
    for(i = 0; i < 3; i++)
    {
        if(!overlaps(a->pos[i], a->dim[i], b->pos[i], b->dim[i]))
        {
            ret = false;
            break;
        }
    }
    return ret;
}

void box3_mtv(Box3 *a, Box3 *b, float out_dv[3])
{
    int i;
    for(i = 0; i < 3; i++)
    {
        out_dv[i] = mtd(a->pos[i], a->dim[i], b->pos[i], b->dim[i]); //TODO: only keep max
    }
}

#ifdef _MESH_H

inline void min3(float *aret, float *b)
{
    if(b[0] < aret[0]) aret[0] = b[0];
    if(b[1] < aret[1]) aret[1] = b[1];
    if(b[2] < aret[2]) aret[2] = b[2];
}

inline void max3(float *aret, float *b)
{
    if(b[0] > aret[0]) aret[0] = b[0];
    if(b[1] > aret[1]) aret[1] = b[1];
    if(b[2] > aret[2]) aret[2] = b[2];
}

void box3_initfrommesh(Box3 *b, struct Mesh *m)
{
    b->pos[0] = m->verts[0].position[0];
    b->pos[1] = m->verts[0].position[1];
    b->pos[2] = m->verts[0].position[2];

    b->dim[0] = m->verts[0].position[0];
    b->dim[1] = m->verts[0].position[1];
    b->dim[2] = m->verts[0].position[2];

    int i;
    for(i = 1; i < m->nverts; i++)
    {
        min3(b->pos, m->verts[i].position);
        max3(b->dim, m->verts[i].position);
    }

    b->dim[0] -= b->pos[0];
    b->dim[1] -= b->pos[1];
    b->dim[2] -= b->pos[2];
}
#endif
