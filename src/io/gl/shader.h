/**
 * shader.h
 * obj
 * February 15, 2012
 * Brandon Surmanski
 */

#ifndef _SHADER_H
#define _SHADER_H

#include <GL/glfw.h>
#include <GL/gl.h>

#include "util/math/matrix.h"


struct shader_attrib_t;
struct shader_buffer_object_t;
struct shader_fragment_output_t;
struct shader_texture_target_t;
struct shader_constant_t;

enum shader_variable_type
{
    SHADER_BYTE,
    SHADER_UBYTE,
    SHADER_FLOAT,
    SHADER_INT,
    SHADER_UINT,
    SHADER_MAT4,
    SHADER_VEC2,
    SHADER_VEC3,
    SHADER_VEC4
};

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

struct shader_attrib_t {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const GLvoid *ptr;
};

typedef struct shader_attachment_t 
{
    short nbuffers;
    short ntextures;
    struct shader_buffer_object_t *buffers;
    struct shader_texture_target_t *textures;
} shader_attachment_t;


typedef struct Shader {
    GLuint program;
    short nattribs;
    short noutputs;
    short ntexture_targets;
    size_t attrib_stride;
    struct shader_attrib_t *attribs;
    struct shader_fragment_output_t *outputs;
    struct shader_texture_target_t *texture_targets;
    struct shader_attachment_t *bound;
} Shader;

typedef struct Shader shader_t;

GLuint shader_load(const char *filenm);
GLuint shader_loadVersion(const char *filenm, int glversion);

void shader_init(shader_t *s, const char *program);
void shader_finalize(shader_t *s);

void shader_add_attrib(shader_t *s, const char *nm, int sz, GLenum type, //TODO: sz can be implicit
                                    bool norm, int stride, void *ptr); //TODO: stride can be implicit

void shader_add_fragment_output(shader_t *s, const char *nm);
void shader_add_texture_target(shader_t *s, const char *nm, short texture_unit);
void shader_add_uniform(shader_t *s, enum shader_variable_type t, char *nm);

#endif
