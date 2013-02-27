/**
 * mesh.h
 * @file    mesh.h
 * obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#ifndef _MESH_H
#define _MESH_H

#include <GL/glfw.h>
#include <GL/gl.h>

#include <stdint.h>

#include "io/gl/gl.h"
#include "util/math/vec.h"

#define FMT_MDL 4
#define BONE_NONE 255

/**
 * @struct mesh_header
 * The internal representation of the '.mesh' file format header.
 * This struct mimics the byte-for-byte storage format within the
 * '.mesh' file format, and can be written or read directly from 
 * the beginning of a '.mesh' file
 */

typedef struct Mesh_header {
    uint8_t     magic[3];       ///< the string "MDL" in a valid file
    uint8_t     version;        ///< the value '2' in a valid file
    uint32_t    nverts;        ///< number of vertices
    uint32_t    nfaces;        ///< number of faces
    uint8_t     nbones;
    uint8_t     name[16];
    uint8_t     PADDING[3];    ///< padding to 32 bytes
} Mesh_header;

//TODO: duplication of gl_vertex in 'gl.h'
typedef struct Mesh_vert {
    float       position[3];
    uhvec3      normal;        
    uhvec2      uv;              ///< UV coordinate on texture. represented as uhvec2 @see uhvec2
    uint16_t    material;      //TODO use
    uint8_t     boneid[2];
    uint8_t     boneweight[2];
    uint8_t     PADDING[4];
} Mesh_vert;

//TODO: duplication of gl_index in 'gl.h'
typedef struct Mesh_face {
    uint16_t    verts[3];        ///< the UVs that this face is constituent of
} Mesh_face;

/**
 * @struct Mesh
 * The internal representation of a mesh. The mesh
 * follows the 'vertex-face' format, with each data-member
 * an array with max length of UINT16_MAX. A Mesh is stored
 * in the custom '.mdl' format
 */
typedef struct Mesh { 
    uint32_t    nverts;             ///< number of vertices. @see mesh_vert
    uint32_t    nfaces;             ///< number of faces. @see mesh_face
    uint16_t    material;           ///< currently unused
    uint8_t     nbones;
    uint8_t     PADDING[1];
    struct Mesh_vert *verts;        ///< array of vertices
    struct Mesh_face *faces;        ///< array of faces
    GLuint vao;                     ///< OpenGL vertex array object to hold bound state
    GLuint      vbo;                ///< OpenGL vertex buffer object handle (GL_ARRAY_BUFFER)
    GLuint      ibo;                ///< OpenGL index buffer object handle (GL_ELEMENT_ARRAY_BUFFER)
} Mesh;

int mesh_initff(Mesh *m, const char *filenm);   //
void mesh_finalize(Mesh *m);                  //

void mesh_commit(Mesh *m);
void mesh_write(Mesh *m, const char *filenm);

float mesh_volume(Mesh *m);
float mesh_surface_area(Mesh *m);

bool mesh_contains_point(Mesh *m, vec3 point);


//VERT
void mesh_vert_normal(Mesh *m, uint16_t vert_i, vec3 norm);           //
void mesh_vert_farthest(Mesh *m, vec3 point, vec3 farthest);
void mesh_vert_closest(Mesh *m, vec3 point, vec3 closest);            //

uint16_t mesh_face_closest(Mesh *m, uint16_t face_start, vec3 point);
int mesh_face_indexOf_vert(Mesh *m, uint16_t face_i, uint16_t vert_i);
void mesh_face_normal(Mesh *m, uint16_t face_i, vec3 normalbuf);          //

#endif
