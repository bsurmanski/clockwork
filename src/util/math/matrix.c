/**
 * matrix.c
 * @file   matrix.h
 * glModern
 * @date   January 5, 2012
 * @author Brandon Surmanski
 */

#include <assert.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "scalar.h"
#include "vec.h"
#include "matrix.h"

///Constant identity matrix
const mat4 MAT4_IDENTITY = 
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

/**
 * Assigns the identity matrix to 'm'
 */
void mat4_identity(mat4 m)
{
    m[MAT_XX] = m[MAT_YY] = m[MAT_ZZ] = m[MAT_WW] = 1.0f;
    m[MAT_XY] = m[MAT_XZ] = m[MAT_XW] = 0.0f;
    m[MAT_YX] = m[MAT_YZ] = m[MAT_YW] = 0.0f;
    m[MAT_ZX] = m[MAT_ZY] = m[MAT_ZW] = 0.0f;
    m[MAT_WX] = m[MAT_WY] = m[MAT_WZ] = 0.0f;
}

void mat4_set(mat4 m, vec4 x, vec4 y, vec4 z, vec4 w)
{
    m[MAT_XX] = x[X];
    m[MAT_XY] = x[Y];
    m[MAT_XZ] = x[Z];
    m[MAT_XW] = x[W];

    m[MAT_YX] = y[X];
    m[MAT_YY] = y[Y];
    m[MAT_YZ] = y[Z];
    m[MAT_YW] = y[W];

    m[MAT_ZX] = z[X];
    m[MAT_ZY] = z[Y];
    m[MAT_ZZ] = z[Z];
    m[MAT_ZW] = z[W];

    m[MAT_WX] = w[X];
    m[MAT_WY] = w[Y];
    m[MAT_WZ] = w[Z];
    m[MAT_WW] = w[W];
}

/**
 * copys matrix a to b, without modifying matrix a
 */
void mat4_copy(mat4 a, mat4 b)
{
    memcpy(b, a, sizeof(mat4));
}

/**
 * transposes the matrix m
 */
void mat4_transpose(mat4 m)
{
    mat4 tmp;
    tmp[MAT_XX] = m[MAT_XX];
    tmp[MAT_XY] = m[MAT_YX];
    tmp[MAT_XZ] = m[MAT_ZX];
    tmp[MAT_XW] = m[MAT_WX];

    tmp[MAT_YX] = m[MAT_XY];
    tmp[MAT_YY] = m[MAT_YY];
    tmp[MAT_YZ] = m[MAT_ZY];
    tmp[MAT_YW] = m[MAT_WY];

    tmp[MAT_ZX] = m[MAT_XZ];
    tmp[MAT_ZY] = m[MAT_YZ];
    tmp[MAT_ZZ] = m[MAT_ZZ];
    tmp[MAT_ZW] = m[MAT_WZ];

    tmp[MAT_WX] = m[MAT_XW];
    tmp[MAT_WY] = m[MAT_YW];
    tmp[MAT_WZ] = m[MAT_ZW];
    tmp[MAT_WW] = m[MAT_WW];

    vec4_copy(tmp, m);
}

/**
 * applys a scale operation to the current matrix
 */
void mat4_scale(mat4 m, float sx, float sy, float sz)
{
    mat4 sMat;
    mat4_identity(sMat);
    sMat[MAT_XX] = sx;
    sMat[MAT_YY] = sy;
    sMat[MAT_ZZ] = sz;

    mat4_mult(sMat, m);
}

/**
 * applys a translation operation to the current matrix
 */
void mat4_translate(mat4 m, float dx, float dy, float dz)
{
    mat4 tMat;
    mat4_identity(tMat);
    tMat[MAT_WX] = dx;
    tMat[MAT_WY] = dy;
    tMat[MAT_WZ] = dz;

    mat4_mult(tMat, m);
}

/**
 * creates a rotation matrix (with rotation around an arbirary axis [xyz], 
 * and left multiplies m by the result
 */
