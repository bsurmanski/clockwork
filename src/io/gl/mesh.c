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
int mesh_initff(Mesh *m, const char *filenm)
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
        glGenVertexArrays(1, &m->vao);
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
    glDeleteVertexArrays(1, &m->vao);
    glDeleteBuffers(1, &m->vbo);
    glDeleteBuffers(1, &m->ibo);
#endif
}

void mesh_commit(Mesh *m)
{
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, m->nverts * sizeof(Mesh_vert), m->verts, GL_STATIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->nfaces * sizeof(Mesh_face), m->faces, GL_STATIC_DRAW);

    glEnableVertexAttribArray(CW_POSITION);
    glEnableVertexAttribArray(CW_NORMAL);
    glEnableVertexAttribArray(CW_UV);
    glEnableVertexAttribArray(CW_MATERIAL);
    glEnableVertexAttribArray(CW_BONEIDS);
    glEnableVertexAttribArray(CW_BONEWEIGHTS);
    glVertexAttribPointer(CW_POSITION,      3, GL_FLOAT,           false, 32, (void *) 0);
    glVertexAttribPointer(CW_NORMAL,        3, GL_SHORT,           true,  32, (void *) 12);
    glVertexAttribPointer(CW_UV,            2, GL_UNSIGNED_SHORT,  true,  32, (void *) 18);
    glVertexAttribPointer(CW_MATERIAL,      1, GL_UNSIGNED_SHORT,  false, 32, (void *) 22);
    glVertexAttribPointer(CW_BONEIDS,       2, GL_UNSIGNED_BYTE,   false, 32, (void *) 24);
    glVertexAttribPointer(CW_BONEWEIGHTS,   2, GL_UNSIGNED_BYTE,   true,  32, (void *) 26);
    glBindVertexArray(0);

    struct GLBVertexLayout vlayout[] = 
    {
        {3, GL_FLOAT,           false,  32, 0},
        {3, GL_SHORT,           true,   32, 12},
        {2, GL_UNSIGNED_SHORT,  true,   32, 18},
        {1, GL_UNSIGNED_SHORT,  false,  32, 22},
        {2, GL_UNSIGNED_BYTE,   false,  32, 24},
        {2, GL_UNSIGNED_BYTE,   true,   32, 26},
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
