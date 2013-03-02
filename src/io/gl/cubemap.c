/**
 * cubemap.c
 * clockwork
 * November 14, 2012
 * Brandon Surmanski
 */

#include <GL/glfw.h>
#include <GL/gl.h>

#include <math.h>

#include "io/gl/texture.h"
#include "gl.h"
#include "cubemap.h"

static enum CubemapImage cubemap_majoraxis(float v[3])
{
    int maxi = 0;
    float max = 0.0f; 

    int i;
    for(i = 0; i < 3; i++)
    {
        if(fabs(v[i]) > max)
        {
            maxi = i * 2;
            max = fabs(v[i]);
            if(v[i] < 0)
            {
                maxi += 1;
            }
        }
    }
    return (enum CubemapImage) maxi;
}

static CubemapIndex cubemap_index(float v[3])
{
    enum CubemapImage majoraxis = cubemap_majoraxis(v); 
    float s;
    float sc;
    float t;
    float tc;

    CubemapIndex index = {majoraxis, s, t};
    return index;
}

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

void cubemap_finalize(Cubemap *c)
{

}

void cubemap_load(Cubemap *c, enum CubemapImage i, const char *filenm)
{

}