void mat4_rotate(mat4 m, float angle, float rx, float ry, float rz)
{
    mat4 aux;
    if(fabs(angle) < FLT_EPSILON) return;
    float len = sqrt(rx*rx + ry*ry + rz*rz);
    if(len < FLT_EPSILON) return;
    rx/=len, ry/=len, rz/=len; // normailize vector

    float c = cos(angle);
    float t = 1.0f - c;
    float s = sin(angle);

    //arbitrary  axis rotation matrix
    aux[MAT_XX] = t * rx * rx + c;
    aux[MAT_YX] = t * rx * ry - s * rz;
    aux[MAT_ZX] = t * rx * rz + s * ry;
    aux[MAT_WX] = aux[MAT_WY] = aux[MAT_WZ] = 0.0f;
    aux[MAT_XY] = t * rx * ry + s * rz;
    aux[MAT_YY] = t * ry * ry + c;
    aux[MAT_ZY] = t * ry * rz - s * rx;
    aux[MAT_XZ] = t * rx * rz - s * ry;
    aux[MAT_YZ] = t * ry * rz + s * rx;
    aux[MAT_ZZ] = t * rz * rz + c;
    aux [MAT_XW] = aux[MAT_YW] = aux[MAT_ZW] = 0.0f;
    aux[MAT_WW] = 1.0f;

    mat4_mult(aux, m);
}

/**
 * rotates a matrix about the identity axies.
 * the rotations occur in the reverse order (z -> y -> x)
 * @param m the matrix to rotate
 * @param rx the angle to rotate about the x axis
 * @param ry the angle to rotate about the y axis
 * @param rz the angle to rotate about the z axis
 */
void mat4_axisRotate(mat4 m, float rx, float ry, float rz)
{
    mat4 auxx;
    mat4 auxy;
    mat4 auxz;
    float xc = cos(rx);
    float xs = sin(rx);
    float yc = cos(ry);
    float ys = sin(ry);
    float zc = cos(rz);
    float zs = sin(rz);

    mat4_identity(auxx);
    mat4_identity(auxy);
    mat4_identity(auxz);

    auxx[MAT_YY] = xc;
    auxx[MAT_ZY] = xs;
    auxx[MAT_YZ] = -xs;
    auxx[MAT_ZZ] = xc;

    auxy[MAT_XX] = yc;
    auxy[MAT_ZX] = -ys;
    auxy[MAT_XZ] = ys;
    auxy[MAT_ZZ] = yc;

    auxz[MAT_XX] = zc;
    auxz[MAT_YX] = zs;
    auxz[MAT_XY] = -zs;
    auxz[MAT_YY] = zc;

    mat4_mult(auxy, auxz);
    mat4_mult(auxx, auxz);
    mat4_mult(auxz, m);
}

/**
 * applys a frustum projection matrix to the current matrix.
 */
void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f)
{
    mat4 aux;
    aux[MAT_XX] = 2.0f * n / (r - l);
    aux[MAT_YX] = 0.0f;
    aux[MAT_ZX] = (r + l) / (r - l);
    aux[MAT_WX] = 0.0f;

    aux[MAT_XY] = 0.0f;
    aux[MAT_YY] = (2.0f * n) / (t - b);
    aux[MAT_ZY] = (t + b) / (t - b);
    aux[MAT_WY] = 0.0f;

    aux[MAT_XZ] = 0.0f;
    aux[MAT_YZ] = 0.0f;
    aux[MAT_ZZ] = -(f + n)/(f - n);
    aux[MAT_WZ] = -(f * n) / (f - n);

    aux[MAT_XW] = 0.0f;
    aux[MAT_YW] = 0.0f;
    aux[MAT_ZW] = -1.0f;
    aux[MAT_WW] = 0.0f;
    mat4_mult(aux, m);
}

/**
 * multiplies the left matrix, and the right matrix, and stores in the right matrix
 */
