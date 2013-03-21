/**
 * model.c
 * clockwork
 * February 26, 2013
 * Brandon Surmanski
 */

#include "model.h"

void model_init(Model *model)
{
    model->refcount = 1;
    varray_init(&model->features, sizeof(ModelFeature));
}

void model_finalize(Model *model)
{
    //TODO: finalize features
    varray_finalize(&model->features, NULL);
}

void model_acquire(Model *model)
{
    model->refcount++;
}

void model_release(Model *model)
{
    model->refcount--;
    if(model->refcount == 0)
    {
        model_finalize(model);
    }
}

void model_addfeature(  Model *model, 
                        struct Mesh *mesh,
                        GLBTexture *color,
                        struct Texture *normal,
                        struct Armature *armature)
{
    Ball3 bounds = {0, {0,0,0}}; //TODO: bounds
    ModelFeature feature = {bounds, mesh, color, normal, armature};
    varray_add(&model->features, &feature);
}
