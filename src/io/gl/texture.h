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
#define TEXTURE_NOINTERP 2
#define TEXTURE_HARDWARE 4
#define TEXTURE_SOFTWARE 8   //TODO use it

#define TEXTURE_RGBA 0
#define TEXTURE_RGB 1
#define TEXTURE_DEPTH 2
#define TEXTURE_INT 3

struct TextureFormat;

typedef struct Texture {
    uint16_t options;
    uint16_t w;
    uint16_t h;
    uint16_t format;
    GLenum gltype;
    GLuint glid;
    struct TextureFormat *fmt;
    void *bits;
} Texture;

typedef Texture texture_t;

typedef void (*TEXTURE_GENFUNC)(struct Texture *t, void *params);

void texture_initfromfile(struct Texture *texture, const char *filenm, int options);
void texture_init(struct Texture *tex, int w, int h, int format, int options);
void texture_finalize(struct Texture *texture);
void texture_commit(struct Texture *texture);
uint32_t texture_getpixel(struct Texture *texture, int x, int y);
void texture_setpixel(struct Texture *texture, int x, int y, uint32_t val);

#endif
