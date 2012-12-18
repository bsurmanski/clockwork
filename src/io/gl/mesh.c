/**
 * mesh.c
 * @file    mesh.h
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

#include "mesh.h"

typedef struct mesh_header {
    uint8_t magic[3];       ///< the string "MDL" in a valid file
    uint8_t version;        ///< the value '2' in a valid file
    uint32_t nverts;        ///< number of vertices
    uint32_t nedges;        ///< number of edges
    uint32_t nfaces;        ///< number of faces
    uint32_t nuvs;          ///< number of UV coordinates in the texture
    uint16_t nseams;        ///< number of seam edges in the texture
    uint8_t PADDING[6];    ///< padding to 32 bytes
    float volume;
} mesh_header;

typedef struct mesh_vert {
    uint16_t incidentEdge;  ///< some edge that connects to this vertex
    uint16_t incidentFace;  ///< some face that connects to this vertex
    float position[3];      ///< 3D position of this vertex in space
} mesh_vert;

typedef struct mesh_edge {
    uint16_t verts[2];      ///< vertices bordering this edge
    uint16_t faces[2];      ///< faces bordering this edge
    uint16_t aprev;         ///< edge counter-clockwise around the first vertex
    uint16_t anext;         ///< edge clockwise around the first vertex
    uint16_t bprev;         ///< edge counter-clockwise around the second vertex
    uint16_t bnext;         ///< edge clockwise around the second vertex
} mesh_edge;

typedef struct mesh_face {
    uint16_t incidentEdge;  ///< some edge that borders this face
    uint16_t uvs[3];        ///< the UVs that this face is constituent of
    uint8_t degenerate;     ///< bool, if face has 0 area
    uint8_t PADDING[1];     ///< padding to 16 bytes
    hvec3 normal;           ///< the face normal. represented as a hvec3 @see hvec3
} mesh_face;

typedef struct mesh_uv {
    uint16_t vert;          ///< corisponding vertex to this UV
    uint16_t material;      //TODO use
    uhvec2 co;              ///< UV coordinate on texture. represented as uhvec2 @see uhvec2
} mesh_uv;

typedef struct mesh_seam {
    uint16_t edge;          ///< the edge this seam is on
} mesh_seam;

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

#define LEFT 0
#define RIGHT 1
#define VERT_A 0
#define VERT_B 1

#define DEFAULT_DIR "res/"
#define DEFAULT_EXT ".mdl"

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

typedef struct gpu_vertex {
    vec3 position;
    vec3 normal; //NOTE: normal can be expressed as uvec3 for extra 6 bytes
    uhvec2 uv;
    uint8_t padding[4];
} gpu_vertex;

typedef unsigned short gpu_index[3];

static int mesh_filetype(const char *filenm);

//OTHER
static void mesh_indices(mesh_t *m, gpu_index *indices);
static void mesh_glBufferData(mesh_t *m);


static int mesh_filetype(const char *filenm)
{
    char *p;
    p = strchr(filenm, '.');
    if(!strncmp(p, DEFAULT_EXT, 4))
        return EXT_MDL;
    return EXT_UKN;
}

bool mesh_contains_point(mesh_t *m, vec3 point)
{
    return false; //TODO
}

float mesh_volume(mesh_t *m)
{
    float sum;
    int i;
    for(i = 0, sum = 0; i < m->nfaces; i++)
    {
        tri3 tri;
        uint16_t verts[3];
        mesh_face_verts(m, i, verts, 3);
        tri3_set(tri,
                m->verts[verts[0]].position,
                m->verts[verts[1]].position,
                m->verts[verts[2]].position);
        sum += tri3_signed_volume(tri);
    }
    return abs(sum);
}

float mesh_surface_area(mesh_t *m)
{
    float sum;
    int i;
    for(i = 0, sum = 0; i < m->nfaces; i++)
    {
        tri3 tri;
        uint16_t verts[3];
        mesh_face_verts(m, i, verts, 3);
        tri3_set(tri,
                m->verts[verts[0]].position,
                m->verts[verts[1]].position,
                m->verts[verts[2]].position);
        sum += tri3_area(tri);
    }
    return sum;
}

/*
 * VERT OPERATIONS
 */

void mesh_vert_co(mesh_t *m, uint16_t vert_i, vec3 co)
{
    vec3_copy(m->verts[vert_i].position, co);
}

