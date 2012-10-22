/**
 * texture.c
 * obj
 * February 3, 2012
 * Brandon Surmanski
 */

#include <GL/glew.h>
#include <GL/gl.h>

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

static void texture_loadfile(struct texture_t *texture, const char *filenm);
static void texture_commit_2d(struct texture_t *texture);
static void texture_commit_cubemap(struct texture_t *texture);

static void texture_loadfile(struct texture_t *texture, const char *filenm)
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
    texture->depth = header.img.depth;
    tga_image_read(fd, &header, texture->bits);
    close(fd);
}

void texture_init(struct texture_t *texture, const char *filenm, uint8_t options)
{
    texture->w = 0;
    texture->h = 0;
    texture->depth = 0;
    texture->options = options;
    texture->binding = -1;
    texture->gltype = (options & TEXTURE_CUBEMAP) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    texture->bits = NULL;

    texture_loadfile(texture, filenm);
    //texture_gen_perlin(texture, NULL);

#ifndef NO_GL
    glGenTextures(1, &texture->glid);
    texture_gpucommit(texture);
#endif

    if(options & TEXTURE_GPUONLY)
    {
        free(texture->bits);
        texture->bits = NULL;
    }
}

void texture_generate(struct texture_t *texture, int w, int h, uint8_t options, TEXTURE_GENFUNC gen_func, void *params)
{
    texture->w = w;
    texture->h = h;
    texture->depth = 32;
    texture->options = options;
    texture->binding = -1;
    texture->gltype = (options & TEXTURE_CUBEMAP) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
    glGenTextures(1, &texture->glid);
    texture->bits = malloc(w * h * texture->depth / 8);

    gen_func(texture, params);

    texture_gpucommit(texture);
    if(options & TEXTURE_GPUONLY)
    {
        free(texture->bits);
        texture->bits = NULL;
    }
}

void texture_finalize(struct texture_t *texture)
{
    if(!(texture->options & TEXTURE_GPUONLY))
    {
        free(texture->bits); 
    }

#ifndef NO_GL
    glDeleteTextures(1, &texture->glid);
#endif
}

static void texture_commit_2d(struct texture_t *texture)
{
    int format = (texture->depth == 32 ? GL_BGRA : GL_BGR); 
    glTexImage2D(
            texture->gltype,
            0,
            texture->depth / 8,
            texture->w,
            texture->h,
            0,
            format,
            GL_UNSIGNED_BYTE,
            texture->bits);
}

static void texture_commit_cubemap(struct texture_t *texture)
{
    int format = (texture->depth == 32 ? GL_BGRA : GL_BGR); 

    for(int i = 0; i < 6; i++)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            texture->depth / 8,
            texture->w,
            texture->h,
            0,
            format,
            GL_UNSIGNED_BYTE,
            texture->bits);
    }
}

void texture_gpucommit(struct texture_t *texture)
{
    GLenum interpolation = (texture->options & TEXTURE_NOINTERP) ? GL_NEAREST : GL_LINEAR;

    glBindTexture(texture->gltype, texture->glid);
    glTexParameteri(texture->gltype, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(texture->gltype, GL_TEXTURE_MAG_FILTER, interpolation);
    glTexParameteri(texture->gltype, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->gltype, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //TODO: invert RGBA to BGRA
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(texture->options & TEXTURE_CUBEMAP)
    {
        texture_commit_cubemap(texture); 
    } else 
    {
        texture_commit_2d(texture);
    }
}

void texture_bind(struct texture_t *texture, int texture_unit)
{
    texture->binding = texture_unit;
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(texture->gltype, texture->glid);

    if(texture->gltype == GL_TEXTURE_CUBE_MAP)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }
}

void texture_unbind(struct texture_t *texture)
{
    if(texture->binding != -1)
    {
        glActiveTexture(GL_TEXTURE0 + texture->binding);
        glBindTexture(texture->gltype, 0);
        texture->binding = -1;

        if(texture->gltype == GL_TEXTURE_CUBE_MAP)
        {
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        }
    }
}

void texture_gen_perlin(texture_t *t, void *params)
{
    int i, j;
    for(j = 0; j < t->h; j++)
    {
        for(i = 0; i < t->w; i++)
        {
            float f = noise2_perlin(i / 10.0f, j /10.0f);
            uint8_t val = (uint8_t) ((1.0f + noise2_perlin(i / 10.0f, j / 10.0f)) * 127);
            //printf("val %f\n", f);
            uint32_t rgba = (val << 24) + (val << 16) + (val << 8) + 255;
            (*(uint32_t*) OFFSET(t->bits, i + (j * t->w))) = rgba;
        }
    }
}
