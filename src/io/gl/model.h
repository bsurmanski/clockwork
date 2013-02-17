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
    uint16_t nbones;
    uint16_t nposes;
    struct Mesh     *meshes;
    struct Texture  *color;
    struct Texture  *normal;
    struct Pose     *poses;
} Model;

#endif
