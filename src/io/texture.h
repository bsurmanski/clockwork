/**
 * texture.h
 * obj
 * January 24, 2012
 * Brandon Surmanski
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <GL/glew.h>
#include <GL/gl.h>

#define TEXTURE_2D 1
#define TEXTURE_CUBEMAP 2
#define TEXTURE_NOINTERP 4
#define TEXTURE_GPUONLY 8
#define TEXTURE_NOGPU 16    //TODO use it

typedef struct texture_t {
    uint16_t w;
    uint16_t h;
    uint8_t depth;
    uint8_t options;
    uint8_t PADDING[2];
    int binding;
    GLenum gltype;
    GLuint glid;
    void *bits;
} texture_t;

typedef void (*TEXTURE_GENFUNC)(struct texture_t *t, void *params);

void texture_init(struct texture_t *texture, const char *filenm, uint8_t options);
void texture_generate(struct texture_t *texture, int w, int h, uint8_t options, TEXTURE_GENFUNC func, void *params);
void texture_finalize(struct texture_t *texture);
void texture_gpucommit(struct texture_t *texture);
void texture_bind(struct texture_t *texture, int texture_unit);
void texture_unbind(struct texture_t *texture);

// generated texture functions
void texture_gen_perlin(texture_t *t, void *params);

/*
struct {
    char* nm; 
    void* func;
} *TEXTURE_EXPORT;
*/
#endif
