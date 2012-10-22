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
typedef float       vec4[4];
typedef float       quaternion[4];

extern const int X;
extern const int Y;
extern const int Z;
extern const int W;

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
extern const float *VEC2_Y;

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
void    vec2_set(vec2, float x, float y);
void    vec2_copy(const vec2 src, vec2 dst);
float   vec2_dot(const vec2 a, const vec2 b);
void    vec2_add(const vec2 a, const vec2 b, vec2 dest);
void    vec2_sub(const vec2 a, const vec2 b, vec2 dest);
float   vec2_lensq(const vec2 a);
void    vec2_normalize(vec2 a);

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
void    vec3_print(const vec3 a);

// 4 Dimensional floating point vector
void    vec4_set(vec4 a, float x, float y, float z, float w);
void    vec4_copy(const vec4 src, vec4 dst);
void    vec4_add(const vec4 a, const vec4 b, vec4 dest);
void    vec4_sub(const vec4 a, const vec4 b, vec4 dest);
float   vec4_dot(const vec4 a, const vec4 b);
float   vec4_lensq(const vec4 a);
void    vec4_normalize(vec4 a);
void    vec4_scale(vec4 a, const float scale);
void    vec4_proj(const vec4 v, const vec4 refaxis, vec4 dest);
void    vec4_avg(vec4 result, int n, ...);
void    vec4_orth(const vec4 v, const vec4 refaxis, vec4 dest);
void    vec4_swizzle(vec4 a, int x, int y, int z, int w);
void    vec4_print(vec4 a);


//Quaternion vec4 alias functions
extern void  (*quaternion_copy)(const quaternion src, quaternion dst);
extern float (*quaternion_dot)(const quaternion a, const quaternion b);
extern void  (*quaternion_add)(const quaternion a, const quaternion b, quaternion dest);
extern void  (*quaternion_sub)(const quaternion a, const quaternion b, quaternion dest);
extern void  (*quaternion_normalize)(quaternion a);
extern float (*quaternion_lensq)(const quaternion a);

void    quaternion_set(quaternion a, float w, float x, float y, float z);
void    quaternion_set_rotation(quaternion a, float angle, vec3 axis);
void    quaternion_identity(quaternion a);
void    quaternion_rotate(quaternion a, float angle, vec3 axis);
float   quaternion_norm(const quaternion a);
void    quaternion_conjugate(const quaternion a, quaternion dest);
void    quaternion_inverse(const quaternion a, quaternion dest);
void    quaternion_real(const quaternion a, quaternion dest);
void    quaternion_imaginary(const quaternion a, quaternion dest);
void    quaternion_mult(const quaternion a, const quaternion b, quaternion dest);
void    quaternion_div(const quaternion a, const quaternion b, quaternion dest);
void    quaternion_vecRotate(quaternion a, vec4 b);
void    quaternion_orient(quaternion a, vec3 up, vec3 fwd);

#endif
