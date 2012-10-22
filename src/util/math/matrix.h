/**
 * matrix.h
 * @file    matrix.h
 * glModern
 * @date    January 5, 2012
 * @author  Brandon Surmanski
 */

#ifndef _MATRIX_H
#define _MATRIX_H

#include "vec.h"

#define MAT_ROW_MAJOR 1

#if !MAT_ROW_MAJOR
#define MAT_XX 0
#define MAT_XY 1
#define MAT_XZ 2
#define MAT_XW 3

#define MAT_YX 4
#define MAT_YY 5
#define MAT_YZ 6
#define MAT_YW 7

#define MAT_ZX 8
#define MAT_ZY 9
#define MAT_ZZ 10
#define MAT_ZW 11

#define MAT_WX 12
#define MAT_WY 13
#define MAT_WZ 14
#define MAT_WW 15

#else 

#define MAT_XX 0
#define MAT_XY 4
#define MAT_XZ 8
#define MAT_XW 12

#define MAT_YX 1
#define MAT_YY 5
#define MAT_YZ 9
#define MAT_YW 13

#define MAT_ZX 2
#define MAT_ZY 6
#define MAT_ZZ 10
#define MAT_ZW 14

#define MAT_WX 3
#define MAT_WY 7
#define MAT_WZ 11
#define MAT_WW 15
#endif
///@TODO: allow generic matricies. for all functions
typedef float* mat;
typedef float mat2[4];
typedef float mat3[9];
typedef float mat4[16];

extern const mat4 MAT4_IDENTITY;

void mat4_identity(mat4 mat);
void mat4_set(mat4 m, vec4 x, vec4 y, vec4 z, vec4 w);
void mat4_copy(mat4 a, mat4 b);
void mat4_transpose(mat4 m);
void mat4_scale(mat4 m, float x, float y, float z);
void mat4_translate(mat4 m, float x, float y, float z);
void mat4_rotate(mat4 m, float angle, float x, float y, float z);
void mat4_axisRotate(mat4 m, float rx, float ry, float rz);
void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f);
void mat4_mult(mat4 left, mat4 right);
void mat4_pow(mat4 m, int pow);
void mat4_orient(mat4 m, vec3 up, vec3 fwd);
void mat4_multVec(mat4 m, vec4 vec);
void mat4_print(mat4 m);

void mat4_x(mat4 m, vec4 v);
void mat4_y(mat4 m, vec4 v);
void mat4_z(mat4 m, vec4 v);
void mat4_w(mat4 m, vec4 v);

#endif
