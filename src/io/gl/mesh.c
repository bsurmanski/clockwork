/**
 * mesh.c
 * @file    mesh.h
 * obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "util/math/geom/tri.h"
#include "util/math/vec.h"

#include "mesh.h"

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

#define DEFAULT_EXT ".mdl"

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

static int mesh_filetype(const char *filenm);

//OTHER


static int mesh_filetype(const char *filenm)
{
    char *p;
    p = strchr(filenm, '.');
    if(!strncmp(p, DEFAULT_EXT, 4))
        return EXT_MDL;
    return EXT_UKN;
}


/**
 * load a mesh from a file
 */
int mesh_read(Mesh *m, const char *filenm)
{
    int fd = open(filenm, O_RDONLY | O_BINARY); 

    int filetype = mesh_filetype(filenm);
    assert(filetype == EXT_MDL);
    int err = 0;

    if(filetype == EXT_MDL)
    {
        struct Mesh_header h;
        int nbyte = read(fd, &h, sizeof(Mesh_header));
        
        //err = (nbyte != sizeof(Mesh_header) || !memcmp(h.magic, "LDM", 3) || h.version != FMT_MDL);

        if(!err){
            size_t v_sz, f_sz, b_sz;

            m->nverts = h.nverts;
            m->nfaces = h.nfaces;
            m->nbones = h.nbones;
            m->material = 0;

            v_sz = m->nverts * sizeof(Mesh_vert);
            f_sz = m->nfaces * sizeof(Mesh_face);
            m->verts = malloc(v_sz);
            m->faces = malloc(f_sz);
            read(fd, m->verts, v_sz);
            read(fd, m->faces, f_sz);

            if(errno){
                errno = 0;
                err = -1;
            }
        }
    }
    close(fd);

#ifndef NO_GL
    //if(!err) 
    {
        glGenBuffers(1, &m->vbo);
        glGenBuffers(1, &m->ibo);
        mesh_commit(m);
    } 
#endif

    return err;
}

/**
 * Finalize the mesh
 */
void mesh_finalize(Mesh *m)
{
    free(m->verts);
    free(m->faces);

#ifndef NO_GL
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ibo);
#endif
}

void mesh_commit(Mesh *m)
{
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, m->nverts * sizeof(Mesh_vert), m->verts, GL_STATIC_DRAW); 
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->nfaces * sizeof(Mesh_face), m->faces, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * writes the mesh out to a file specified by filenm.
 */
void mesh_write(Mesh *m, const char *filenm)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = open(filenm, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, mode); 

    int filetype = mesh_filetype(filenm);
    int err = 0;

    struct Mesh_header h;
    memcpy(h.magic, "MDL", 3);
    h.version = 2;
    h.nverts = m->nverts;
    h.nfaces = m->nfaces;
    memset(h.PADDING, 0, sizeof(uint8_t) * 16);

    size_t v_sz, f_sz;
    v_sz = m->nverts * sizeof(Mesh_vert);
    f_sz = m->nfaces * sizeof(Mesh_face);

    write(fd, &h, sizeof(Mesh_header));
    write(fd, m->verts, v_sz);
    write(fd, m->faces, f_sz);

    if(errno){
        errno = 0;
        err = -1;
    }
    close(fd);
    //return err;
}

float mesh_volume(Mesh *m)
{
    float sum;
    int i;
    for(i = 0, sum = 0; i < m->nfaces; i++)
    {
        tri3 tri;
        uint16_t verts[3];
        memcpy(verts, &m->faces[i], sizeof(uint16_t) * 3);
        tri3_set(tri,
                m->verts[verts[0]].position,
                m->verts[verts[1]].position,
                m->verts[verts[2]].position);
        sum += tri3_signed_volume(tri);
    }
    return abs(sum);
}

float mesh_surface_area(Mesh *m)
{
    float sum;
    int i;
    for(i = 0, sum = 0; i < m->nfaces; i++)
    {
        tri3 tri;
        uint16_t verts[3];
        memcpy(verts, &m->faces[i], sizeof(uint16_t) * 3);
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

void mesh_vert_farthest(Mesh *m, vec3 point, vec3 max)
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
void mesh_vert_closest(Mesh *m, vec3 point, vec3 min)
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
 * FACE OPERATIONS
 */

void mesh_face_normal(Mesh *m, uint16_t face_i, vec3 normalbuf)
{
    //hvec3_to_vec3(m->faces[face_i].normal, normalbuf);
    //TODO
    assert(0 && "TODO");
}
