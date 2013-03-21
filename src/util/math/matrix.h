/**
 * matrix.h
 * @file    matrix.h
 * glModern
 * @date    January 5, 2012
 * @author  Brandon Surmanski
 */

#ifndef _MATRIX_H
#define _MATRIX_H

#include <stdbool.h>

#include "const.h"
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
typedef float* matn;
typedef float* matnxn;
typedef float mat2[4];
typedef float mat3[9];
typedef float mat4[16];

void mat3_rotate(mat3 m, float x, float y, float z);
void mat3_translate(mat3 m, float x, float y);

void mat4_identity(mat4 mat);
void mat4_set(mat4 m, int i, int j, float val);
void mat4_setv(mat4 m, vec4 x, vec4 y, vec4 z, vec4 w);
float mat4_get(mat4 m, int i, int j);
vec4 mat4_getv(mat4 m, int i);

void mat4_copy(mat4 a, mat4 b);
void mat4_transpose(mat4 m);
void mat4_scale(mat4 m, float x, float y, float z);
void mat4_scalev(mat4 m, float *v);
void mat4_translate(mat4 m, float x, float y, float z);
void mat4_translatev(mat4 m, float *v);
void mat4_rotate(mat4 m, float angle, float x, float y, float z);
void mat4_rotatev(mat4 m, float angle, float *v);
void mat4_axisRotate(mat4 m, float rx, float ry, float rz);
void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f);
//TODO:mat4_ortho(mat4 m, float l, float r, float b, float t, float n, float f);
void mat4_mult(mat4 left, mat4 right, mat4 dst);
void mat4_pow(mat4 m, int pow);
void mat4_orient(mat4 m, vec3 *up, vec3 *fwd);
void mat4_multVec(mat4 m, vec4 *v);
void mat4_print(mat4 m);

void mat4_lu(mat4 m, mat4 l, mat4 u);
void mat4_fwdsubstitute(const mat4 const lower, const vec const b, vec x);
void mat4_bwdsubstitute(const mat4 const upper, const vec4 const *b, vec4 *x);
void mat4_ludecompose(mat4 m, mat4 l, mat4 u);

void mat4_x(mat4 m, vec4 v);
void mat4_y(mat4 m, vec4 v);
void mat4_z(mat4 m, vec4 v);
void mat4_w(mat4 m, vec4 v);

/*
 * NxN dimensional Matricies
 */

void matn_init(mat m, int n);
void matn_identity(mat m, int n);
void matn_add(matn a, int n, matn b, matn ret);
void matn_mult(matn a, int n, matn b, matn ret);
void matn_vmult(matn m, int n, vec v, vec ret);
void matn_scale(matn a, int n, float scale);
void matn_scalev(matn a, int n, float *v);
bool matn_islower(matn a, int n);
bool matn_isupper(matn a, int n);
void matn_fwdsubstitute(const matn const lower, int n, const vec const b, vec x);
void matn_bwdsubstitute(const matn const upper, int n, const vec const b, vec x);
void matn_lu(matn m, int n, matn l, matn u);
void matn_solvelu(matn a, int n, const vec const b, vec x);
float matn_det(matn m, int n);
void matn_print(matn m, int n);

float matn_get(matn m, int n, int i, int j);
void matn_set(matn m, int n, int i, int j, float val);
void matn_getv(matn m, int n, int i, vec v);
void matn_setv(matn m, int n, int j, vec v);

#endif
