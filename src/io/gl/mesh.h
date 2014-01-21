/**
 * mesh.h
 * @file    mesh.h
 * obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#ifndef _MESH_H
#define _MESH_H

#include <glb/glb.h>

#include <stdint.h>

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
    int16_t     normal[3];        
    uint16_t    uv[2];         ///< UV coordinate on texture. represented as uhvec2 @see uhvec2
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
    uint32_t    maxverts;           ///< number of vertices allocated
    uint32_t    maxfaces;           ///< number of faces allocated
    uint16_t    material;           ///< currently unused
    uint8_t     nbones;
    uint8_t     PADDING[1];
    struct Mesh_vert *verts;        ///< array of vertices
    struct Mesh_face *faces;        ///< array of faces
    struct GLBBuffer *vbuffer;
    struct GLBBuffer *ibuffer;
} Mesh;

int mesh_load(Mesh *m, const char *filenm);   //
void mesh_init(Mesh *m, int nverts, Mesh_vert *verts, int nfaces, Mesh_face *faces);
void mesh_finalize(Mesh *m);                  //

void mesh_reserveVertices(Mesh *m, int n);
void mesh_reserveFaces(Mesh *m, int n);
void mesh_addVertices(Mesh *m, int n, Mesh_vert *vert);
void mesh_addFaces(Mesh *m, int n, Mesh_face *face);

void mesh_commit(Mesh *m);
void mesh_write(Mesh *m, const char *filenm);

#endif
