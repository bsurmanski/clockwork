/**
 * gl.h
 * clockwork
 * October 25, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>
#include <alloca.h>
#include <assert.h>
#include <stddef.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "util/math/matrix.h"
#include "io/gl/framebuffer.h"
#include "io/gl/mesh.h"
#include "io/gl/texture.h"
#include "io/gl/shader.h"

#include "gl.h"

#define MAX_TEX 16

static int WINDOW_WIDTH = 0;
static int WINDOW_HEIGHT = 0;

static texture_t    *acv_texture[MAX_TEX]   = {NULL};
static shader_t     *acv_shader             = NULL;
static mesh_t       *acv_mesh               = NULL;
static Framebuffer  *acv_framebuffers[2]    = {NULL};

static Shader *drawtexture      = NULL; //TODO:tmp
static Shader *drawframebuffer  = NULL;

int IGLVERSION = 30; //TODO

static GLuint UNIT_SQUARE;
static const gl_vertex_t UNIT_SQUARE_VERTS[] = 
{
    // Triangle 1
    {
        {-1,-1,0},
        {0,0,1},
        {-1,-1}
    },
    {
        {1,-1,0},
        {0,0,1},
        {1,-1}
    },
    {
        {1,1,0},
        {0,0,1},
        {1,1}
    },
    // Triangle 2
    {
        {-1,-1,0},
        {0,0,1},
        {-1,-1}
    },
    {
        {1,1,0},
        {0,0,1},
        {1,1}
    },
    {
        {-1,1,0},
        {0,0,1},
        {-1,1}
    }
};

void gl_init(int win_w, int win_h)
{
    WINDOW_WIDTH = win_w;
    WINDOW_HEIGHT = win_h;

    glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create unit square buffer
    glGenBuffers(1, &UNIT_SQUARE);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UNIT_SQUARE_VERTS), UNIT_SQUARE_VERTS, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    /*
    int i;
    for(i = 0; i < 2; i++)
    {
        framebuffers[i] = malloc(sizeof(Framebuffer));
        framebuffer_init(framebuffers[i], win_w, win_h);
        framebuffer_addtexture(framebuffers[i], TEXTURE_DEPTH); //depth
        framebuffer_addtexture(framebuffers[i], TEXTURE_RGBA); // color
        framebuffer_addtexture(framebuffers[i], TEXTURE_RGBA); // normal
        framebuffer_addtexture(framebuffers[i], TEXTURE_RGBA); // light accum
    }*/

    //TODO: note, temporary below
    drawtexture = malloc(sizeof(Shader));
    shader_init(drawtexture, "glsl/sprite");
    shader_add_attrib(drawtexture, "position", 2, GL_FLOAT, false, 32, (void*) 0);
    shader_add_fragment_output(drawtexture, "fragColor");
    shader_add_texture_target(drawtexture, "tex0", 0);

    drawframebuffer = malloc(sizeof(Shader));
    shader_init(drawframebuffer, "glsl/drawframebuffer");
    shader_add_attrib(drawframebuffer, "position", 2, GL_FLOAT, false, 32, (void*) 0);
    shader_add_fragment_output(drawframebuffer, "outColor");
    shader_add_texture_target(drawframebuffer, "depthTex", 0);
    shader_add_texture_target(drawframebuffer, "colorTex", 1);
    shader_add_texture_target(drawframebuffer, "normalTex", 2);
    shader_add_texture_target(drawframebuffer, "lightTex", 3);
}

static void gl_bindshaderattributes(void)
{
    assert(acv_shader);

    int i;
    for(i = 0; i < acv_shader->nattribs; i++)
    {
        struct shader_attrib_t *att = &acv_shader->attribs[i];
        glEnableVertexAttribArray(att->index);
        glVertexAttribPointer(att->index, att->size, att->type, att->normalized, att->stride, att->ptr);
    }
}

