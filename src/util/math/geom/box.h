/**
 * box.h
 * clockwork
 * November 25, 2012
 * Brandon Surmanski
 */

// axis aligned box, includes box2 (rectangle) and box3 (cuboid)

#ifndef _BOX_H
#define _BOX_H

#include <stdbool.h>

/// Rectangle (2D box)
typedef struct Box2
{
    float pos[2]; ///< position of bottom left point
    float dim[2]; ///< dimensions of each axis
} Box2;

/// Rectangular cuboid (3D box)
typedef struct Box3
{
    float pos[3]; ///< position of bottom left front back (negative) point
    float dim[3]; ///< dimensions of each axis
} Box3;

//Box2
void box2_init(Box2 *b, float center[2], float dim[2]);
void box2_setpositionv(Box2 *b, float p[2]);
void box2_setcenterv(Box2 *b, float p[2]);
void box2_setdimensionv(Box2 *b, float dims[2]);
void box2_movev(Box2 *b, float dv[2]);
void box2_scalev(Box2 *b, float scale[2]);
bool box2_collides(Box2 *a, Box2 *b);
void box2_mtv(Box2 *a, Box2 *b, float dv[2]);


//Box3
void box3_init(Box3 *b, float pos[3], float dim[3]);
void box3_setpositionv(Box3 *b, float p[3]);
void box3_setcenterv(Box3 *b, float p[3]);
void box3_setdimensionv(Box3 *b, float dims[3]);
void box3_movev(Box3 *b, float dv[3]);
void box3_scalev(Box3 *b, float scale[3]);
bool box3_collides(Box3 *a, Box3 *b);
void box3_mtv(Box3 *a, Box3 *b, float out_dv[3]); 

#ifdef _MESH_H
struct Mesh;
void box3_initfrommesh(Box3 *b, struct Mesh *m);
#endif

#endif
