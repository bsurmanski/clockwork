/**
 * mesh.h
 * @file    mesh.h
 * obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#ifndef _MESH_H
#define _MESH_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <stdint.h>

#include "util/math/vec.h"
#include "util/struct/varray.h"

#define FMT_MDL 2

#define MESH_FV_3 1
#define MESH_FV_3_3 2
#define MESH_FV_3_3_2 3
#define MESH_WE 4

size_t MESHFMT_VSZ[] = {
    0,
    sizeof(float) * 3,
    sizeof(float) * 6,
    sizeof(float) * 8
};

size_t MESHFMT_FSZ[] = {
    0,
    sizeof(uint16_t) * 3,
    sizeof(uint16_t) * 3,
    sizeof(uint16_t) * 3
};

/**
 * @struct mesh_header
 * The internal representation of the '.mesh' file format header.
 * This struct mimics the byte-for-byte storage format within the
 * '.mesh' file format, and can be written or read directly from 
 * the beginning of a '.mesh' file
 */

struct fvmesh_header;

/**
 * @struct mesh_t
 * The internal representation of a mesh. The mesh
 * follows the 'winged-edge' format, with each data-member
 * an array with max length of UINT16_MAX. A mesh_t is stored
 * in the custom '.mdl' format
 */
typedef struct fvmesh_t { 
    uint32_t nverts;        ///< number of vertices. @see mesh_vert
    uint32_t nedges;        ///< number of edges. @see mesh_edge
    uint32_t nfaces;        ///< number of faces. @see mesh_face
    uint32_t nuvs;          ///< number of UV coordinates. @see mesh_uv
    uint16_t nseams;        ///< number of seams. @see mesh_seam
    uint16_t format;        ///< currently unused
    uint16_t material;      ///< currently unused
    Varray *verts; ///< array of vertices
    Varray *edges; ///< array of edges
    Varray *faces; ///< array of faces
    Varray *uvs;     ///< array of UV coordinates
    Varray *seams; ///< array of seam indices
    GLuint vbo;             ///< OpenGL vertex buffer object handle (GL_ARRAY_BUFFER)
    GLuint ibo;             ///< OpenGL index buffer object handle (GL_ELEMENT_ARRAY_BUFFER)
    GLuint tex;             ///< OpenGL texture handle (GL_TEXTURE2D)
} fvmesh_t;

void fvmesh_init(fvmesh_t *m, uint16_t format);
void fvmesh_finalize(fvmesh_t *m);                  //

//VERT
void fvmesh_vert_add(fvmesh_t *m, void *v);
void fvmesh_vert_addall(fvmesh_t *m, int n, void* v);

void fvmesh_face_add(fvmesh_t *m, void *f);
void fvmesh_face_addall(fvmesh_t *m, int n, void *f);

#endif
