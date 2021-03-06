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
#include <stdio.h>
#include <string.h>

#include <GL/glfw.h>
#include <GL/gl.h>

#include <glb/glb.h>

#include "util/math/convert.h"
#include "util/math/matrix.h"
#include "util/math/vec.h"
#include "io/gl/framebuffer.h"
#include "io/gl/mesh.h"
#include "io/gl/armature.h"
#include "io/gl/texture.h"
#include "io/gl/model.h"
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
static GLBProgram *glbdrawmodel = NULL;
static GLBFramebuffer *glbframebuffer = NULL;
static Framebuffer framebuffers[2];

static vec3 ambient;

GLuint UNIT_SQUARE_VAO;
GLuint UNIT_SQUARE;
GLBBuffer *glbQUAD;
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

//TODO: remove bind functions.
static void gl_bindshader(struct Shader *shader);
static void gl_bindtexture(struct Texture *tex, int texture_unit);
static void gl_bindtextures(struct Texture **tex, int n);
static void gl_bindframebuffer(struct Framebuffer *f, enum Framebuffer_IO io);
static void gl_unbindshader(void);
static void gl_unbindtexture(int texture_unit);
static void gl_unbindframebuffer(enum Framebuffer_IO io);

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
    glGenVertexArrays(1, &UNIT_SQUARE_VAO);

    glBindVertexArray(UNIT_SQUARE_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, UNIT_SQUARE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UNIT_SQUARE_VERTS), UNIT_SQUARE_VERTS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(CW_POSITION);
    glEnableVertexAttribArray(CW_NORMAL);
    glEnableVertexAttribArray(CW_UV);
    glVertexAttribPointer(CW_POSITION,      3, GL_FLOAT,           false, 32, (void *) 0);
    glVertexAttribPointer(CW_NORMAL,        3, GL_SHORT,           true,  32, (void *) 12);
    glVertexAttribPointer(CW_UV,            2, GL_UNSIGNED_SHORT,  true,  32, (void *) 18);
    glVertexAttribPointer(CW_MATERIAL,      1, GL_UNSIGNED_SHORT,  true,  32, (void *) 22);
    glVertexAttribPointer(CW_BONEIDS,       2, GL_UNSIGNED_BYTE,   false, 32, (void *) 24);
    glVertexAttribPointer(CW_BONEWEIGHTS,   2, GL_UNSIGNED_BYTE,   true,  32, (void *) 26);
    glBindVertexArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    drawmodel = malloc(sizeof(Shader));
    shader_init(drawmodel, "clockwork/glsl/drawmodel");
    shader_add_fragment_output(drawmodel, "outColor");
    shader_add_fragment_output(drawmodel, "outNormal");
    shader_add_texture_target(drawmodel, "inColor", 0);

    /*{{{ XXX debug*/
    printf("current\n");
    int max_attribs;
    glGetProgramiv(drawmodel->program, GL_ACTIVE_ATTRIBUTES, &max_attribs);
    int i;
    for(i = 0; i < max_attribs; i++)
    {
        GLchar buf[64]; 
        GLint size;
        GLenum type;
        glGetActiveAttrib(drawmodel->program, i, 64, NULL, &size, &type, buf);

        printf("attrib: %d, %s, %s\n", i, glbTypeString(type), buf);
    }
    printf("...\n");
/*}}}*/

    glbdrawmodel = glbCreateProgram(NULL);
    int err = 
    glbProgramAttachNewShaderSourceFile(glbdrawmodel, 
                                        "clockwork/glsl/drawmodel.vs", 
                                        GL_VERTEX_SHADER);
    glbProgramAttachNewShaderSourceFile(glbdrawmodel, 
                                        "clockwork/glsl/drawmodel.fs", 
                                        GL_FRAGMENT_SHADER);

    struct GLBVertexLayout vlayout[] = 
    {
        {3, GL_FLOAT,           false,  32, 0},
        {3, GL_SHORT,           true,   32, 12},
        {2, GL_UNSIGNED_SHORT,  true,   32, 18},
        {1, GL_UNSIGNED_SHORT,  false,  32, 22},
        {2, GL_UNSIGNED_BYTE,   false,  32, 24},
        {2, GL_UNSIGNED_BYTE,   true,   32, 26},
    };

    glbQUAD = glbCreateVertexBuffer(6, sizeof(gl_vertex_t), 
                    UNIT_SQUARE_VERTS, 6, vlayout, GLB_STATIC_DRAW, NULL);
   
    glbframebuffer = glbCreateFramebuffer(NULL);
    GLBTexture *fb_color[4] = {
        glbCreateTexture(0, GLB_RGBA, win_w, win_h, 1, NULL, NULL), // color
        glbCreateTexture(0, GLB_RGBA, win_w, win_h, 1, NULL, NULL), // normal
        glbCreateTexture(0, GLB_RGBA, win_w, win_h, 1, NULL, NULL), // light front
        glbCreateTexture(0, GLB_RGBA, win_w, win_h, 1, NULL, NULL), // light back
        };
    GLBTexture *fb_depth = glbCreateTexture(0, GLB_DEPTH, win_w, win_h, 1, NULL, NULL);
    GLBTexture *fb_stencil = glbCreateTexture(0, GLB_STENCIL, win_w, win_h, 1, NULL, NULL);
    glbFramebufferTexture(glbframebuffer, fb_color[0]);
    glbFramebufferTexture(glbframebuffer, fb_color[1]);
    glbFramebufferTexture(glbframebuffer, fb_color[2]);
    glbFramebufferTexture(glbframebuffer, fb_color[3]);
    glbFramebufferTexture(glbframebuffer, fb_depth);
    glbFramebufferTexture(glbframebuffer, fb_stencil);

    // framebuffer is now owner of textures
    glbReleaseTexture(fb_color[0]);
    glbReleaseTexture(fb_color[1]);
    glbReleaseTexture(fb_color[2]);
    glbReleaseTexture(fb_color[3]);
    glbReleaseTexture(fb_depth);
    glbReleaseTexture(fb_stencil);


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
    glfwCloseWindow();
}

