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

#include <GL/glfw.h>
#include <GL/gl.h>

#include "io/gl/gl.h"
#include "io/file.h"
#include "io/gl/texture.h"

#include "shader.h"

#define BUF_SZ 1024
#define SHADER_VERT_EXT ".vs"
#define SHADER_FRAG_EXT ".fs"
#define SHADER_DIR "glsl/"

static int IGLVERSION = 30; //TODO: decouple this
extern GLuint UNIT_SQUARE;

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

GLuint shader_load(const char *shaderFile)
{
    char vname[256]; 
    char fname[256];
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
    glEnableVertexAttribArray(CW_POSITION);
    glEnableVertexAttribArray(CW_NORMAL);
    glEnableVertexAttribArray(CW_UV);
    glEnableVertexAttribArray(CW_MATERIAL);
    glEnableVertexAttribArray(CW_BONEIDS);
    glEnableVertexAttribArray(CW_BONEWEIGHTS);
    glBindAttribLocation(program, CW_POSITION,      "position");
    glBindAttribLocation(program, CW_NORMAL,        "normal");
    glBindAttribLocation(program, CW_UV,            "uv");
    glBindAttribLocation(program, CW_MATERIAL,      "material");
    glBindAttribLocation(program, CW_BONEIDS,       "boneids");
    glBindAttribLocation(program, CW_BONEWEIGHTS,   "boneweights");
    glBindFragDataLocation(program, CW_OUTCOLOR,    "outColor");
    glBindFragDataLocation(program, CW_OUTNORMAL,   "outNormal");
    glBindFragDataLocation(program, CW_OUTLIGHT,    "outLight");
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
    s->attrib_stride = 0;
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

static void shader_set_block(shader_t *s, char *nm, void *value, size_t sz)
{
    unsigned int location = glGetUniformBlockIndex(s->program, nm);
    if(location == GL_INVALID_INDEX) //ERROR block does not exist
    {
        return;
    }
}

void shader_set_parameter(shader_t *s, char *nm, void *value, size_t sz)
{
    char buf[64];
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    glUseProgram(s->program);
    
    int location = glGetUniformLocation(s->program, nm); 
    GLuint index; //Why is there a distinction between location/index?
    glGetUniformIndices(s->program, 1, (const GLchar * const*)&nm, &index);
    if(location < 0 || location == GL_INVALID_INDEX) // ERROR uniform does not exist
    {
        shader_set_block(s, nm, value, sz); // try with uniform block
        goto CLEANUP;
    }

    GLenum type;
    glGetActiveUniform(s->program, index, 0, NULL, NULL, &type, NULL);

    //void (*uniform_func)(GLint loc, GLsizei count, const GLuint *val) = 0;
    switch(type)
    {
        case GL_FLOAT_VEC4:
            glUniform4fv(location, sz/(sizeof(float) * 4), value);
            break;
        case GL_FLOAT_VEC3:
            glUniform3fv(location, sz/(sizeof(float) * 3), value);
            break;
        case GL_FLOAT_VEC2:
            glUniform2fv(location, sz/(sizeof(float) * 2), value);
            break;
        case GL_FLOAT:
            glUniform1fv(location, sz/sizeof(float), value);
            break;
        case GL_INT_VEC4:
            glUniform4iv(location, (sz/sizeof(int) * 4), value);
            break;
        case GL_INT_VEC3:
            glUniform3iv(location, (sz/sizeof(int) * 3), value);
            break;
        case GL_INT_VEC2:
            glUniform2iv(location, (sz/sizeof(int) * 2), value);
            break;
        case GL_INT:
        case GL_BOOL:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
            glUniform1iv(location, sz/sizeof(int), value);
            break;
        case GL_UNSIGNED_INT_VEC4:
            glUniform4uiv(location, (sz/sizeof(unsigned int) * 4), value);
            break;
        case GL_UNSIGNED_INT_VEC3:
            glUniform3uiv(location, (sz/sizeof(unsigned int) * 3), value);
            break;
        case GL_UNSIGNED_INT_VEC2:
            glUniform2uiv(location, (sz/sizeof(unsigned int) * 2), value);
            break;
        case GL_UNSIGNED_INT:
            glUniform1uiv(location, sz/sizeof(unsigned int), value);
            break;
        case GL_FLOAT_MAT4:
            glUniformMatrix4fv(location, sz/(sizeof(float) * 16), true, value);
            break;
        case GL_FLOAT_MAT3:
            glUniformMatrix3fv(location, sz/(sizeof(float) * 9), true, value);
            break;
        default:
            assert(0 && "invalid parameter type");
    }

CLEANUP:
    glUseProgram(current_program);
}
