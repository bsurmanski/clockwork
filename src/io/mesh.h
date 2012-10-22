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
#include "util/math/tri.h"

#define FMT_MDL 2

/**
 * @struct mesh_header
 * The internal representation of the '.mesh' file format header.
 * This struct mimics the byte-for-byte storage format within the
 * '.mesh' file format, and can be written or read directly from 
 * the beginning of a '.mesh' file
 */

struct mesh_header;
struct mesh_vert;
struct mesh_edge;
struct mesh_face;
struct mesh_uv;
struct mesh_seam;

/**
 * @struct mesh_t
 * The internal representation of a mesh. The mesh
 * follows the 'winged-edge' format, with each data-member
 * an array with max length of UINT16_MAX. A mesh_t is stored
 * in the custom '.mdl' format
 */
typedef struct mesh_t { 
    uint32_t nverts;        ///< number of vertices. @see mesh_vert
    uint32_t nedges;        ///< number of edges. @see mesh_edge
    uint32_t nfaces;        ///< number of faces. @see mesh_face
    uint32_t nuvs;          ///< number of UV coordinates. @see mesh_uv
    uint16_t nseams;        ///< number of seams. @see mesh_seam
    uint16_t format;        ///< currently unused
    uint16_t material;      ///< currently unused
    struct mesh_vert *verts; ///< array of vertices
    struct mesh_edge *edges; ///< array of edges
    struct mesh_face *faces; ///< array of faces
    struct mesh_uv *uvs;     ///< array of UV coordinates
    struct mesh_seam *seams; ///< array of seam indices
    GLuint vbo;             ///< OpenGL vertex buffer object handle (GL_ARRAY_BUFFER)
    GLuint ibo;             ///< OpenGL index buffer object handle (GL_ELEMENT_ARRAY_BUFFER)
    GLuint tex;             ///< OpenGL texture handle (GL_TEXTURE2D)
} mesh_t;

int mesh_init(mesh_t *m, const char *filenm);
void mesh_finalize(mesh_t *m);

void mesh_subdivide(mesh_t *m);
void mesh_sphereProject(mesh_t *m);
float mesh_volume(mesh_t *m);
float mesh_surface_area(mesh_t *m);

int mesh_write(mesh_t *m, char *filenm);
bool mesh_contains_point(mesh_t *m, vec3 point);


//VERT
void mesh_vert_co(mesh_t *m, uint16_t vert_i, vec3 co);
void mesh_vert_normal(mesh_t *m, uint16_t vert_i, vec3 norm);
int mesh_vert_faces(mesh_t *m, uint16_t vert_i, uint16_t *fbuf, int max);
int mesh_vert_edges(mesh_t *m, uint16_t vert_i, uint16_t *ebuf, int max);
void mesh_vert_avg(mesh_t *m, vec3 avg);
void mesh_vert_max(mesh_t *m, vec3 max);
void mesh_vert_min(mesh_t *m, vec3 min);
void mesh_vert_farthest(mesh_t *m, vec3 point, vec3 farthest);
void mesh_vert_closest(mesh_t *m, vec3 point, vec3 closest);

//EDGE
uint16_t mesh_edge_cwEdge(mesh_t *m, uint16_t edge_i, uint16_t vert_i);
uint16_t mesh_edge_ccwEdge(mesh_t *m, uint16_t edge_i, uint16_t vert_i);
void mesh_edge_faces(mesh_t *m, uint16_t edge_i, uint16_t *fbuf);
void mesh_edge_normal(mesh_t *m, uint16_t edge_i, vec3 normalbuf);
bool mesh_edge_incidentToFace(mesh_t *m, uint16_t edge_i, uint16_t face_i);
bool mesh_edge_incidentToVert(mesh_t *m, uint16_t edge_i, uint16_t vert_i);

//FACE
bool mesh_face_isLeft(mesh_t *m, uint16_t edge_i, uint16_t face_i);
uint16_t mesh_face_cwFace(mesh_t *m, uint16_t edge_i, uint16_t vert_i);
uint16_t mesh_face_ccwFace(mesh_t *m, uint16_t edge_i, uint16_t vert_i);
uint16_t mesh_face_ccwEdge(mesh_t *m, uint16_t face_i, uint16_t edge_i);
uint16_t mesh_face_closest(mesh_t *m, uint16_t face_start, vec3 point);
int mesh_face_indexOf_vert(mesh_t *m, uint16_t face_i, uint16_t vert_i);
int mesh_face_verts(mesh_t *m, uint16_t face_i, uint16_t *vbuf, int max);
int mesh_face_edges(mesh_t *m, uint16_t face_i, uint16_t *ebuf, int max); 
int mesh_face_faces(mesh_t *m, uint16_t face_i, uint16_t *fbuf, int max);
void mesh_face_normal(mesh_t *m, uint16_t face_i, vec3 normalbuf);

#endif
