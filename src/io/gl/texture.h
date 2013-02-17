/**
 * texture.h
 * obj
 * January 24, 2012
 * Brandon Surmanski
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <GL/glfw.h>
#include <GL/gl.h>

#define TEXTURE_2D 1
#define TEXTURE_3D 2 //TODO: unused
#define TEXTURE_LINEAR 4
#define TEXTURE_NEAREST 8
#define TEXTURE_HARDWARE 16
#define TEXTURE_SOFTWARE 32  //TODO use it
#define TEXTURE_SINGLEBUFFERED 64
#define TEXTURE_DOUBLEBUFFERED 128
#define TEXTURE_MIPMAPPED 256 //TODO: use

enum Texture_Format
{
    TEXTURE_RGBA            = 0,
    TEXTURE_RGB             = 1,
    TEXTURE_DEPTH           = 2,
    TEXTURE_STENCIL         = 3,
    TEXTURE_DEPTH_STENCIL   = 4,
    TEXTURE_INT             = 5,
    TEXTURE_SHORT           = 6,
    TEXTURE_BYTE            = 7
};

struct TextureFormat;

typedef struct Texture_Buffer 
{
    GLuint glid;
    uint8_t *bits;
} Texture_Buffer;

typedef struct Texture {
    uint16_t options;
    uint16_t w;
    uint16_t h;
    uint16_t format;
    uint32_t size;
    GLenum gltype;
    struct TextureFormat *fmt;
    Texture_Buffer buffers[2];
    Texture_Buffer *read;
    Texture_Buffer *write;
} Texture;

typedef Texture texture_t;

//TODO: better name for load file?
void texture_loadfile(struct Texture *texture, const char *filenm);
void texture_read(struct Texture *texture, const char *filenm, int options);
void texture_init(struct Texture *tex, int w, int h, enum Texture_Format format, int options);
void texture_finalize(struct Texture *texture);
void texture_commit(struct Texture *texture);
void texture_flip(struct Texture *texture);
void texture_fill(struct Texture *texture, uint32_t color);
uint32_t texture_getpixel(struct Texture *texture, int x, int y);
void texture_setpixel(struct Texture *texture, int x, int y, uint32_t val);

#endif
