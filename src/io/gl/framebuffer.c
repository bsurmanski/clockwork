/**
 * framebuffer.c
 * obj
 * July 26, 2012
 * Brandon Surmanski
 */

#include <stdio.h>
#include <stdlib.h>

#include <GL/glfw.h>
#include <GL/gl.h>

#include "io/gl/texture.h"

#include "framebuffer.h"

#define NTEXTURES 16

void framebuffer_addtexture(struct Framebuffer *f, int texture_type, int options)
{
    glBindFramebuffer(GL_FRAMEBUFFER, f->fbo);

    f->textures[f->ntextures] = malloc(sizeof(Texture));

    texture_init(f->textures[f->ntextures], f->w, f->h, texture_type, TEXTURE_HARDWARE | options);

    GLenum attachment;
    if(texture_type == TEXTURE_DEPTH)
    {
        attachment = GL_DEPTH_ATTACHMENT;
    } else 
    {
        attachment = GL_COLOR_ATTACHMENT0 + f->ncolortextures;
        f->ncolortextures++;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                    GL_TEXTURE_2D, f->textures[f->ntextures]->read->glid, 0);

    f->ntextures++;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void framebuffer_addtextures(struct Framebuffer *f, int n, ...)
{
    va_list args;
    va_start(args, n);
    int i;
    for(i = 0; i < n; i++)
    {
        int type = va_arg(args,int);
        framebuffer_addtexture(f, type, 0);
    }
    va_end(args);
}

/**
 * prepare a framebuffer, so it'll be ready for rendering
 */
void framebuffer_init(struct Framebuffer *f, int w, int h) //TODO: FBO not valid in GL2.1
{
    f->w = w;
    f->h = h;
    glGenFramebuffers(1, &f->fbo);
    //NEW STUFF
    //TODO, throw initialization into LUA
    f->ntextures = 0;
    f->ncolortextures = 0;
    f->textures = malloc(sizeof(struct texture_t*) * NTEXTURES);

#ifdef DEBUG
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR, INCOMPLETE FRAMEBUFFER: %d\n",
                        glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
        printf("FBSTATUS: %d\n", GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
        exit(-1);
    }
#endif

}

void framebuffer_finalize(struct Framebuffer *f)
{
    glDeleteFramebuffers(1, &f->fbo);
    
    int i;
    for(i = 0; i < f->ntextures; i++)
    {
        texture_finalize(f->textures[i]);
    }
    free(f->textures);
}

struct Texture **framebuffer_textures(struct Framebuffer *f)
{
    return f->textures;
}

int framebuffer_ntextures(struct Framebuffer *f)
{
    return f->ntextures;
}
