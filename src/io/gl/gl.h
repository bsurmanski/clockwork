/**
 * gl.h
 * clockwork 
 * October 25, 2012
 * Brandon Surmanski
 */

#ifndef _CWGL_H
#define _CWGL_H

#include <stdint.h>
#include <glb/glb.h>

#define TEXTUREUNIT_COLOR 0
#define TEXTUREUNIT_NORMAL 1
#define TEXTUREUNIT_DEPTH 2

struct Shader;
struct Model;
struct Mesh;
struct Armature;
struct Texture;
struct Framebuffer;

enum Vertex_Entry
{
    CW_POSITION     = 0,
    CW_NORMAL       = 1,
    CW_UV           = 2,
    CW_MATERIAL     = 3,
    CW_BONEIDS      = 4,
    CW_BONEWEIGHTS  = 5
};

enum Texture_Input
{
    CW_INCOLOR      = 0,
    CW_INNORMAL     = 1,
    CW_INLIGHT      = 2,
};

enum Fragment_Output
{
    CW_OUTCOLOR     = 0,
    CW_OUTNORMAL    = 1,
    CW_OUTLIGHT     = 2
};

enum Framebuffer_IO
{
    FRAMEBUFFER_INPUT = 0,
    FRAMEBUFFER_OUTPUT = 1
};

typedef struct gl_vertex
{
    float position[3];
    uint16_t normal[3];
    uint16_t uv[2];
    uint16_t material;
    uint8_t boneids[2];
    uint8_t boneweights[2];
    uint8_t padding[4];
} gl_vertex_t;

typedef struct gl_Uniform
{
    GLuint type;
    void *vale;
} gl_Uniform;

typedef unsigned short gl_index_t[3];

void gl_init(int win_w, int win_h);
void gl_finalize(void);

void gl_swapioframebuffers(void);

//lighting
void gl_lightambient(float color[3]);
void gl_lightpoint(float color[3], float pos[3]); //TODO: color, ambient, difuse, spec, etc
void gl_lightdirect(float color[3], float dir[3]); //TODO: make light accum one method?
void gl_lightspot(float color[3], float pos[3], float dir[3]);

//draw2D
void gl_drawtexture(struct Texture *t, float pos[2], float rotation);
void gl_processtexture(struct Texture *t, struct Shader *s);

//draw 3D
void gl_drawbones(struct Armature *arm, int frame, float *mMat, float *vMat, float *pMat);

void mesh_draw_t(struct Mesh *mesh, struct Texture *t, float *mMat, float *vMat, float *pMat); 
void mesh_draw_ts(struct Mesh *mesh, GLBTexture *t, struct Armature *arm, int frame, float *mMat, float *vMat, float *pMat); 
void mesh_draw_tn(struct Mesh *mesh, struct Texture *t, struct Texture *nmap, float *mMat, float *vMat, float *pMat); 

void model_draw(struct Model *model, float *mMat, float *vMat, float *pMat); 

void gl_drawcustom(struct Shader *s, struct Mesh *mesh, int ntextures, struct Texture *tex, int nuniforms, struct gl_Uniform *uniforms);

void gl_drawframebuffer(void); //TODO: flags for position/lighting on-off/etc
void gl_swapbuffers(void);

#endif