//replace function with "enable default attribs" function
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

/* bind/unbind functions {{{*/
/* bind functions {{{*/
static void gl_bindshader(struct Shader *s)
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

static void gl_bindtexture(Texture *tex, int tunit)
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

static void gl_bindtextures(Texture **tex, int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        gl_bindtexture(tex[i], i);
    }
}

static void gl_bindframebuffer(struct Framebuffer *f, enum Framebuffer_IO io)
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
/*}}}*/

/* unbind functions {{{*/
static void gl_unbindshader(void)
{
    acv_shader = NULL;
    glUseProgram(0);
}

static void gl_unbindtexture(int tunit)
{
    assert(tunit < MAX_TEX);
    acv_texture[tunit] = NULL;
}

static void gl_unbindframebuffer(enum Framebuffer_IO io)
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
/*}}}*/
/*}}}*/

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

/* lighting {{{*/
void gl_lightambient(float color[3])
{
    /*{{{ XXX REMOVE*/
    static Shader *lightambient;
    gl_swapioframebuffers();

    static GLBProgram *glbprogram;
    static int once = 1;
    if(once)
    {
        /*
        glbprogram = glbCreateProgram(NULL);
        glbProgramAttachNewShaderSourceFile(glbprogram, 
                "clockwork/glsl/lighting/ambient.vs", GLB_VERTEX_SHADER);
        glbProgramAttachNewShaderSourceFile(glbprogram, 
                "clockwork/glsl/lighting/ambient.fs", GLB_FRAGMENT_SHADER);
                */
        //TODO: rest of shader

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
        once = 0;
    }

    gl_bindshader(lightambient);
    glBindVertexArray(UNIT_SQUARE_VAO);
    gl_bindtextures(framebuffer_textures(acv_framebuffers[FRAMEBUFFER_INPUT]), 
            framebuffer_ntextures(acv_framebuffers[FRAMEBUFFER_INPUT]));
    shader_set_parameter(lightambient, "ambient_color", color, sizeof(float[3]));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    gl_unbindshader();/*}}}*/

    //ambient = vec3_addp(color, &ambient);
}

//TODO: buffer directional light, drawing 8/16 lights at a time
//TODO: convert lighting direction to eye space, pass in MVP?
void gl_lightdirect(float color[3], float dir[3])
{
    static Shader *lightdirect;
    gl_swapioframebuffers();
    static GLBProgram *program;

    static int once = 1;
    if(once)
    {
        program = glbCreateProgram(NULL);
        glbProgramAttachNewShaderSourceFile(program, 
                "clockwork/glsl/lighting/direct.vs", GLB_VERTEX_SHADER);
        glbProgramAttachNewShaderSourceFile(program, 
                "clockwork/glsl/lighting/direct.fs", GLB_FRAGMENT_SHADER);

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
        once = 0;
    }

    gl_bindshader(lightdirect);
    glBindVertexArray(UNIT_SQUARE_VAO);
    gl_bindtextures(framebuffer_textures(acv_framebuffers[FRAMEBUFFER_INPUT]), 
            framebuffer_ntextures(acv_framebuffers[FRAMEBUFFER_INPUT]));
    shader_set_parameter(lightdirect, "light_color", color, sizeof(float[3]));
    shader_set_parameter(lightdirect, "light_direction", dir, sizeof(float[3]));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    gl_unbindshader();

    //glbProgramOutput(program, ...);
    glbProgramUniform(program, GL_FRAGMENT_SHADER, 0, sizeof(float[3]), color);
    glbProgramUniform(program, GL_FRAGMENT_SHADER, 1, sizeof(float[3]), dir);
    //glbProgramDraw(program, ...array...);

}
/*}}}*/

