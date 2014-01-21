/**
 * mesh.c
 * @file    mesh.h
 * obj
 * @date    June 1, 2012
 * @author  Brandon Surmanski
 */

#include <GL/gl.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

/**
 * load a mesh from a file
 */
int mesh_load(Mesh *m, const char *filenm)
{
    int fd = open(filenm, O_RDONLY | O_BINARY); 

    int err = 0;

    struct Mesh_header h;
    int nbyte = read(fd, &h, sizeof(Mesh_header));
    
    if(!err){
        size_t v_sz, f_sz, b_sz;

        m->nverts = h.nverts;
        m->nfaces = h.nfaces;
        m->maxverts = h.nverts;
        m->maxfaces = h.nfaces;
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
    close(fd);

#ifndef NO_GL
    //if(!err) 
    {
        mesh_commit(m);
    } 
#endif

    return err;
}

void mesh_init(Mesh *m, int nverts, Mesh_vert *verts, int nfaces, Mesh_face *faces)
{
    m->nverts = nverts; 
    m->nfaces = nfaces;
    m->maxverts = nverts;
    m->maxfaces = nfaces;
    m->material = 0;
    m->nbones = 0;
    m->verts = malloc(sizeof(Mesh_vert[nverts]));
    m->faces = malloc(sizeof(Mesh_face[nfaces]));
    memcpy(m->verts, verts, sizeof(Mesh_vert[nverts]));
    memcpy(m->faces, faces, sizeof(Mesh_face[nfaces]));

    mesh_commit(m);
}

/**
 * Finalize the mesh
 */
void mesh_finalize(Mesh *m)
{
    free(m->verts);
    free(m->faces);

    glbDeleteBuffer(m->vbuffer);
    glbDeleteBuffer(m->ibuffer);
}

void mesh_reserveVertices(Mesh *m, int n)
{

}

void mesh_reserveFaces(Mesh *m, int n)
{

}

void mesh_addVertices(Mesh *m, int n, Mesh_vert *vert)
{

}

void mesh_addFaces(Mesh *m, int n, Mesh_face *face)
{

}

void mesh_commit(Mesh *m)
{
    struct GLBVertexLayout vlayout[] = 
    {
        {3, GL_FLOAT,           false,  32, 0}, //position
        {3, GL_SHORT,           true,   32, 12},//normal
        {2, GL_UNSIGNED_SHORT,  true,   32, 18},//uv
        {1, GL_UNSIGNED_SHORT,  false,  32, 22},//material
        {2, GL_UNSIGNED_BYTE,   false,  32, 24},//boneid
        {2, GL_UNSIGNED_BYTE,   true,   32, 26},//boneweight
    };

    m->vbuffer = glbCreateVertexBuffer(m->nverts, sizeof(Mesh_vert), m->verts, 6, vlayout, GL_STATIC_DRAW, NULL);

    m->ibuffer = glbCreateIndexBuffer(m->nfaces, sizeof(Mesh_face), m->faces, GL_UNSIGNED_SHORT, GL_STATIC_DRAW, NULL);
}

/**
 * writes the mesh out to a file specified by filenm.
 */
void mesh_write(Mesh *m, const char *filenm)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = open(filenm, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, mode); 

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
}
