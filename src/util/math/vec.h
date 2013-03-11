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
typedef int32_t     ivec3[3];

typedef float*      vec;
typedef float       vec2[2];
typedef float       vec3[3];


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
void    hvec3_to_vec3(hvec3 src, vec3 dst);
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

// 3 Dimensional floating point vector
void    vec3_set(vec3 a, float x, float y, float z);
void    vec3_copy(const vec3 src, vec3 dst);
bool    vec3_eq(const vec3 a, const vec3 b);
void    vec3_cmp(const vec3 a, const vec3 b, ivec3 cmp);
float   vec3_dot(const vec3 a, const vec3 b);
float   vec3_lensq(const vec3 a);
float   vec3_len(const vec3 a);
float   vec3_distsq(const vec3 a, const vec3 b);
void    vec3_add(const vec3 a, const vec3 b, vec3 dest);
void    vec3_sub(const vec3 a, const vec3 b, vec3 dest);
void    vec3_normalize(vec3 a);
void    vec3_scale(vec3 a, float val);
void    vec3_cross(const vec3 a, const vec3 b, vec3 dest);
void    vec3_proj(const vec3 v, const vec3 refaxis, vec3 dest);
void    vec3_orth(const vec3 v, const vec3 refaxis, vec3 dest);
void    vec3_avg(vec3 result, int n, ...);
void    vec3_wavg(vec3 result, int n, ...);
void    vec3_lavg(vec3 result, int n, vec *list);
void    vec3_lwavg(vec3 result, int n, vec *list, float *weight);
void    vec3_rand(vec3 res);
void    vec3_print(const vec3 a);

// 4 Dimensional floating point vector

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

#define vec4_set(x, y, z, w) vec4_setp(x,y,z,w)
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

//quaternion vec4 alias functions

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

#endif
