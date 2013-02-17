/**
 * cubemap.c
 * clockwork
 * November 14, 2012
 * Brandon Surmanski
 */

#include <GL/glfw.h>
#include <GL/gl.h>

#include "gl.h"
#include "cubemap.h"

void cubemap_init(Cubemap *c, int options)
{
    c->w = 0;
    c->h = 0;
    c->options = options;
    c->format = 0;
    c->glid = 0;

    int i;
    for(i = 0; i < 6; i++)
    {
        //c->bits[i] = malloc(c->w * c->h * sizeof(uint32_t) * 6); //TODO: set w/h
    }
}

void cubemap_load(Cubemap *c, enum Cubemap_image i, const char *filenm)
{

}

void cubemap_finalize(Cubemap *c)
{

}