void mesh_vert_normal(mesh_t *m, uint16_t vert_i, vec3 norm)
{
    vec3_set(norm, 0.0f, 0.0f, 0.0f);

    uint16_t facebuf[64];
    int nfaces = mesh_vert_faces(m, vert_i, facebuf, 64);
    assert(nfaces < 64 && "mesh connectivity error (or too many edges on a vert)");

    int i;
    for(i = 0; i < nfaces; i++)
    {
        vec3 fnormal;
        mesh_face_normal(m, facebuf[i], fnormal);
        vec3_add(fnormal, norm, norm);
    }

    vec3_normalize(norm);
}

int mesh_vert_faces(mesh_t *m, uint16_t vert_i, uint16_t *fbuf, int max)
{
    int count = 0;
    uint16_t edge = m->verts[vert_i].incidentEdge;
    uint16_t face_first = m->edges[edge].faces[0];
    uint16_t face_cur = face_first;

    if(max > 0)
    {
        do
        {
            fbuf[count] = face_cur;    
            edge = mesh_edge_cwEdge(m, edge, vert_i);
            face_cur = mesh_face_cwFace(m, edge, vert_i);
        } while(++count < max && face_cur != face_first);
    }

    return count;
}

int mesh_vert_edges(mesh_t *m, uint16_t vert_i, uint16_t *ebuf, int max)
{
    int count = 0;
    uint16_t edge_first = m->verts[vert_i].incidentEdge;
    uint16_t edge = edge_first;

    if (max > 0)
    {
        do
        {
            ebuf[count] = edge; 
            edge = mesh_edge_cwEdge(m, edge, vert_i);
        } while(++count < max && edge != edge_first);
    }

    return count;
}


void mesh_vert_avg(mesh_t *m, vec3 avg)
{
    vec3_set(avg, 0.0f, 0.0f, 0.0f);

    int i;
    for(i = 0; i < m->nverts; i++)
    {
        vec3_add(avg, m->verts[i].position, avg);
    }
    vec3_scale(avg, 1.0f / m->nverts);
}

void mesh_vert_max(mesh_t *m, vec3 max)
{
    vec3_copy(m->verts[0].position, max);

    int i;
    for(i = 0; i < m->nverts; i++)
    {
        max[X] = MAX(m->verts[i].position[X], max[X]);
        max[Y] = MAX(m->verts[i].position[Y], max[Y]);
        max[Z] = MAX(m->verts[i].position[Z], max[Z]);
    }
}

void mesh_vert_min(mesh_t *m, vec3 min)
{
    vec3_copy(m->verts[0].position, min);

    int i;
    for(i = 0; i < m->nverts; i++)
    {
        min[X] = MIN(m->verts[i].position[X], min[X]);
        min[Y] = MIN(m->verts[i].position[Y], min[Y]);
        min[Z] = MIN(m->verts[i].position[Z], min[Z]);
    }
}

void mesh_vert_farthest(mesh_t *m, vec3 point, vec3 max)
{
    vec3 tmp;
    vec3_set(max, 0.0f, 0.0f, 0.0f);
    float maxdistsq = 0.0f;

    int i;
    for(i = 0; i < m->nverts; i++)
    {
        float dist_i_sq;
        vec3_sub(m->verts[i].position, point, tmp);
        dist_i_sq = vec3_lensq(tmp);
        if(dist_i_sq > maxdistsq)
        {
            vec3_copy(tmp, max);
            maxdistsq = dist_i_sq;
        }
    }
}

//NOTE: im not quite sure why this is here
void mesh_vert_closest(mesh_t *m, vec3 point, vec3 min)
{
    assert(m->nverts > 0);

    vec3 tmp;
    vec3_sub(m->verts[0].position, point, tmp);
    vec3_copy(tmp, min); //?
    float mindistsq = vec3_lensq(tmp);

    int i;
    for(i = 0; i < m->nverts; i++)
    {
        float dist_i_sq;
        vec3_sub(m->verts[i].position, point, tmp);
        dist_i_sq = vec3_lensq(tmp);
        if(dist_i_sq < mindistsq)
        {
            vec3_copy(tmp, min); //NOTE: should this be 'm->verts[i].position'?
            mindistsq = dist_i_sq;
        }
    }
}

/*
 * EDGE OPERATIONS
 */

