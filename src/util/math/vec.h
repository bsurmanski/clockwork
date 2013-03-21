/**
 * vec.h
 * @file    vec.h
 * heightmap
 * @date    November 04, 2011
 * @author  Brandon Surmanski
 */

#ifndef _VEC_H_
#define _VEC_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "const.h"

#define EPSILON FLT_EPSILON

typedef uint16_t*   uhvec;
typedef uint16_t    uhvec2[2];
typedef uint16_t    uhvec3[3];

typedef int16_t*    hvec;
typedef int16_t     hvec2[2];
typedef int16_t     hvec3[3];

typedef int32_t*    ivec;
typedef int32_t     ivec2[2];
//typedef int32_t     ivec3[3];

typedef float*      vec;
typedef float       vec2[2];
//typedef float       vec3[3];

typedef union ivec3
{
    struct 
    {
        int32_t x;
        int32_t y;
        int32_t z;
    };

    struct 
    {
        int32_t r;
        int32_t g;
        int32_t b;
        int32_t a;
    };

    int32_t v[3];
} ivec3;

typedef union vec3
{
    struct 
    {
        float x;
        float y;
        float z;
    };

    struct 
    {
        float r;
        float g;
        float b;
        float a;
    };

    float v[3];
} vec3;

typedef union vec4 {
    struct 
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct 
    {
        float r;
        float g;
        float b;
        float a;
    };

    float v[4];
} vec4;

typedef vec4 quaternion;
typedef vec4 quat;

/*
extern const float VEC4_ZERO[];
extern const float VEC4_ONE[];
extern const float VEC4_X[];
extern const float VEC4_Y[];
extern const float VEC4_Z[];
extern const float VEC4_W[];

extern const float *VEC3_ZERO;
extern const float *VEC3_ONE;
extern const float *VEC3_X;
extern const float *VEC3_Y;
extern const float *VEC3_Z;

extern const float *VEC2_ZERO;
extern const float *VEC2_ONE;
extern const float *VEC2_X;
extern const float *VEC2_Y;*/

// 2 Dimensional half precision signed integer vectors
void    hvec2_to_vec2(hvec2 src, vec2 dst);
void    vec2_to_hvec2(vec2 src, hvec2 dst);
void    hvec2_set(hvec2 a, int16_t x, int16_t y);
bool    hvec2_eq(hvec2 a, hvec2 b);

// 2 Dimensional half precision unsigned integer vector
void    uhvec2_to_vec2(uhvec2 src, vec2 dst);
void    vec2_to_uhvec2(vec2 src, uhvec2 dst);
void    uhvec2_set(uhvec2 a, uint16_t x, uint16_t y);
bool    uhvec2_eq(uhvec2 a, uhvec2 b);

// 3 Dimensional half precision signed integer vectors
vec3    hvec3_to_vec3(hvec3 src);
void    vec3_to_hvec3(vec3 src, hvec3 dst);
void    hvec3_set(hvec3 a, int16_t x, int16_t y, int16_t z);
bool    hvec3_eq(hvec3 a, hvec3 b);

// 2 Dimensional floating point vector
void    vec2_set(vec2 a, float x, float y);
void    vec2_scale(vec2 a, float scale);
void    vec2_copy(const vec2 src, vec2 dst);
float   vec2_dot(const vec2 a, const vec2 b);
void    vec2_add(const vec2 a, const vec2 b, vec2 dest);
void    vec2_sub(const vec2 a, const vec2 b, vec2 dest);
float   vec2_lensq(const vec2 a);
void    vec2_normalize(vec2 a);
void    vec2_rand(vec2 res);

/*{{{ 3 Dimensional floating point vectors */
vec3    vec3_setp(float x, float y, float z);
vec3    vec3_copyp(const vec3 *src);
bool    vec3_eqp(const vec3 *a, const vec3 *b);
ivec3   vec3_cmpp(const vec3 *a, const vec3 *b);
float   vec3_dotp(const vec3 *a, const vec3 *b);
float   vec3_lensqp(const vec3 *a);
float   vec3_lenp(const vec3 *a);
float   vec3_distsqp(const vec3 *a, const vec3 *b);
vec3    vec3_addp(const vec3 *a, const vec3 *b);
vec3    vec3_subp(const vec3 *a, const vec3 *b);
void    vec3_normalizep(vec3 *a);
void    vec3_scalep(vec3 *a, float val);
vec3    vec3_crossp(const vec3 *a, const vec3 *b);
vec3    vec3_projp(const vec3 *v, const vec3 *refaxis);
vec3    vec3_orthp(const vec3 *v, const vec3 *refaxis);
vec3    vec3_avgp(int n, ...);
vec3    vec3_wavgp(int n, ...);
vec3    vec3_lavgp(int n, vec *list);
vec3    vec3_lwavgp(int n, vec *list, float *weight);
vec3    vec3_randp(void);
void    vec3_printp(const vec3 *a);

