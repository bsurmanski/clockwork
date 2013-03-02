/**
 * cubemap.h
 * clockwork
 * November 14, 2012
 * Brandon Surmanski
 */

#ifndef _CUBEMAP_H
#define _CUBEMAP_H

#include <GL/glfw.h>
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

enum CubemapImage
{
    CUBEMAP_POSITIVE_X = 0,
    CUBEMAP_NEGATIVE_X = 1,
    CUBEMAP_POSITIVE_Y = 2,
    CUBEMAP_NEGATIVE_Y = 3,
    CUBEMAP_POSITIVE_Z = 4,
    CUBEMAP_NEGATIVE_Z = 5
};

typedef struct CubemapIndex
{
    enum CubemapImage image;
    int s;
    int t;
} CubemapIndex;

void cubemap_init(struct Cubemap *c, int options);
void cubemap_finalize(struct Cubemap *c);
void cubemap_load(struct Cubemap *c, enum CubemapImage i, const char *filenm);
void cubemap_commit(struct Cubemap *c);
void cubemap_fill(struct Cubemap *cubemap, uint32_t color);

#endif