uint16_t mesh_edge_cwEdge(mesh_t *m, uint16_t edge_i, uint16_t vert_i)
{
    mesh_edge *e = &m->edges[edge_i];
    assert((e->verts[VERT_A] == vert_i) || (e->verts[VERT_B] == vert_i));
    assert((e->verts[VERT_A] == vert_i) ^ (e->verts[VERT_B] == vert_i));
    return (e->verts[VERT_A] == vert_i) ? e->anext : e->bnext;
}

uint16_t mesh_edge_ccwEdge(mesh_t *m, uint16_t edge_i, uint16_t vert_i)
{
    mesh_edge *e = &((mesh_edge*)m->edges)[edge_i];
    return (e->verts[VERT_A] == vert_i) ? e->aprev : e->bprev;
}

void mesh_edge_faces(mesh_t *m, uint16_t edge_i, uint16_t *fbuf)
{
    fbuf[0] = m->edges[edge_i].faces[0];
    fbuf[1] = m->edges[edge_i].faces[1];
}

void mesh_edge_normal(mesh_t *m, uint16_t edge_i, vec3 normalbuf)
{
    uint16_t faces[2];
    vec3 fnormal;
    mesh_edge_faces(m, edge_i, faces);
    vec3_set(normalbuf, 0.0f, 0.0f, 0.0f);
    
    mesh_face_normal(m, faces[0], fnormal);
    vec3_add(normalbuf, fnormal, normalbuf);

    mesh_face_normal(m, faces[1], fnormal);
    vec3_add(normalbuf, fnormal, normalbuf);
    
    vec3_normalize(normalbuf);
}

bool mesh_edge_incidentToFace(mesh_t *m, uint16_t edge_i, uint16_t face_i)
{
    mesh_edge *edge = &m->edges[edge_i];
    return (edge->faces[LEFT] == face_i || edge->faces[RIGHT] == face_i);
}

bool mesh_edge_incidentToVert(mesh_t *m, uint16_t edge_i, uint16_t vert_i)
{
    mesh_edge *edge = &m->edges[edge_i];
    return (edge->verts[VERT_A] == vert_i || edge->faces[VERT_B] == vert_i);
}


/*
 * FACE OPERATIONS
 */

bool mesh_face_isLeft(mesh_t *m, uint16_t edge_i, uint16_t face_i)
{
    mesh_edge *e = &m->edges[edge_i];
    return e->faces[LEFT] == face_i;
}

uint16_t mesh_face_cwFace(mesh_t *m, uint16_t edge_i, uint16_t vert_i)
{
    mesh_edge *e = &m->edges[edge_i];
    assert((e->verts[VERT_A] == vert_i) || (e->verts[VERT_B] == vert_i));
    assert((e->verts[VERT_A] == vert_i) ^ (e->verts[VERT_B] == vert_i));
    return (e->verts[VERT_A] != vert_i) ? e->faces[RIGHT] : e->faces[LEFT];
}

uint16_t mesh_face_ccwFace(mesh_t *m, uint16_t edge_i, uint16_t vert_i)
{
    mesh_edge *e = &m->edges[edge_i];
    return (e->verts[VERT_A] == vert_i) ? e->faces[LEFT] : e->faces[RIGHT];
}

uint16_t mesh_face_ccwEdge(mesh_t *m, uint16_t face_i, uint16_t edge_i)
{
    mesh_edge *edge = &m->edges[edge_i];
    return mesh_edge_incidentToFace(m, edge->aprev, face_i) ?
        edge->aprev : edge->bprev;
}

uint16_t mesh_face_closest(mesh_t *m, uint16_t face_start, vec3 point)
{
    uint16_t fbuf[4]; 
    int nfaces = mesh_face_faces(m, face_start, fbuf, 4);
    //TODO: finish
    return 0;
}

