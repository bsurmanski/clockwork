/**
 * cubemap.h
 * clockwork
 * November 14, 2012
 * Brandon Surmanski
 */

#ifndef _CUBEMAP_H
#define _CUBEMAP_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <stdint.h>

typedef struct Cubemap 
{
    uint16_t w;
    uint16_t h;
    uint8_t options;
    uint16_t format;
    GLuint glid;
    void *bits;
} Cubemap;

enum Cubemap_image
{
    CUBEMAP_POSITIVE_X,
    CUBEMAP_POSITIVE_Y,
    CUBEMAP_POSITIVE_Z,
    CUBEMAP_NEGATIVE_X,
    CUBEMAP_NEGATIVE_Y,
    CUBEMAP_NEGATIVE_Z
};

void cubemap_init(Cubemap *c, int options);
void cubemap_load(Cubemap *c, enum Cubemap_image i, const char *filenm);
void cubemap_finalize(Cubemap *c);

#endif
