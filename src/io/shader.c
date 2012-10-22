/**
 * shader.c
 * obj
 * February 15, 2012
 * Brandon Surmanski
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "file.h"
#include "shader.h"
#include "io/texture.h"

#define BUF_SZ 1024
#define SHADER_VERT_EXT ".vs"
#define SHADER_FRAG_EXT ".fs"
#define SHADER_DIR "glsl/"

static int IGLVERSION = 30; //TODO: decouple this

/*struct shader_attrib_t {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const GLvoid *ptr;
};*/

struct shader_buffer_object_t 
{
    GLenum target;
    GLuint index;
};

struct shader_fragment_output_t 
{
    GLenum attachment;
};

struct shader_texture_target_t
{
    short texture_unit;
    char PADDING[2];
    GLuint location;
};

struct shader_uniform_t
{
    enum shader_variable_type type;
    GLuint index;
    void *val;
};

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
static int strnlen(const char *str, int len)
{
    const char *c;
    for(c = str; *c && len--; ++c);
    return (c - str);
}

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
        char *log = malloc(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetShaderInfoLog(vshader, BUF_SZ, 0, (GLchar*) log);
        printf("Vertex shader error (%s): %s\n", shaderFile, log);
        exit(-1);
        free(log);
        return 0;
    }
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE){
        char *log = malloc(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetShaderInfoLog(fshader, BUF_SZ, 0, (GLchar*) log);
        printf("Fragment shader error(%s): %s\n", shaderFile, log);
        exit(-1);
        free(log);
        return 0;
    }
#endif

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status != GL_TRUE){
        char *log = malloc(BUF_SZ);
        memset(log, 0, BUF_SZ);
        glGetProgramInfoLog(program, BUF_SZ, 0, (GLchar*)log);
        printf("link error(%s): %s\n", shaderFile, log);
        free(log);

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
    s->program = shader_loadVersion(program, IGLVERSION);

    s->nattribs = 0; 
    s->noutputs = 0;
    s->ntexture_targets = 0;
    s->attribs = malloc(sizeof(struct shader_attrib_t) * 16);
    s->outputs = malloc(sizeof(struct shader_fragment_output_t) * 16);
    s->texture_targets = malloc(sizeof(struct shader_texture_target_t) * 16);
}

void shader_bind(shader_t *s)
{
    glUseProgram(s->program); 

    GLenum drawbufs[s->noutputs];
    int i;

    if(s->noutputs > 0)
    {
        for(i = 0; i < s->noutputs; i++)
        {
            drawbufs[i] = s->outputs[i].attachment;
        }
        glDrawBuffers(s->noutputs, drawbufs); //TODO: make frag_output_t able to attach without ittr
    }

    for(i = 0; i < s->ntexture_targets; i++)
    {
        struct shader_texture_target_t *target = &s->texture_targets[i];
        glUniform1i(target->location, target->texture_unit);
    }
}

/*
void shader_bind_attachment(shader_t *s, shader_attachment_t *att)
{

}*/

void shader_finalize(shader_t *s)
{
    //TODO
}

/*
#include "world/scene/model.h"
#include "io/mesh.h"
void shader_bind_model(shader_t *s, struct model_t *model)
{
    
    glBindBuffer(GL_ARRAY_BUFFER, model->mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->mesh->ibo);

    int i;
    for(i = 0; i < model->ntextures; i++)
    {
        texture_bind(model->textures[i], i);
    }

    for(i = 0; i < s->nattribs; i++)
    {
        struct shader_attrib_t *att = &s->attribs[i];
        glEnableVertexAttribArray(att->index);
        glVertexAttribPointer(att->index, att->size, att->type, att->normalized, att->stride, att->ptr);
    }
}*/

void shader_unbind(shader_t *s)
{
    glUseProgram(0);
}

/*
 * ***************
 * shader bindings
 * ***************
 */

/*

void shader_binding_init(shader_binding_t *b)
{
    b->nbuffers = 0;
    b->buffers = malloc(sizeof(struct shader_buffer_object_t) * 16);
}

void shader_binding_add_buffer(shader_binding_t *b, GLenum target, GLuint id)
{
    struct shader_buffer_object_t *sbo = &b->buffers[b->nbuffers];
    sbo->target = target;
    sbo->index = id;
    b->nbuffers++;
}

void shader_binding_add_texture(shader_binding_t *b, GLenum attachment, GLuint id)
{

}*/
