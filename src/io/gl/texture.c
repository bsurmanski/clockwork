/**
 * texture.c
 * obj
 * February 3, 2012
 * Brandon Surmanski
 */

#include <GL/glew.h>
#include <GL/gl.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include "io/tga.h"
#include "util/noise.h"

#include "texture.h"

/*
 * O_BINARY is a flag required by windows for calls to 'open'. if not
 * on Windows, define it to be harmless
 */
#ifndef __WIN32
#define O_BINARY 0
#endif

#define EXT_UKN -1
#define EXT_TGA 1
#define EXT_PNG 2

#define OFFSET(a,n) ((void*)((char*)(a) + (n)))

struct TextureFormat
{
    uint8_t depth; 
    uint16_t internalFormat;
    uint16_t format;
    uint16_t type;
};

static struct TextureFormat texFormat[] = 
{
    {4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE},                   // RGBA
    {3, GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE},                     // RGB
    {4, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT},    // DEPTH
    {4, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT},              // INT
    {4, GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT}              // RG
};

static void texture_loadfile(struct Texture *texture, const char *filenm);
static int texture_pitch(struct Texture *texture);
static int texture_depth(struct Texture *texture);

static void texture_loadfile(struct Texture *texture, const char *filenm)
{
    //TODO: determine what filetype the file is (this is only valid for tga right now)
    int err;
    int fd = open(filenm, O_RDONLY | O_BINARY);

    struct tga_header header;
    err = tga_header_read(fd, &header);
    if (err) {
        printf("Invalid TGA file");
        return;
    }
    int buf_sz = tga_image_sz(&header);
    texture->bits = malloc(buf_sz);
    texture->w = header.img.w;
    texture->h = header.img.h;
    texture->format = header.img.depth ==  32 ? TEXTURE_RGBA : TEXTURE_RGB; //TODO: cover all
    texture->fmt = &texFormat[texture->format];
    texture->gltype = GL_TEXTURE_2D;
    tga_image_read(fd, &header, texture->bits);
    close(fd);
}

static int texture_pitch(struct Texture *texture)
{
    return texture->w * texture->fmt->depth;
}

static int texture_depth(struct Texture *texture)
{
    return texture->fmt->depth;
}

void texture_initfromfile(struct Texture *texture, const char *filenm, int options)
{
    texture->w = 0;
    texture->h = 0;
    texture->options = options;
    texture->format = 0;
    texture->fmt = &texFormat[texture->format];
    texture->gltype = GL_TEXTURE_2D;
    texture->bits = NULL;

    texture_loadfile(texture, filenm);
    //texture_gen_perlin(texture, NULL);

#ifndef NO_GL
    if(options & TEXTURE_HARDWARE)
    {
        glGenTextures(1, &texture->glid);
        texture_commit(texture);
    }
#endif

    if(!(options & TEXTURE_SOFTWARE))
    {
        free(texture->bits);
        texture->bits = NULL;
    }
}

void texture_init(struct Texture *tex, int w, int h, int format, int options)
{
    tex->w = w;
    tex->h = h;
    tex->options = options;
    tex->format = format;
    tex->gltype = GL_TEXTURE_2D;
    tex->fmt = &texFormat[tex->format];
    tex->bits = malloc(tex->fmt->depth * w * h);

    glGenTextures(1, &tex->glid);
    texture_commit(tex);
}

void texture_finalize(struct Texture *texture)
{
    if((texture->options & TEXTURE_SOFTWARE))
    {
        free(texture->bits); 
    }

#ifndef NO_GL
    glDeleteTextures(1, &texture->glid);
#endif
}

void texture_commit(struct Texture *texture)
{
    //TODO: asynchronous. make a queue for GL updates, have this called in main thread
    GLenum interpolation = (texture->options & TEXTURE_NOINTERP) ? GL_NEAREST : GL_LINEAR;

    glBindTexture(texture->gltype, texture->glid);
    glTexParameteri(texture->gltype, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(texture->gltype, GL_TEXTURE_MAG_FILTER, interpolation);
    glTexParameteri(texture->gltype, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->gltype, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //TODO: invert RGBA to BGRA
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
            texture->gltype,
            0,
            texture->fmt->internalFormat,
            texture->w,
            texture->h,
            0,
            texture->fmt->format,
            texture->fmt->type,
            texture->bits);
}

uint32_t texture_getpixel(struct Texture *texture, int x, int y)
{
    uint8_t *ret;
    if(texture->options & TEXTURE_HARDWARE)
    {
        assert(false && "cannot get pixel from hardware texture");
        //TODO sample from gl 
    } else 
    {
        assert(texture->options & TEXTURE_SOFTWARE);
        ret = ((uint8_t*) texture->bits) + 
                texture_depth(texture) * x + texture_pitch(texture) * y;
    }
    return *((uint32_t*) ret);
}

void texture_setpixel(struct Texture *texture, int x, int y, uint32_t val)
{
    if(texture->options & TEXTURE_HARDWARE)
    {
        glTexSubImage2D(
                texture->gltype,
                0,
                x,
                y,
                1,
                1,
                texture->fmt->format,
                texture->fmt->type,
                &val
                );
    } else
    {
        assert(texture->options & TEXTURE_SOFTWARE);
        uint8_t *pxl = ((uint8_t*) texture->bits) +
                texture_depth(texture) * x + texture_pitch(texture) * y;
        memcpy(pxl, &val, texture_depth(texture));
    }
}
