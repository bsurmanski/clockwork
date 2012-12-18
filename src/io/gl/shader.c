/**
 * shader.c
 * obj
 * February 15, 2012
 * Brandon Surmanski
 */

#include <alloca.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "io/file.h"
#include "io/gl/texture.h"

#include "shader.h"

#define BUF_SZ 1024
#define SHADER_VERT_EXT ".vs"
#define SHADER_FRAG_EXT ".fs"
#define SHADER_DIR "glsl/"

static int IGLVERSION = 30; //TODO: decouple this


void shader_add_attrib(shader_t *s, const char *nm, int size, GLenum type, bool normalized, int stride, void *ptr)
{
    assert(s->nattribs < 16);
    struct shader_attrib_t *attr = &s->attribs[s->nattribs];
    attr->index = glGetAttribLocation(s->program, nm);
    attr->size = size;
    attr->type = type;
    attr->normalized = normalized;
    attr->stride = stride;
    attr->ptr = ptr;
    s->nattribs++;
}

void shader_add_texture_target(shader_t *s, const char *nm, short texture_unit)
{
    struct shader_texture_target_t *loc = &s->texture_targets[s->ntexture_targets];
    loc->location = glGetUniformLocation(s->program, nm);
    loc->texture_unit = texture_unit;
    s->ntexture_targets++;
}

void shader_add_uniform(shader_t *s, enum shader_variable_type t, char *nm)
{
    //struct shader_uniform_t *u = &s->uniforms[s->nuniforms];
    //u->type = t;
    //u->index = glGetUniformLocation(s->program, nm);
    //u->val = NULL;

    //s->nuniforms++;
}

/**
 * @param nm: output name in the fragment shader, ex: "fragColor"
 */
void shader_add_fragment_output(shader_t *s, const char *nm)
{
    struct shader_fragment_output_t *f_out = &s->outputs[s->noutputs];
    GLuint loc = glGetFragDataLocation(s->program, nm);
    f_out->attachment = GL_COLOR_ATTACHMENT0 + loc;
    s->noutputs++;
}

/**
 * strnlen not found in gcc C99, and MINGW posix <string.h>.
 * Implimentation to allow strlen without undefined behaviour on 
 * non-nullterminated strings
 */
/*
static int strnlen(const char *str, int len)
{
    const char *c;
    for(c = str; *c && len--; ++c);
    return (c - str);
}*/

GLuint shader_load(const char *shaderFile)
{
    char vname[32]; 
    char fname[32];
    //memcpy(vname, shaderFile, strnlen(shaderFile, 32)+1);
    //memcpy(fname, shaderFile, strnlen(shaderFile, 32)+1);
    memcpy(vname, shaderFile, strlen(shaderFile)+1);
    memcpy(fname, shaderFile, strlen(shaderFile)+1);
    strcat(vname, SHADER_VERT_EXT);
    strcat(fname, SHADER_FRAG_EXT);

    char *vshaderSource = file_toString(vname);
    char *fshaderSource = file_toString(fname);
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    int status;

    glShaderSource(vshader, 1, (const GLchar**) &vshaderSource, 0);
    glShaderSource(fshader, 1, (const GLchar**) &fshaderSource, 0);
    
    glCompileShader(vshader);
    glCompileShader(fshader);

#ifdef DEBUG
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE){
        char *log = alloca(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetShaderInfoLog(vshader, BUF_SZ, 0, (GLchar*) log);
        printf("Vertex shader error (%s): %s\n", shaderFile, log);
        exit(-1);
        return 0;
    }
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE){
        char *log = alloca(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetShaderInfoLog(fshader, BUF_SZ, 0, (GLchar*) log);
        printf("Fragment shader error(%s): %s\n", shaderFile, log);
        exit(-1);
        return 0;
    }
#endif

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE){
        char *log = alloca(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetProgramInfoLog(program, BUF_SZ, 0, (GLchar*)log);
        printf("link error(%s): %s\n", shaderFile, log);

#ifdef DEBUG
        exit(-1);
#endif

        //return 0;
    }
    free(vshaderSource);
    free(fshaderSource);
    return program;
}

GLuint shader_loadVersion(const char *filenm, int GLver)
{
    if(GLver > 100)
        return -1;
    char ver[4] = {0,0,0,0};
    snprintf(ver, 4, "%d", GLver);
    char buf[32] = SHADER_DIR;
    strcat(buf, filenm);
    return shader_load(buf);
}

void shader_init(shader_t *s, const char *program)
{
    s->program = shader_load(program);

    s->nattribs = 0; 
    s->noutputs = 0;
    s->ntexture_targets = 0;
    s->attribs = malloc(sizeof(struct shader_attrib_t) * 16);
    s->outputs = malloc(sizeof(struct shader_fragment_output_t) * 16);
    s->texture_targets = malloc(sizeof(struct shader_texture_target_t) * 16);
}

void shader_finalize(shader_t *s)
{
    free(s->attribs);
    free(s->outputs);
    free(s->texture_targets);
}