int mesh_face_indexOf_vert(mesh_t *m, uint16_t face_i, uint16_t vert_i)
{
    int ret = -1;
    int i;
    for(i = 0; i < 3; i++)
    {
        if(m->uvs[m->faces[face_i].uvs[i]].vert == vert_i)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

int mesh_face_verts(mesh_t *m, uint16_t face_i, uint16_t *vbuf, int max)
{
    mesh_face *f = &m->faces[face_i];
    int i;
    for (i = 0; i < max && i < 3; i++)
    {
        vbuf[i] = m->uvs[f->uvs[i]].vert;
    }
    return i;
}

int mesh_face_edges(mesh_t *m, uint16_t face_i, uint16_t *ebuf, int max)
{
    mesh_face *face = &m->faces[face_i];

    ebuf[0] = face->incidentEdge;

    int i;
    uint16_t next;
    for(i = 1; i < max; i++, next = mesh_face_ccwEdge(m, face_i, ebuf[i-1]))
    {
        if(next == ebuf[0])
        {
            break;
        } 
        ebuf[i] = next;
    }

    return i;
}

int mesh_face_faces(mesh_t *m, uint16_t face_i, uint16_t *fbuf, int max)
{
    uint16_t ebuf[4];
    int nedges = mesh_face_edges(m, face_i, ebuf, 4);
    int i;
    for(i = 0; i < nedges; i++)
    {
        mesh_edge *edge = &m->edges[ebuf[i]];
        fbuf[i] = ((edge->faces[0] == face_i) ? edge->faces[1] : edge->faces[0]);
    }
    return nedges;
}

void mesh_face_normal(mesh_t *m, uint16_t face_i, vec3 normalbuf)
{
    hvec3_to_vec3(m->faces[face_i].normal, normalbuf);
}


/*
 * MESH OPERATIONS
 */

static void mesh_vertices(mesh_t *m, gpu_vertex *vertices)
{
    uint16_t i;
    for (i = 0; i < m->nuvs; i++)
    {
        mesh_uv *uv = &m->uvs[i];
        mesh_vert *vert = &m->verts[uv->vert];
        vec3 normal;
        mesh_vert_normal(m, uv->vert, normal);
        memcpy(vertices[i].position, vert->position, sizeof(vec3));
        memcpy(vertices[i].normal, normal, sizeof(vec3));
        memcpy(vertices[i].uv, uv->co, sizeof(uhvec2));
    }
}

static void mesh_indices(mesh_t *m, gpu_index *indices)
{
    uint16_t i;
    for( i = 0; i < m->nfaces; i++)
    {
        mesh_face *face = &m->faces[i];
        memcpy(indices[i], face->uvs, sizeof(uhvec3));
    }
}


static void mesh_glBufferData(mesh_t *m)
{
    glGenBuffers(1, &m->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, m->nuvs * sizeof(gpu_vertex), NULL, GL_STATIC_DRAW); 
    gpu_vertex *vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    mesh_vertices(m, vertices);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->nfaces * sizeof(gpu_index), NULL, GL_STATIC_DRAW);
    gpu_index *indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    mesh_indices(m, indices);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * load a mesh from a file
 */
int mesh_init(mesh_t *m, const char *filenm)
{
    memset(m, 0, sizeof(mesh_t));

    int fd = open(filenm, O_RDONLY | O_BINARY); 

    int filetype = mesh_filetype(filenm);
    int err = 0;

    if(filetype == EXT_MDL)
    {
        struct mesh_header h;
        int nbyte = read(fd, &h, sizeof(mesh_header));
        
        err = (nbyte != sizeof(mesh_header) || !memcmp(h.magic, "LDM", 3) || h.version != FMT_MDL);

        if(!err){
            size_t v_sz, e_sz, f_sz, uv_sz, seam_sz;

            m->nverts = h.nverts;
            m->nedges = h.nedges;
            m->nfaces = h.nfaces;
            m->nuvs = h.nuvs;
            m->format = FMT_MDL;
            m->material = 0;
            m->tex = -1;

            v_sz = m->nverts * sizeof(mesh_vert);
            e_sz = m->nedges * sizeof(mesh_edge);
            f_sz = m->nfaces * sizeof(mesh_face);
            uv_sz = m->nuvs * sizeof(mesh_uv);
            seam_sz = m->nseams * sizeof(mesh_seam);
            m->verts = malloc(v_sz);
            m->edges = malloc(e_sz);
            m->faces = malloc(f_sz);
            m->uvs = malloc(uv_sz);
            m->seams = malloc(seam_sz);
            read(fd, m->verts, v_sz);
            read(fd, m->edges, e_sz);
            read(fd, m->faces, f_sz);
            read(fd, m->uvs, uv_sz);
            read(fd, m->seams, seam_sz);

            if(errno){
                errno = 0;
                err = -1;
            }
        }
    }
    close(fd);

#ifndef NO_GL
    if(!err) 
    {
        mesh_glBufferData(m);
    } 
#endif

    return err;
}

/**
 * Finalize the mesh
 */
void mesh_finalize(mesh_t *m)
{
    free(m->verts);
    free(m->edges);
    free(m->faces);
    free(m->uvs);

#ifndef NO_GL
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ibo);

    if(m->tex != -1) 
    {
        glDeleteTextures(1, &m->tex);
    }
#endif
}

static void mesh_subdivide_edges_setWings(mesh_t *m, struct mesh_edge *sub_edge, int i)
{
     sub_edge->aprev = ((m->edges[m->edges[i].aprev].verts[VERT_A] == m->edges[i].verts[VERT_A]) ?
                m->edges[i].aprev :
                 m->edges[i].aprev + m->nedges);
     sub_edge->anext = ((m->edges[m->edges[i].anext].verts[VERT_A] == m->edges[i].verts[VERT_A]) ?
                m->edges[i].anext :
                 m->edges[i].anext + m->nedges);

     sub_edge->bprev = ((m->edges[m->edges[i].aprev].verts[VERT_B] == m->edges[i].verts[VERT_B]) ?
                m->edges[i].bprev :
                 m->edges[i].bprev + m->nedges);
     sub_edge->bnext = ((m->edges[m->edges[i].bnext].verts[VERT_B] == m->edges[i].verts[VERT_B]) ?
                m->edges[i].bnext :
                 m->edges[i].bnext + m->nedges);
}

/**
 * The mesh verts before the subdivision shall remain, with all additional verts being accounted for
 * bt edge midpoints
 */
static void mesh_subdivide_verts(mesh_t *m, struct mesh_vert *sub_verts, int sub_nverts)
{
    memcpy(sub_verts, m->verts, m->nverts * sizeof(struct mesh_vert));

    int i;
    for(i = 0; i < m->nedges; i++)
    {
        //Add midpoints of edges to sub_verts
        struct mesh_vert *midpoint = &sub_verts[m->nverts + i];
        struct mesh_edge *edge = &m->edges[i];
        struct mesh_vert *v1 = &m->verts[edge->verts[VERT_A]];
        struct mesh_vert *v2 = &m->verts[edge->verts[VERT_B]];
        midpoint->position[X] = (v1->position[X] + v2->position[X]) / 2.0f;
        midpoint->position[Y] = (v1->position[Y] + v2->position[Y]) / 2.0f;
        midpoint->position[Z] = (v1->position[Z] + v2->position[Z]) / 2.0f;
        midpoint->incidentEdge = i;
        midpoint->incidentFace = edge->faces[LEFT];
    }
}

static void mesh_subdivide_edges(mesh_t *m, struct mesh_edge *sub_edges, int sub_nedges)
{
    int i;
    for(i = 0; i < m->nedges; i++)
    {
        struct mesh_edge *edge_a = &sub_edges[i];
        struct mesh_edge *edge_b = &sub_edges[m->nedges + i];
        struct mesh_edge *edge_i = &m->edges[i];

        edge_a->verts[0] = edge_i->verts[0];
        edge_a->verts[1] = m->nverts + i; //midpoint vert
        edge_a->faces[0] = edge_i->faces[0] + (m->nfaces * mesh_face_indexOf_vert(m, edge_i->faces[0], edge_a->verts[0])); 
        edge_a->faces[1] = edge_i->faces[1] + (m->nfaces * mesh_face_indexOf_vert(m, edge_i->faces[1], edge_a->verts[0])); 
        mesh_subdivide_edges_setWings(m, edge_a, i);

        edge_b->verts[0] = m->nverts + i; //midpoint vert
        edge_b->verts[1] = edge_i->verts[1];
        edge_b->faces[0] = edge_i->faces[0] + (m->nfaces * mesh_face_indexOf_vert(m, edge_i->faces[0], edge_b->verts[1])); 
        edge_b->faces[1] = edge_i->faces[1] + (m->nfaces * mesh_face_indexOf_vert(m, edge_i->faces[1], edge_b->verts[1])); 
        mesh_subdivide_edges_setWings(m, edge_b, i);
    }
}

//TODO: finish face subdivision
static void mesh_subdivide_faces(mesh_t *m, struct mesh_face *sub_faces, int sub_nfaces)
{
    int i;
    for(i = 0; i < m->nfaces; i++)
    {
        struct mesh_face *face_a = &sub_faces[i]; //attatched to vert_a
        struct mesh_face *face_b = &sub_faces[i + m->nfaces * 1]; //attached to vert_b
        struct mesh_face *face_c = &sub_faces[i + m->nfaces * 2]; //attached to vert_c
        struct mesh_face *face_d = &sub_faces[i + m->nfaces * 3]; //middle face
    }
}

/**
 * will subdivide the mesh such that each face will be split into 4 new faces.
 * the implimentation is valid for trianglar faces. 
 *
 * When subdividing, all current vertices will remain the same, and the midpoint
 * of edges will be added as new vertices.
 * All edges will be split in 2, with an edge from vert A to the midpoint, and another
 * from the midpoint to vert B
 * All faces will be split into 4 sub faces, in the topology of a triforce
 *
 * The storage format of the new memebers relative to the current mesh is as so:
 * VERTS: [ORIGINAL VERTS | EDGE MIDPOINTS]
 * EDGES: [VERT A ADJACENT | VERT B ADJACENT]
 * FACES: [VERT 1 ADJACENT | VERT 2 ADJACENT| VERT 3 ADJACENT | MIDDLE FACE]
 */
void mesh_subdivide(mesh_t *m)
{
    int sub_nverts = (m->nverts + m->nedges); 
    int sub_nedges = (m->nedges * 2 + m->nfaces * 3);
    int sub_nfaces = (m->nfaces * 4);
    int sub_nuvs = (m->nuvs + m->nedges + m->nseams); //TODO: set number of seams
    int sub_nseams = (m->nseams * 2);
    int NMAX = 0xFFFF; //2**16, max index that can fit into mesh

    if (sub_nverts > NMAX || sub_nedges > NMAX || sub_nfaces > NMAX || sub_nuvs > NMAX || sub_nseams > NMAX)
    {
        return; //CANNOT SUBDIVIDE, SUBDIVISION WILL ESCAPE INDEX LENGTH
    }

    struct mesh_vert *sub_verts = malloc(sizeof(mesh_vert) * sub_nverts);
    struct mesh_edge *sub_edges = malloc(sizeof(mesh_edge) * sub_nedges);
    struct mesh_face *sub_faces = malloc(sizeof(mesh_face) * sub_nfaces);
    struct mesh_uv *sub_uvs = malloc(sizeof(mesh_uv) * sub_nuvs);

    mesh_subdivide_verts(m, sub_verts, sub_nverts);
    mesh_subdivide_edges(m, sub_edges, sub_nedges);
    mesh_subdivide_faces(m, sub_faces, sub_nfaces);
    //TODO: uv subdivision

    free(m->verts);
    free(m->edges);
    free(m->faces);
    m->verts = sub_verts;
    m->edges = sub_edges;
    m->faces = sub_faces;
    m->nverts = sub_nverts;
    m->nedges = sub_nedges;
    m->nfaces = sub_nfaces;
    m->nseams = sub_nseams;
}

/**
 * writes the mesh out to a file specified by filenm.
 */
int mesh_write(mesh_t *m, char *filenm)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = open(filenm, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, mode); 

    int filetype = mesh_filetype(filenm);
    int err = 0;

    if(filetype == EXT_MDL)
    {
        struct mesh_header h;
        memcpy(h.magic, "MDL", 3);
        h.version = 2;
        h.nverts = m->nverts;
        h.nedges = m->nedges;
        h.nfaces = m->nfaces;
        h.nuvs = m->nuvs;
        memset(h.PADDING, 0, sizeof(uint8_t) * 12);

        size_t v_sz, e_sz, f_sz, uv_sz;
        v_sz = m->nverts * sizeof(mesh_vert);
        e_sz = m->nedges * sizeof(mesh_edge);
        f_sz = m->nfaces * sizeof(mesh_face);
        uv_sz = m->nuvs * sizeof(mesh_uv);

        write(fd, &h, sizeof(mesh_header));
        write(fd, m->verts, v_sz);
        write(fd, m->edges, e_sz);
        write(fd, m->faces, f_sz);
        write(fd, m->uvs, uv_sz);

        if(errno){
            errno = 0;
            err = -1;
        }
    }
    close(fd);
    return err;
}

/*
void mesh_transform(mesh_t *m, void (*transform)(float a, float b, float c, ...), int extraargs, ...)
{

}*/