/* draw2D {{{*/
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

    matn tmat = alloca(sizeof(float[16]));
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
    glBindVertexArray(UNIT_SQUARE_VAO);
    glUniformMatrix3fv(tmatloc, 1, false, tmat);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    gl_unbindtexture(0);
    gl_unbindshader();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
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
    glBindVertexArray(UNIT_SQUARE_VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(t->gltype, 0);
    glBindVertexArray(0);
    gl_unbindshader();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t->gltype, 0, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
/*}}}*/

/* draw3D {{{*/
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
        memcpy(&bones[i * 2].position, &arm->bones[i].head, sizeof(float[3]));
        memcpy(&bones[i * 2 + 1].position, &arm->bones[i].tail, sizeof(float[3]));
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    mat4 t_matrix;
    mat4_identity(t_matrix);
    mat4_mult(mMat, t_matrix, t_matrix);
    mat4_mult(vMat, t_matrix, t_matrix);
    mat4_mult(pMat, t_matrix, t_matrix);

    gl_bindshader(drawbones);
    gl_bindshaderattributes(); //TODO: remove
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
/*}}}*/

/*draw mesh {{{*/
void mesh_draw_t(struct Mesh *mesh, struct Texture *texture, float *mMat, float *vMat, float *pMat)
{
    /*
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

    gl_bindshader(drawmodel);
    glBindVertexArray(mesh->vao);
    gl_bindtexture(texture, 0); //TODO: change to normal gl calls

    //glUniformMatrix4fv(tmatloc, 1, true, t_matrix);
    shader_set_parameter(drawmodel, "t_matrix", t_matrix, sizeof(float[16]));
    glUniform1i(bone_enloc, false);
    glDrawElements(GL_TRIANGLES, mesh->nfaces * 3, GL_UNSIGNED_SHORT, (void*) 0);
    glBindVertexArray(0);

    gl_unbindtexture(0);
    gl_unbindshader();
    */
    assert(false && "TODO: convert to GLB");
}

void mesh_draw_ts   (struct Mesh *mesh, 
                     GLBTexture *tex, 
                     struct Armature *arm,
                     int frame,
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

    mat4 *bmat = alloca(sizeof(mat4) * arm->nbones);
    armature_matrices(arm, frame, bmat);

    static bool TRUE = 1;
    glbProgramTexture(glbdrawmodel, GLB_FRAGMENT_SHADER, 0, tex);
    glbProgramUniform(glbdrawmodel, GLB_VERTEX_SHADER, 0, sizeof(bool), &TRUE);
    glbProgramUniformMatrix(glbdrawmodel, GLB_VERTEX_SHADER, 1, 
                            sizeof(float[16]), true, t_matrix);
    glbProgramUniformMatrix(glbdrawmodel, GLB_VERTEX_SHADER, 2, 
                            sizeof(float[16]) * arm->nbones, true, bmat);
    glbProgramOutput(glbdrawmodel, glbframebuffer);
    glbProgramDrawIndexed(glbdrawmodel, mesh->vbuffer, mesh->ibuffer);
}
/*}}}*/

void model_draw(struct Model *model, float *mMat, float *vMat, float *pMat)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4 t_matrix;
    mat4_identity(t_matrix);
    mat4_mult(mMat, t_matrix, t_matrix);
    mat4_mult(vMat, t_matrix, t_matrix);
    mat4_mult(pMat, t_matrix, t_matrix);
    //mat4_transpose(t_matrix);

    int i;
    for(i = 0; i < varray_length(&model->features); i++)
    {
        static int FALSE = 0;
        const ModelFeature *feature = varray_get(&model->features, i);
        glbProgramTexture(glbdrawmodel, GLB_FRAGMENT_SHADER, 0, feature->color);
        glbProgramUniformMatrix(glbdrawmodel, GLB_VERTEX_SHADER, 0, 
                sizeof(int), true, &FALSE);
        glbProgramUniformMatrix(glbdrawmodel, GLB_VERTEX_SHADER, 1, 
                sizeof(float[16]), true, t_matrix);
        glbProgramDrawIndexed(glbdrawmodel, feature->mesh->vbuffer, feature->mesh->ibuffer);

    }
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
    glBindVertexArray(UNIT_SQUARE_VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    //gl_unbindframebuffer(FRAMEBUFFER_INPUT);
    gl_unbindshader();

    //if(output) //TODO: clean up this too
    {
        gl_bindframebuffer(f, FRAMEBUFFER_OUTPUT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

void gl_swapbuffers(void)
{
    gl_drawframebuffer();
    glfwSwapBuffers();
}