#define vec3_set(x, y, z)   vec3_setp((x), (y), (z));
#define vec3_copy(src)      vec3_copyp(&(src));
#define vec3_eq(a, b)       vec3_eqp(&(a), &(b));
#define vec3_cmp(a, b)      vec3_cmpp(&(a), &(b));
#define vec3_dot(a, b)      vec3_dotp(&(a), &(b));
#define vec3_lensq(a)       vec3_lensqp(&(a));
#define vec3_len(a)         vec3_lenp(&(a));
#define vec3_distsq(a, b)   vec3_distsqp(&(a), &(b));
#define vec3_add(a, b)      vec3_addp(&(a), &(b));
#define vec3_sub(a, b)      vec3_subp(&(a), &(b));
#define vec3_normalize(a)   vec3_normalizep(&(a));
#define vec3_scale(a, val)  vec3_scalep(&(a), (val));
#define vec3_cross(a, b)    vec3_crossp(&(a), &(b));
#define vec3_proj(v, axis)  vec3_projp(&(v), &(axis));
#define vec3_orth(v, axis)  vec3_orthp(&(v), &(axis));
#define vec3_avg(n, ...)    vec3_avgp(n, __VA_ARGS__);
#define vec3_wavg(n, ...)   vec3_wavgp(n, __VA_ARGS__);
#define vec3_lavg(n, list)  vec3_lavgp(n, list);
#define vec3_lwavg(n, list, weight) vec3_lwavgp(n, list, weight);
#define vec3_rand()         vec3_randp()
#define vec3_print(a)       vec3_printp(&(a))
/*}}}*/

/*{{{ 4 Dimensional floating point vectors */

vec4    vec4_setp(float x, float y, float z, float w);
vec4    vec4_copyp(const vec4 *src);
vec4    vec4_addp(const vec4 *a, const vec4 *b);
vec4    vec4_subp(const vec4 *a, const vec4 *b);
float   vec4_dotp(const vec4 *a, const vec4 *b);
float   vec4_lensqp(const vec4 *a);
void    vec4_normalizep(vec4 *a);
vec4    vec4_normalizedp(vec4 *a);
void    vec4_scalep(vec4 *a, const float scale);
vec4    vec4_projp(const vec4 *v, const vec4 *refaxis);
vec4    vec4_orthp(const vec4 *v, const vec4 *refaxis);
void    vec4_printp(vec4 *a);
//void    vec4_swizzlep(vec4 a, int x, int y, int z, int w);
vec4    vec4_avg(int n, ...); //XXX: cannot make avgp?

#define vec4_set(x, y, z, w) vec4_setp((x),(y),(z),(w))
#define vec4_copy(src) vec4_copyp(&(src))
#define vec4_add(a, b) vec4_addp(&(a), &(b))
#define vec4_sub(a, b) vec4_subp(&(a), &(b))
#define vec4_dot(a, b) vec4_dotp(&(a), &(b))
#define vec4_lensq(a) vec4_lensqp(&(a))
#define vec4_normalize(a) vec4_normalizep(&(a))
#define vec4_normalized(a) vec4_normalizedp(&(a))
#define vec4_scale(a, scale) vec4_scalep(&(a), scale)
#define vec4_proj(v, refaxis) vec4_projp(&(v), &(refaxis))
#define vec4_orth(v, refaxis) vec4_orthp(&(v), &(refaxis))
#define vec4_print(a) vec4_printp(&(a))
/*}}}*/

/*{{{ quaternion vec4 alias functions*/

extern quat  (*quaternion_copyp)(const quat *src);
extern float (*quaternion_dotp)(const quat *a, const quat *b);
extern quat  (*quaternion_addp)(const quat *a, const quat *b);
extern quat  (*quaternion_subp)(const quat *a, const quat *b);
extern void  (*quaternion_normalizep)(quat *a);
extern float (*quaternion_lensqp)(const quat *a);

quat    quaternion_identity(void);
quat    quaternion_set(float w, float x, float y, float z);
quat    quaternion_set_rotation(float angle, vec3 axis);

quat    quaternion_lerpp(quat *a, quat *b, float t);
quat    quaternion_slerpp(quat *a, quat *b, float t);
void    quaternion_rotatep(quat *a, float angle, vec3 axis);
float   quaternion_normp(const quat *a);
quat    quaternion_conjugatep(const quat *a);
quat    quaternion_inversep(const quat *a);
quat    quaternion_realp(const quat *a);
quat    quaternion_imaginaryp(const quat *a);
quat    quaternion_multp(const quat *a, const quat *b);
quat    quaternion_divp(const quat *a, const quat *b);
quat    quaternion_vecRotatep(quat *a, vec4 *b);
quat    quaternion_orientp(vec3 up, vec3 fwd);

#define quaternion_copy(src)    quaternion_copyp(&(src))
#define quaternion_dot(a, b)    quaternion_dotp(&(a), &(b))
#define quaternion_add(a, b)    quaternion_add(&(a), &(b)
#define quaternion_sub(a, b)    quaternion_sub(&(a), &(b)
#define quaternion_normalize(a) quaternion_normalize(&(a))
#define quaternion_lensq(a)     quaternion_lensq(&(a))

#define quaternion_lerp(a, b, t)            quaternion_lerpp(&(a), &(b), t);
#define quaternion_slerp(a, b, t)           quaternion_slerpp(&(a), &(b), t);
#define quaternion_rotate(a, angle, axis)   quaternion_rotatep(&(a), angle, axis);
#define quaternion_norm(a)                  quaternion_normp(&(a));
#define quaternion_conjugate(a)             quaternion_conjugatep(&(a));
#define quaternion_inverse(a)               quaternion_inversep(&(a));
#define quaternion_real(a)                  quaternion_realp(&(a));
#define quaternion_imaginary(a)             quaternion_imaginaryp(&(a));
#define quaternion_mult(a, b)               quaternion_multp(&(a), &(b));
#define quaternion_div(a, b)                quaternion_divp(&(a), &(b));
#define quaternion_vecRotate(a, b)          quaternion_vecRotatep(&(a), &(b));
#define quaternion_orient(a, up, fwd)       quaternion_orientp(&(a), up, fwd);
/*}}}*/

#endif
