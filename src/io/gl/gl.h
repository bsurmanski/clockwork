/**
 * gl.h
 * clockwork 
 * October 25, 2012
 * Brandon Surmanski
 */

#ifndef _CWGL_H
#define _CWGL_H

#include <stdint.h>

struct Shader;
struct mesh_t;
struct Texture;
struct Framebuffer;

enum Framebuffer_IO
{
    FRAMEBUFFER_INPUT = 0,
    FRAMEBUFFER_OUTPUT = 1
};

typedef struct gl_vertex
{
    float position[3];
    float normal[3];
    uint16_t uv[2];
    uint8_t padding[4];
} gl_vertex_t;

typedef unsigned short gl_index_t[3];

extern int IGLVERSION; //TODO

void gl_init(int win_w, int win_h);

void gl_bindshader(struct Shader *shader);
void gl_bindmesh(struct mesh_t *mesh);
void gl_bindtexture(struct Texture *tex, int texture_unit);
void gl_bindtextures(struct Texture **tex, int n);
void gl_bindframebuffer(struct Framebuffer *f, enum Framebuffer_IO io);

void gl_unbindshader(void);
void gl_unbindmesh(void);
void gl_unbindtexture(int texture_unit);
void gl_unbindtextures(void);
void gl_unbindframebuffer(enum Framebuffer_IO io);

void gl_swapioframebuffers(void);
void gl_accumpointlight(float pos[3]); //TODO: color, ambient, difuse, spec, etc
void gl_accumdirectlight(float pos[3], float dir[3]); //TODO: make light accum one method?
void gl_drawtexture(struct Texture *t, float pos[2], float rotation);
void gl_drawmodel(void *modelTODO, float *mMat, float *vMat, float *pMat); //TODO: model struct
void gl_drawframebuffer(struct Framebuffer *f); //TODO: flags for position/lighting on-off/etc

#endif
