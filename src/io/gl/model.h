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

typedef struct Model
{
    uint16_t nmeshes;
    uint16_t narmatures;
    struct Mesh     *meshes;
    struct Texture  *color;
    struct Texture  *normal;
    struct Armature *armatures;
} Model;

void model_init(Model *model);
void model_addmesh(Model *model, Mesh *mesh, Texture *color, Texture *normal);

#endif
