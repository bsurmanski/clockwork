/**
 * framebuffer.c
 * obj
 * July 26, 2012
 * Brandon Surmanski
 */

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "io/gl/texture.h"

#include "framebuffer.h"

#define NTEXTURES 16

// framebuffer vertices, remapped to [-1, 1]
const uint8_t fb_verts[] = {
        0,      0,
        255,    0,
        255,    255,
        0,      0,
        255,    255,
        0,      255
};

void framebuffer_addTexture(struct Framebuffer *f, int texture_type)
{
    glBindFramebuffer(GL_FRAMEBUFFER, f->fbo);

    f->gltextures[f->ntextures] = malloc(sizeof(Texture));

    texture_init(f->gltextures[f->ntextures], f->w, f->h, TEXTURE_HARDWARE, texture_type);

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
                    GL_TEXTURE_2D, f->gltextures[f->ntextures]->glid, 0);

    f->ntextures++;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * prepare a framebuffer, so it'll be ready for rendering
 */
void framebuffer_init(struct Framebuffer *f, int w, int h) //TODO: FBO not valid in GL2.1
{
    f->w = w;
    f->h = h;

    glGenFramebuffers(1, &f->fbo);

    //generate screen renderquad if non existant
    glGenBuffers(1, &f->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fb_verts), fb_verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //NEW STUFF
    //TODO, throw initialization into LUA
    f->ntextures = 0;
    f->ncolortextures = 0;
    f->textures = malloc(sizeof(GLuint) * NTEXTURES);
    f->textureTargets = malloc(sizeof(GLuint) * NTEXTURES);
    glGenTextures(NTEXTURES, f->textures);

    f->gltextures = malloc(sizeof(struct texture_t*) * NTEXTURES);

#ifdef DEBUG
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR, INCOMPLETE FRAMEBUFFER: %d\n",
                        glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));
        printf("FBSTATUS: %d\n", GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
        exit(-1);
    }
#endif

}

void framebuffer_finalize(struct Framebuffer *fb)
{
    glDeleteTextures(fb->ntextures, fb->textures);
    glDeleteFramebuffers(1, &fb->fbo);
    glDeleteBuffers(1, &fb->vbo);
    free(fb->textures);
    free(fb->textureTargets);
}

struct Texture **framebuffer_textures(struct Framebuffer *f)
{
    return f->gltextures;
}

int framebuffer_ntextures(struct Framebuffer *f)
{
    return f->ntextures;
}



//TODO: remove

/*
void framebuffer_attach_src(struct Framebuffer *f)
{
    GLuint posloc = glGetAttribLocation(f->shader->program, "position");

    glUseProgram(f->shader->program);

    int i;
    int invalids = 0;
    for(i = 0; i < f->ntextures; i++)
    {
        if(f->textureTargets[i] != GL_INVALID_VALUE)
        {
            glActiveTexture(GL_TEXTURE0 + i - invalids);
            glBindTexture(GL_TEXTURE_2D, f->gltextures[i]->glid);
            glUniform1i(f->textureTargets[i], i - invalids);
        } else {
            invalids++;
        }
    }

    glEnableVertexAttribArray(posloc);

    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);

    glVertexAttribPointer(posloc, 3, GL_UNSIGNED_BYTE, GL_TRUE, 2 * sizeof(uint8_t),
            (void *) 0);
}

void framebuffer_end_src(struct Framebuffer *f)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(glGetAttribLocation(f->shader->program,
                "position"));
    int i;
    for(i = 0; i < f->ntextures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(0);
}
*/
