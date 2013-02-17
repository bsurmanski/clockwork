/**
 * framebuffer.h
 * obj
 * July 26, 2012
 * Brandon Surmanski
 */

#ifndef _FRAMEBUFFER_H 
#define _FRAMEBUFFER_H

#include <GL/glfw.h>
#include <GL/gl.h>

#include <stdarg.h>

#define FRAMEBUFFER_DEPTH 1
#define FRAMEBUFFER_STENCIL 2
#define FRAMEBUFFER_DEPTH_STENCIL 3
#define FRAMEBUFFER_RGBA 4
#define FRAMEBUFFER_INT 5

struct Texture;

typedef struct Framebuffer {
    uint16_t w;
    uint16_t h;
    GLuint fbo;
    uint16_t ntextures;
    uint16_t ncolortextures;
    struct Texture **textures;
} Framebuffer;

void framebuffer_init(struct Framebuffer *f, int w, int h);
void framebuffer_resize(struct Framebuffer *f, int w, int h);
void framebuffer_addtexture(struct Framebuffer *f, int type, int options);
void framebuffer_addtextures(struct Framebuffer *f, int n, ...);
void framebuffer_finalize(struct Framebuffer *f);
struct Texture **framebuffer_textures(struct Framebuffer *f);
int framebuffer_ntextures(struct Framebuffer *f);

void framebuffer_attach_src(struct Framebuffer *f);
void framebuffer_end_src(struct Framebuffer *f);

#endif