void mat4_mult(mat4 l, mat4 r)
{
    mat4 ret;

     ret[MAT_XX] = l[MAT_XX] * r[MAT_XX] + l[MAT_YX] * r[MAT_XY] + 
                l[MAT_ZX] * r[MAT_XZ] + l[MAT_WX] * r[MAT_XW];
     ret[MAT_XY] = l[MAT_XY] * r[MAT_XX] + l[MAT_YY] * r[MAT_XY] + 
                l[MAT_ZY] * r[MAT_XZ] + l[MAT_WY] * r[MAT_XW];
     ret[MAT_XZ] = l[MAT_XZ] * r[MAT_XX] + l[MAT_YZ] * r[MAT_XY] + 
                l[MAT_ZZ] * r[MAT_XZ] + l[MAT_WZ] * r[MAT_XW];
     ret[MAT_XW] = l[MAT_XW] * r[MAT_XX] + l[MAT_YW] * r[MAT_XY] + 
                l[MAT_ZW] * r[MAT_XZ] + l[MAT_WW] * r[MAT_XW];

     ret[MAT_YX] = l[MAT_XX] * r[MAT_YX] + l[MAT_YX] * r[MAT_YY] + 
                l[MAT_ZX] * r[MAT_YZ] + l[MAT_WX] * r[MAT_YW];
     ret[MAT_YY] = l[MAT_XY] * r[MAT_YX] + l[MAT_YY] * r[MAT_YY] + 
                l[MAT_ZY] * r[MAT_YZ] + l[MAT_WY] * r[MAT_YW];
     ret[MAT_YZ] = l[MAT_XZ] * r[MAT_YX] + l[MAT_YZ] * r[MAT_YY] + 
                l[MAT_ZZ] * r[MAT_YZ] + l[MAT_WZ] * r[MAT_YW];
     ret[MAT_YW] = l[MAT_XW] * r[MAT_YX] + l[MAT_YW] * r[MAT_YY] + 
                l[MAT_ZW] * r[MAT_YZ] + l[MAT_WW] * r[MAT_YW];

     ret[MAT_ZX] = l[MAT_XX] * r[MAT_ZX] + l[MAT_YX] * r[MAT_ZY] + 
                l[MAT_ZX] * r[MAT_ZZ] + l[MAT_WX] * r[MAT_ZW];
     ret[MAT_ZY] = l[MAT_XY] * r[MAT_ZX] + l[MAT_YY] * r[MAT_ZY] + 
                l[MAT_ZY] * r[MAT_ZZ] + l[MAT_WY] * r[MAT_ZW];
     ret[MAT_ZZ] = l[MAT_XZ] * r[MAT_ZX] + l[MAT_YZ] * r[MAT_ZY] + 
                l[MAT_ZZ] * r[MAT_ZZ] + l[MAT_WZ] * r[MAT_ZW];
     ret[MAT_ZW] = l[MAT_XW] * r[MAT_ZX] + l[MAT_YW] * r[MAT_ZY] + 
                l[MAT_ZW] * r[MAT_ZZ] + l[MAT_WW] * r[MAT_ZW];

     ret[MAT_WX] = l[MAT_XX] * r[MAT_WX] + l[MAT_YX] * r[MAT_WY] + 
                l[MAT_ZX] * r[MAT_WZ] + l[MAT_WX] * r[MAT_WW];
     ret[MAT_WY] = l[MAT_XY] * r[MAT_WX] + l[MAT_YY] * r[MAT_WY] + 
                l[MAT_ZY] * r[MAT_WZ] + l[MAT_WY] * r[MAT_WW];
     ret[MAT_WZ] = l[MAT_XZ] * r[MAT_WX] + l[MAT_YZ] * r[MAT_WY] + 
                l[MAT_ZZ] * r[MAT_WZ] + l[MAT_WZ] * r[MAT_WW];
     ret[MAT_WW] = l[MAT_XW] * r[MAT_WX] + l[MAT_YW] * r[MAT_WY] + 
                l[MAT_ZW] * r[MAT_WZ] + l[MAT_WW] * r[MAT_WW];

    memcpy(r, ret, sizeof(mat4));
}

/**
 * exponentiates the matrix. Only valid for positive powers
 */
