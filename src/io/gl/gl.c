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
#include <string.h>

#include <GL/glfw.h>
#include <GL/gl.h>

#include "util/math/convert.h"
#include "util/math/matrix.h"
#include "io/gl/framebuffer.h"
#include "io/gl/mesh.h"
#include "io/gl/armature.h"
#include "io/gl/texture.h"
#include "io/gl/shader.h"

#include "gl.h"

#define MAX_TEX 16

static int WINDOW_WIDTH = 0;
static int WINDOW_HEIGHT = 0;

static Texture      *acv_texture[MAX_TEX]   = {NULL};
static Shader       *acv_shader             = NULL;
static Mesh         *acv_mesh               = NULL;
static Framebuffer  *acv_framebuffers[2]    = {NULL};

static GLuint processFBO;
static Shader *drawmodel        = NULL;
static Framebuffer framebuffers[2];

GLuint UNIT_SQUARE;
static const gl_vertex_t UNIT_SQUARE_VERTS[] = 
{
    // Triangle 1
    {
        {-1,-1,0},
        {0,0,1},
        {0x0000,0x0000}
    },
    {
        {1,-1,0},
        {0,0,1},
        {0xffff,0x0000}
    },
    {
        {1,1,0},
        {0,0,1},
        {0xffff,0xffff}
    },
    // Triangle 2
    {
        {-1,-1,0},
        {0,0,1},
        {0x0000,0x0000}
    },
    {
        {1,1,0},
        {0,0,1},
        {0xffff,0xffff}
    },
    {
        {-1,1,0},
        {0,0,1},
        {0x0000,0xffff}
    }
};

void gl_init(int win_w, int win_h)
{
    WINDOW_WIDTH = win_w;
    WINDOW_HEIGHT = win_h;

    glfwInit();

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    //glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindow(win_w, win_h, 8, 8, 8, 8, 32, 0, GLFW_WINDOW);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    static GLuint bufs[16] = 
    {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8,
        GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11,
        GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14,
        GL_COLOR_ATTACHMENT15};
    glDrawBuffers(16, bufs);

    // create unit square buffer
    glGenBuffers(1, &UNIT_SQUARE);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UNIT_SQUARE_VERTS), UNIT_SQUARE_VERTS, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    drawmodel = malloc(sizeof(Shader));
    shader_init(drawmodel, "clockwork/glsl/drawmodel");
    shader_add_attrib(drawmodel, "position", 3, GL_FLOAT, false, 32, (void*) 0);
    shader_add_attrib(drawmodel, "normal", 3, GL_SHORT, true, 32, (void*) 12);
    shader_add_attrib(drawmodel, "uv", 2, GL_UNSIGNED_SHORT, true, 32, (void*) 18);
    shader_add_attrib(drawmodel, "boneids", 2, GL_UNSIGNED_BYTE, false, 32, (void*) 24);
    shader_add_attrib(drawmodel, "boneweights", 2, GL_UNSIGNED_BYTE, true, 32, (void*) 26);
    shader_add_fragment_output(drawmodel, "fragColor");
    shader_add_fragment_output(drawmodel, "fragNormal");
    shader_add_texture_target(drawmodel, "tex0", 0);


    //process framebuffer
    glGenFramebuffers(1, &processFBO);

    framebuffer_init(&framebuffers[0], win_w, win_h);
    framebuffer_init(&framebuffers[1], win_w, win_h);
    framebuffer_addtextures(&framebuffers[0], 4, 
                    TEXTURE_DEPTH, TEXTURE_RGBA, TEXTURE_RGBA, TEXTURE_RGBA);
    framebuffer_addtextures(&framebuffers[1], 4,
                    TEXTURE_DEPTH, TEXTURE_RGBA, TEXTURE_RGBA, TEXTURE_RGBA);
    gl_bindframebuffer(&framebuffers[0], FRAMEBUFFER_OUTPUT);
    gl_bindframebuffer(&framebuffers[1], FRAMEBUFFER_INPUT);
}

void gl_finalize(void)
{

}

static void gl_bindshaderattributes(void)
{
    assert(acv_shader);

//TODO: remove below
    int i;
    for(i = 0; i < acv_shader->nattribs; i++)
    {
        struct shader_attrib_t *att = &acv_shader->attribs[i];
        glEnableVertexAttribArray(att->index);
        glVertexAttribPointer(att->index, att->size, att->type, att->normalized, att->stride, att->ptr);
    }
}

//bind functions
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

