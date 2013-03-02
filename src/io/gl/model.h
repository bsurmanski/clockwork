/**
 * model.h
 * clockwork
 * February 08, 2013
 * Brandon Surmanski
 */

#ifndef _MODEL_H
#define _MODEL_H

#include "util/math/geom/ball.h"
#include "util/struct/varray.h"

struct Mesh;
struct Texture;
struct Pose;

/**
 * contains a sigle reference to a mesh, color texture, normal texture and armature.
 * each part is optional
 */
typedef struct ModelFeature
{
    struct Ball3     bounds;
    struct Mesh     *mesh;
    struct Texture  *color;
    struct Texture  *normal;
    struct Armature *armature;
} ModelFeature;

typedef struct Model
{
    int     refcount;
    Varray  features;
} Model;

void model_init(Model *model);
void model_finalize(Model *model);
void model_acquire(Model *model);
void model_release(Model *model);
void model_addfeature(  Model *model, 
                        struct Mesh *mesh,
                        struct Texture *color,
                        struct Texture *normal,
                        struct Armature *armature);

#endif