void mat4_pow(mat4 m, int pow)
{
    assert(pow >= 0);

    mat4 orig;
    mat4_copy(m, orig);
    mat4_identity(m);
    while(pow--)
    {
        mat4_mult(m, orig); 
    }
}

/**
 * multiplies a vector 'v' by matrix 'm' according to matrix math definitions
 */
void mat4_multVec(mat4 m, vec4 v)
{
    vec4 temp;
    temp[X] = m[MAT_XX] * v[X] + m[MAT_YX] * v[Y] + m[MAT_ZX] * v[Z] + m[MAT_WX] * v[W];
    temp[Y] = m[MAT_XY] * v[X] + m[MAT_YY] * v[Y] + m[MAT_ZY] * v[Z] + m[MAT_WY] * v[W];
    temp[Z] = m[MAT_XZ] * v[X] + m[MAT_YZ] * v[Y] + m[MAT_ZZ] * v[Z] + m[MAT_WZ] * v[W];
    temp[W] = m[MAT_XW] * v[X] + m[MAT_YW] * v[Y] + m[MAT_ZW] * v[Z] + m[MAT_WW] * v[W];
    memcpy(v, temp, sizeof(vec4));
}

/**
 * orients a matrix such that it is aligned with the vectors 'up' and 'fwd'
 * even if 'up' and 'fwd' are not perpindicular, the matrix will still be orthogonal
 */
void mat4_orient(mat4 m_in, vec3 up, vec3 fwd)
{
    vec3 s;
    vec3 u;
    vec3 f;
    mat4 m;

    memcpy(u, up, sizeof(vec3));
    memcpy(f, fwd, sizeof(vec3));
    vec3_normalize(u);
    vec3_normalize(f);
    vec3_cross(f, u, s);
    vec3_normalize(s);
    vec3_cross(s, f, u);
    vec3_normalize(u);

    m[MAT_XX] = s[X]; 
    m[MAT_XY] = s[Y]; 
    m[MAT_XZ] = s[Z]; 
    m[MAT_XW] = 0;

    m[MAT_YX] = u[X]; 
    m[MAT_YY] = u[Y]; 
    m[MAT_YZ] = u[Z]; 
    m[MAT_YW] = 0;

    m[MAT_ZX] = -f[X];
    m[MAT_ZY] = -f[Y]; 
    m[MAT_ZZ] = -f[Z]; 
    m[MAT_ZW] = 0;

    m[MAT_WX] = 0; 
    m[MAT_WY] = 0; 
    m[MAT_WZ] = 0; 
    m[MAT_WW] = 1;

    mat4_mult(m, m_in);
    //memcpy(mat, m, sizeof(mat4));
}

/**
 * prints out a matrix to STDOUT. useful for debugging
 */
void mat4_print(mat4 m)
{
    printf("%f, %f, %f, %f\n", m[MAT_XX], m[MAT_YX], m[MAT_ZX], m[MAT_WX]);
    printf("%f, %f, %f, %f\n", m[MAT_XY], m[MAT_YY], m[MAT_ZY], m[MAT_WY]);
    printf("%f, %f, %f, %f\n", m[MAT_XZ], m[MAT_YZ], m[MAT_ZZ], m[MAT_WZ]);
    printf("%f, %f, %f, %f\n", m[MAT_XW], m[MAT_YW], m[MAT_ZW], m[MAT_WW]);
    printf("\n");
}

void mat4_x(mat4 m, vec4 v)
{
    vec4_set(v, m[MAT_XX], m[MAT_XY], m[MAT_XZ], m[MAT_XW]);
}

void mat4_y(mat4 m, vec4 v)
{
    vec4_set(v, m[MAT_YX], m[MAT_YY], m[MAT_YZ], m[MAT_YW]);
}

void mat4_z(mat4 m, vec4 v)
{
    vec4_set(v, m[MAT_ZX], m[MAT_ZY], m[MAT_ZZ], m[MAT_ZW]);
}

void mat4_w(mat4 m, vec4 v)
{
    vec4_set(v, m[MAT_WX], m[MAT_WY], m[MAT_WZ], m[MAT_WW]);
}