void gl_bindmesh(struct Mesh *mesh)
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
    glBindTexture(tex->gltype, tex->read->glid);

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
        if(f)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, f->fbo);
            glViewport(0, 0, f->w, f->h);
        }
    } else 
    {
        assert(io == FRAMEBUFFER_INPUT && "invalid enum");
        acv_framebuffers[io] = f;
        if(f)
        {
            //gl_bindtextures(framebuffer_textures(f), framebuffer_ntextures(f));
        }
    }
    assert((!acv_framebuffers[io] || acv_framebuffers[0] != acv_framebuffers[1]) 
            && "framebuffer cannot be both input and output");
}

//unbind functions
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
    acv_framebuffers[io] = NULL;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    if(io == FRAMEBUFFER_OUTPUT)
    {
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        static GLuint drawbuf = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &drawbuf);
    }
}

void gl_swapioframebuffers(void)
{
    void *out = acv_framebuffers[0];
    void *in = acv_framebuffers[1];
    gl_unbindframebuffer(FRAMEBUFFER_INPUT);
    gl_unbindframebuffer(FRAMEBUFFER_OUTPUT);
    gl_bindframebuffer(out, FRAMEBUFFER_OUTPUT);
    gl_bindframebuffer(in, FRAMEBUFFER_INPUT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static int acv_framebuffer_w(int fb)
{
    return (acv_framebuffers[fb] ? acv_framebuffers[fb]->w : WINDOW_WIDTH);
}

static int acv_framebuffer_h(int fb)
{
    return (acv_framebuffers[fb] ? acv_framebuffers[fb]->h : WINDOW_HEIGHT);
}

//lighting
void gl_lightambient(float color[3])
{
    static Shader *lightambient;
    static GLuint ambient_color;
    gl_swapioframebuffers();

    static int once = 1;
    if(once)
    {
        lightambient = malloc(sizeof(Shader));
        shader_init(lightambient, "clockwork/glsl/lighting/ambient");
        shader_add_attrib(lightambient, "position", 2, GL_FLOAT, false, 32, (void*) 0);
        shader_add_texture_target(lightambient, "inDepth", 0);
        shader_add_texture_target(lightambient, "inColor", 1);
        shader_add_texture_target(lightambient, "inNormal", 2);
        shader_add_texture_target(lightambient, "inLight", 3);
        shader_add_fragment_output(lightambient, "outColor");
        shader_add_fragment_output(lightambient, "outNormal");
        shader_add_fragment_output(lightambient, "outLight");
        ambient_color = glGetUniformLocation(lightambient->program, "ambient_color");
        once = 0;
    }

    gl_bindshader(lightambient);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();
    gl_bindtextures(framebuffer_textures(acv_framebuffers[FRAMEBUFFER_INPUT]), 
            framebuffer_ntextures(acv_framebuffers[FRAMEBUFFER_INPUT]));
    glUniform3fv(ambient_color, 1, color);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindshader();
}

//TODO: buffer directional light, drawing 8/16 lights at a time
//TODO: convert lighting direction to eye space, pass in MVP?
void gl_lightdirect(float color[3], float dir[3])
{
    static Shader *lightdirect;
    static GLuint light_direction;
    static GLuint light_color;
    gl_swapioframebuffers();

    static int once = 1;
    if(once)
    {
        lightdirect = malloc(sizeof(Shader));
        shader_init(lightdirect, "clockwork/glsl/lighting/direct");
        shader_add_attrib(lightdirect, "position", 2, GL_FLOAT, false, 32, (void*) 0);
        shader_add_texture_target(lightdirect, "inDepth", 0);
        shader_add_texture_target(lightdirect, "inColor", 1);
        shader_add_texture_target(lightdirect, "inNormal", 2);
        shader_add_texture_target(lightdirect, "inLight", 3);
        shader_add_fragment_output(lightdirect, "outColor");
        shader_add_fragment_output(lightdirect, "outNormal");
        shader_add_fragment_output(lightdirect, "outLight");
        light_direction = glGetUniformLocation(lightdirect->program, "light_direction");
        light_color = glGetUniformLocation(lightdirect->program, "light_color");
        once = 0;
    }

    gl_bindshader(lightdirect);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();
    gl_bindtextures(framebuffer_textures(acv_framebuffers[FRAMEBUFFER_INPUT]), 
            framebuffer_ntextures(acv_framebuffers[FRAMEBUFFER_INPUT]));
    glUniform3fv(light_color, 1, color);
    glUniform3fv(light_direction, 1, dir);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindshader();
}

// draw2D
void gl_drawtexture(Texture *t, float *pos, float rotation)
{
    static float default_pos[2] = {0.0f, 0.0f};
    static GLuint tmatloc;
    static Shader *drawtexture;

    static int once = 1;
    if(once)
    {
        drawtexture = malloc(sizeof(Shader));
        shader_init(drawtexture, "clockwork/glsl/drawtexture");
        shader_add_attrib(drawtexture, "position", 2, GL_FLOAT, false, 32, (void*) 0);
        shader_add_fragment_output(drawtexture, "fragColor");
        shader_add_texture_target(drawtexture, "tex0", 0);
        tmatloc = glGetUniformLocation(drawtexture->program, "tmat");
        once = 0;
    }

    if(!pos)
    {
        pos = default_pos;
    }

    matn tmat = alloca(sizeof(float) * 3 * 3);
    matn_init(tmat, 3);
    float tsize[3] = {(float) t->w, (float) t->h, 1.0f};
    float invfbsize[3] = {(float) 1.0f / (float) acv_framebuffer_w(FRAMEBUFFER_OUTPUT),
                (float) 1.0f / (float) acv_framebuffer_h(FRAMEBUFFER_OUTPUT), 1.0f};
    matn_scalev(tmat, 3, tsize);
    mat3_rotate(tmat, 0.0f, 0.0f, rotation);
    matn_scalev(tmat, 3, invfbsize);
    mat3_translate(tmat, pos[0] / (float) acv_framebuffer_w(FRAMEBUFFER_OUTPUT), 
                pos[1] / (float) acv_framebuffer_h(FRAMEBUFFER_OUTPUT));

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

// draw3D
void gl_drawbones(Armature *arm, int frame, float *mMat, float *vMat, float *pMat)
{
    static Shader *drawbones;
    static GLuint tmatloc;
    static GLuint bone_enloc;
    static GLuint boneloc;

    static int once = 1;
    if(once)
    {
        drawbones = malloc(sizeof(Shader));
        shader_init(drawbones, "clockwork/glsl/drawbones");
        shader_add_attrib(drawbones, "position", 3, GL_FLOAT, false, 32, (void*) 0);
        shader_add_fragment_output(drawbones, "fragColor");

        tmatloc = glGetUniformLocation(drawbones->program, "t_matrix");
        bone_enloc = glGetUniformLocation(drawbones->program, "bones_enable");
        boneloc = glGetUniformLocation(drawbones->program, "bones");
        once = 0;
    }

    glDisable(GL_DEPTH_TEST);
    static GLuint bonetmp = 0;
    if(!bonetmp)
    {
        glGenBuffers(1, &bonetmp);
    }
    glBindBuffer(GL_ARRAY_BUFFER, bonetmp);
    glBufferData(GL_ARRAY_BUFFER, arm->nbones * 64, NULL, GL_STREAM_DRAW);
    struct Mesh_vert *bones = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int i;
    for(i = 0; i < arm->nbones; i++)
    {
        //TODO: remove
        memcpy(&bones[i * 2].position, &arm->bones[i].head, sizeof(float) * 3);
        memcpy(&bones[i * 2 + 1].position, &arm->bones[i].tail, sizeof(float) * 3);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    mat4 t_matrix;
    mat4_identity(t_matrix);
    mat4_mult(mMat, t_matrix, t_matrix);
    mat4_mult(vMat, t_matrix, t_matrix);
    mat4_mult(pMat, t_matrix, t_matrix);

    gl_bindshader(drawbones);
    gl_bindshaderattributes();
    glUniformMatrix4fv(tmatloc, 1, true, t_matrix);
    glUniform1i(bone_enloc, true);

    mat4 *m = alloca(arm->nbones * sizeof(mat4));
    armature_matrices(arm, frame, m);

    glUniformMatrix4fv(boneloc, arm->nbones, true, (float*)m);
    glDrawArrays(GL_LINES, 0, arm->nbones * 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindshader();
    glEnable(GL_DEPTH_TEST);
}

//TODO: model struct
void gl_drawmodel(Mesh *mesh, Texture *t, float *mMat, float *vMat, float *pMat)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 t_matrix;
    mat4_identity(t_matrix);
    mat4_mult(mMat, t_matrix, t_matrix);
    mat4_mult(vMat, t_matrix, t_matrix);
    mat4_mult(pMat, t_matrix, t_matrix);

    static GLuint tmatloc;
    static GLuint nmatloc;
    static GLuint bone_enloc;
    static int once = 1;
    if(once)
    {
        tmatloc = glGetUniformLocation(drawmodel->program, "t_matrix");
        bone_enloc = glGetUniformLocation(drawmodel->program, "bones_enable");
        once=0;
    }

    gl_bindtexture(t, 0);
    gl_bindshader(drawmodel);
    gl_bindshaderattributes();
    gl_bindmesh(mesh);

    glUniformMatrix4fv(tmatloc, 1, true, t_matrix);
    glUniform1i(bone_enloc, false);
    glDrawElements(GL_TRIANGLES, mesh->nfaces * 3, GL_UNSIGNED_SHORT, (void*) 0);

    gl_unbindmesh();
    gl_unbindtexture(0);
    gl_unbindshader();
}

void gl_drawmodelskinned(struct Mesh *mesh, 
                         struct Armature *arm,
                         int frame,
                         struct Texture *tex, 
                         float *mMat, 
                         float *vMat, 
                         float *pMat)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 t_matrix;
    mat4_identity(t_matrix);
    mat4_mult(mMat, t_matrix, t_matrix);
    mat4_mult(vMat, t_matrix, t_matrix);
    mat4_mult(pMat, t_matrix, t_matrix);

    static GLuint tmatloc;
    static GLuint nmatloc;
    static GLuint bone_enloc;
    static GLuint boneloc;
    static int once = 1;
    if(once)
    {
        tmatloc = glGetUniformLocation(drawmodel->program, "t_matrix");
        bone_enloc = glGetUniformLocation(drawmodel->program, "bones_enable");
        boneloc = glGetUniformLocation(drawmodel->program, "bones");
        once = 0;
    }

    gl_bindtexture(tex, 0);
    gl_bindshader(drawmodel);
    gl_bindshaderattributes();
    gl_bindmesh(mesh);

    glUniformMatrix4fv(tmatloc, 1, true, t_matrix);
    mat4 *bmat = alloca(sizeof(mat4) * arm->nbones);
    armature_matrices(arm, frame, bmat);
    glUniformMatrix4fv(boneloc, arm->nbones, true, (float*) bmat);
    glUniform1i(bone_enloc, true);
    glDrawElements(GL_TRIANGLES, mesh->nfaces * 3, GL_UNSIGNED_SHORT, (void*) 0);

    gl_unbindmesh();
    gl_unbindtexture(0);
    gl_unbindshader();
}

void gl_drawframebuffer(void)
{
    //TODO: clean up this silly conditional
    Framebuffer *f = acv_framebuffers[FRAMEBUFFER_INPUT];
    gl_swapioframebuffers();
    gl_unbindframebuffer(FRAMEBUFFER_OUTPUT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static Shader *drawframebuffer;
    static int once = 1;
    if(once)
    {
        drawframebuffer = malloc(sizeof(Shader));
        shader_init(drawframebuffer, "glsl/drawframebuffer");
        shader_add_attrib(drawframebuffer, "position", 2, GL_FLOAT, false, 32, (void*) 0);
        shader_add_fragment_output(drawframebuffer, "outColor");
        shader_add_texture_target(drawframebuffer, "depthTex", 0);
        shader_add_texture_target(drawframebuffer, "colorTex", 1);
        shader_add_texture_target(drawframebuffer, "normalTex", 2);
        shader_add_texture_target(drawframebuffer, "lightTex", 3);
        once = 0;
    }

    glViewport(0, 0, acv_framebuffer_w(FRAMEBUFFER_OUTPUT), acv_framebuffer_h(FRAMEBUFFER_OUTPUT));
    //gl_bindframebuffer(f, FRAMEBUFFER_INPUT);
    gl_bindtextures(framebuffer_textures(acv_framebuffers[FRAMEBUFFER_INPUT]), 
            framebuffer_ntextures(acv_framebuffers[FRAMEBUFFER_INPUT]));
    gl_bindshader(drawframebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //gl_unbindframebuffer(FRAMEBUFFER_INPUT);
    gl_unbindshader();

    //if(output) //TODO: clean up this too
    {
        gl_bindframebuffer(f, FRAMEBUFFER_OUTPUT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void gl_processtexture(struct Texture *t, struct Shader *s)
{
    assert(t->options & TEXTURE_DOUBLEBUFFERED);
    assert(t->read != t->write);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, processFBO);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t->gltype, t->write->glid, 0);

    gl_bindshader(s);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(t->gltype, t->read->glid);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    gl_bindshaderattributes();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(t->gltype, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl_unbindshader();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t->gltype, 0, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void gl_swapbuffers(void)
{
    gl_drawframebuffer();
    glfwSwapBuffers();
}
