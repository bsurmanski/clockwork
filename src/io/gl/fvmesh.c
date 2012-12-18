/**
 * fvmesh.c
 * @file    fvmesh.h
 * Obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util/math/geom/tri.h"
#include "util/math/vec.h"

#include "fvmesh.h"

/**
 * formats: 
 *
 * FV (face-vertex).
 * PROS: easily usible by GPU, simple
 * CONS: lack of conectivity
 *
 * WE (winged edge)
 * PROS: conectivity data
 * CONS: must be converted to a format that the GPU can understand
 */

typedef struct fvmesh_header {
    uint8_t magic[3];       ///< the string "MDL" in a valid file
    uint8_t version;        ///< the value '2' in a valid file
    uint32_t nverts;        ///< number of vertices
    uint32_t nfaces;        ///< number of faces
    uint32_t nedges;
    uint32_t nuvs;          ///< number of UV coordinates in the texture
    uint32_t nseams;
    uint16_t format;
    uint8_t PADDING[6];    ///< padding to 32 bytes
    float volume;
} mesh_header;

typedef struct fvmesh_vert {
    vec3 position;      ///< 3D position of this vertex in space
    vec3 normal;
    uhvec2 uv;
    uint8_t padding[4];
} fvmesh_vert;

typedef struct fvmesh_face {
    uint16_t uvs[3];        ///< the UVs that this face is constituent of
} fvmesh_face;

typedef struct fvmesh_uv {
    uint16_t vert;          ///< corisponding vertex to this UV
    uint16_t material;      //TODO use
    uhvec2 co;              ///< UV coordinate on texture. represented as uhvec2 @see uhvec2
} fvmesh_uv;

/*
 * O_BINARY is a flag required under Windows for calls to 'open'. 
 * If it is not defined, then we arent using Windows, so define 
 * it to have no effect. Similarly, S_IRGRP and S_IROTH are not
 * defined under MINGW.
 */
#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif
#ifndef S_IROTH
#define S_IROTH 0
#endif

#define EXT_UKN -1
#define EXT_MDL 1

#define DEFAULT_DIR "res/"
#define DEFAULT_EXT ".mdl"

//OTHER
static void fvmesh_glBufferData(fvmesh_t *m);


/*
 * VERT OPERATIONS
 */

void fvmesh_vert_add(fvmesh_t *m, void *v)
{
    if(m->verts)
    {
        varray_add(m->verts, v);
    }
}

void fvmesh_vert_addall(fvmesh_t *m, int n, void *v)
{
    if(m->verts)
    {
        //TODO
        //varray_addall(m->verts, n, v);
    }
}

/*
 * FACE OPERATIONS
 */

void fvmesh_face_add(fvmesh_t *m, void *f)
{
    if(m->faces)
    {
        varray_add(m->faces, f);
    }
}

void fvmesh_face_addall(fvmesh_t *m, int n, void *f)
{
    if(m->faces)
    {
        //TODO
        //varray_addall(m->faces, n, f);
    }
}

/*
 * MESH OPERATIONS
 */

static void fvmesh_glBufferData(fvmesh_t *m)
{
    glGenBuffers(1, &m->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, varray_datasize(m->verts), varray_dataptr(m->verts), GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData(GL_ARRAY_BUFFER, varray_datasize(m->faces), varray_dataptr(m->faces), GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void fvmesh_init(fvmesh_t *m, uint16_t format)
{
    m->nverts = 0;
    m->nedges = 0;
    m->nfaces = 0;
    m->nuvs = 0;
    m->nseams = 0;
    m->format = format;

    m->verts = malloc(sizeof(Varray));
    m->edges = malloc(sizeof(Varray));
    m->faces = malloc(sizeof(Varray));
    m->uvs = malloc(sizeof(Varray));
    m->seams = malloc(sizeof(Varray));
    varray_init(m->verts, MESHFMT_VSZ[format]);
    //TODO: edges
    varray_init(m->faces, MESHFMT_FSZ[format]);
    //TODO:uvs
    //TODO: seams
    m->vbo = -1;
    m->ibo = -1;
    m->tex = -1;
}

/**
 * Finalize the mesh
 */
void fvmesh_finalize(fvmesh_t *m)
{
    free(m->verts);
    free(m->edges);
    free(m->faces);
    free(m->uvs);
    free(m->seams);

#ifndef NO_GL
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ibo);

    if(m->tex != -1) 
    {
        glDeleteTextures(1, &m->tex);
    }
#endif
}