void gl_bindshader(struct Shader *s)
{
    acv_shader = s;

    glUseProgram(s->program);

    GLenum *drawbufs = alloca(sizeof(GLenum) * s->noutputs);
    int i;
    if(s->noutputs > 0)
    {
        for(i = 0; i < s->noutputs; i++)
        {
            drawbufs[i] = s->outputs[i].attachment;
        }
        glDrawBuffers(s->noutputs, drawbufs);
    }

    for(i = 0; i < s->ntexture_targets; i++)
    {
        struct shader_texture_target_t *target = &s->texture_targets[i];
        glUniform1i(target->location, target->texture_unit);
    }

    if(acv_shader && acv_mesh)
    {
        gl_bindshaderattributes();
    }
}

void gl_bindmesh(struct mesh_t *mesh)
{
    if(!mesh)
    {
        gl_unbindmesh();
        return;
    }

    acv_mesh = mesh;

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

    if(acv_shader && acv_mesh)
    {
        gl_bindshaderattributes();
    }
}

void gl_bindtexture(Texture *tex, int tunit)
{
    assert(tunit < MAX_TEX);

    acv_texture[tunit] = tex;
    glActiveTexture(GL_TEXTURE0 + tunit);
    glBindTexture(tex->gltype, tex->glid);

    if(tex->gltype == GL_TEXTURE_CUBE_MAP)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }
}

void gl_bindtextures(Texture **tex, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        gl_bindtexture(tex[i], i);
    }
}

void gl_bindframebuffer(struct Framebuffer *f, enum Framebuffer_IO io)
{

    if(io == FRAMEBUFFER_OUTPUT)
    {
        acv_framebuffers[io] = f;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, f->fbo);
        static GLuint bufs[16] = 
        {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
            GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
            GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8,
            GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
            GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14,
            GL_COLOR_ATTACHMENT15};
        glDrawBuffers(f->ncolortextures, bufs);
    } else 
    {
        assert(io == FRAMEBUFFER_INPUT && "invalid enum");
        gl_bindtextures(framebuffer_textures(f), framebuffer_ntextures(f));
    }
}

/*
 * UNBIND
 */

void gl_unbindshader(void)
{
    acv_shader = NULL;
    glUseProgram(0);
}

void gl_unbindmesh(void)
{
    acv_mesh = NULL;
}

void gl_unbindtexture(int tunit)
{
    assert(tunit < MAX_TEX);
    acv_texture[tunit] = NULL;
}

void gl_unbindtextures(void)
{
    int i;
    for(i = 0; i < MAX_TEX; i++)
    {
        gl_unbindtexture(i);
    }
}

void gl_unbindframebuffer(enum Framebuffer_IO io)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void gl_swapioframebuffers(void)
{
    void *out = acv_framebuffers[0];
    void *in = acv_framebuffers[1];
    gl_bindframebuffer(out, FRAMEBUFFER_OUTPUT);
    gl_bindframebuffer(in, FRAMEBUFFER_INPUT);
}

void gl_drawtexture(Texture *t, float pos[2], float rotation)
{
    static int inited = 0;
    static GLuint tmatloc;
    if(!inited)
    {
        tmatloc = glGetUniformLocation(drawtexture->program, "tmat");
        inited = 1;
    }

    matn tmat = alloca(sizeof(float) * 3 * 3);
    matn_init(tmat, 3);
    float tsize[3] = {(float) t->w, (float) t->h, 1.0f};
    float invwinsize[3] = {(float) 1.0f / WINDOW_WIDTH, (float) 1.0f / WINDOW_HEIGHT, 1.0f};
    matn_scalev(tmat, 3, tsize);
    mat3_rotate(tmat, 0.0f, 0.0f, rotation);
    matn_scalev(tmat, 3, invwinsize);
    mat3_translate(tmat, pos[0] / (float) WINDOW_WIDTH, pos[1] / (float) WINDOW_HEIGHT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    gl_bindshader(drawtexture);
    gl_bindtexture(t, 0);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();
    glUniformMatrix3fv(tmatloc, 1, false, tmat);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindtexture(0);
    gl_unbindshader();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void gl_drawframebuffer(struct Framebuffer *f)
{
    gl_bindshader(drawframebuffer);
    gl_bindframebuffer(f, FRAMEBUFFER_INPUT);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindtextures();
    gl_unbindshader();
}
