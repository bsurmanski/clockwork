/**
 * model.h
 * clockwork
 * February 08, 2013
 * Brandon Surmanski
 */

#ifndef _MODEL_H
#define _MODEL_H

struct Mesh;
struct Texture;
struct Pose;

/**
 * contains a sigle reference to a mesh, color texture, normal texture and armature.
 * each part is optional
 */
typedef struct ModelComponent
{
    struct Ball3 bounds;
    struct Mesh *mesh;
    struct Texture *color;
    struct Texture *normal;
    struct Armature *armature;
} ModelComponent;

typedef struct Model
{
    int ncomponents;
    struct ModelComponent *components;
} Model;

void model_init(Model *model);
void model_finalize(Model *model);
void model_addmesh(Model *model, Mesh *mesh, Texture *color, Texture *normal);

#endif
