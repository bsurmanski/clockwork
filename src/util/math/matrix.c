/**
 * matrix.c
 * @file   matrix.h
 * glModern
 * @date   January 5, 2012
 * @author Brandon Surmanski
 */

#include <assert.h>
#include <alloca.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "scalar.h"
#include "vec.h"
#include "matrix.h"

void mat3_identity(mat3 m)
{
    m[0] = m[4] = m[8] = 1.0f;
    m[1] = m[2] = 0.0f;
    m[3] = m[5] = 0.0f;
    m[6] = m[7] = 0.0f;
}

void mat3_rotate(mat3 m, float rx, float ry, float rz)
{
    mat3 auxx;
    mat3 auxy;
    mat3 auxz;
    float xc = cos(rx);
    float xs = sin(rx);
    float yc = cos(ry);
    float ys = sin(ry);
    float zc = cos(rz);
    float zs = sin(rz);

    matn_identity(auxx, 3);
    matn_identity(auxy, 3);
    matn_identity(auxz, 3);

    matn_set(auxx, 3, 1, 1, xc);
    matn_set(auxx, 3, 1, 2, xs);
    matn_set(auxx, 3, 2, 1, -xs);
    matn_set(auxx, 3, 2, 2, xc);

    matn_set(auxy, 3, 0, 0, yc);
    matn_set(auxy, 3, 0, 2, -ys);
    matn_set(auxy, 3, 2, 0, ys);
    matn_set(auxy, 3, 2, 2, yc);

    matn_set(auxz, 3, 0, 0, zc);
    matn_set(auxz, 3, 0, 1, zs);
    matn_set(auxz, 3, 1, 0, -zs);
    matn_set(auxz, 3, 1, 1, zc);

    matn_mult(auxz, 3, auxy, auxz);
    matn_mult(auxz, 3, auxx, auxz);
    matn_mult(m, 3, auxz, m);
}

void mat3_translate(mat3 m, float x, float y)
{
    mat3 tMat;
    matn_identity(tMat, 3);
    matn_set(tMat, 3, 2, 0, x);
    matn_set(tMat, 3, 2, 1, y);

    matn_mult(m, 3, tMat, m);
}

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

/**
 * sets a specific entry in matrix 'm'
 */
void mat4_set(mat4 m, int i, int j, float val)
{
    m[4 * i + j] = val;
}

/**
 * sets the matrix through all the vectors. TODO: inconsistant with matn_setv
 */
void mat4_setv(mat4 m, vec4 x, vec4 y, vec4 z, vec4 w)
{
    m[MAT_XX] = x.x;
    m[MAT_XY] = x.y;
    m[MAT_XZ] = x.z;
    m[MAT_XW] = x.w;

    m[MAT_YX] = y.x;
    m[MAT_YY] = y.y;
    m[MAT_YZ] = y.z;
    m[MAT_YW] = y.w;

    m[MAT_ZX] = z.x;
    m[MAT_ZY] = z.y;
    m[MAT_ZZ] = z.z;
    m[MAT_ZW] = z.w;

    m[MAT_WX] = w.x;
    m[MAT_WY] = w.y;
    m[MAT_WZ] = w.z;
    m[MAT_WW] = w.w;
}

/**
 * retrieves a specific entry in the matrix 'm'
 */
float mat4_get(mat4 m, int i, int j)
{
    return m[i * 4 + j];
}

/**
 * retrives a column vector from m, and stores it into 'v'
 */
vec4 mat4_getv(mat4 m, int j)
{
    vec4 ret;
    int i;
    for(i = 0; i < 4; i++)
    {
        ret.v[i] = m[i * 4 + j];
    }
    return ret;
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

    mat4_copy(tmp, m);
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

    mat4_mult(sMat, m, m);
}

void mat4_scalev(mat4 m, float *v)
{
    mat4_scale(m, v[0], v[1], v[2]);
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

    mat4_mult(tMat, m, m);
}

void mat4_translatev(mat4 m, float *v)
{
    mat4_translate(m, v[0], v[1], v[2]);
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

    mat4_mult(aux, m, m);
}

void mat4_rotatev(mat4 m, float angle, float *v)
{
    mat4_rotate(m, angle, v[0], v[1], v[2]);
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

    mat4_mult(auxy, auxz, auxz);
    mat4_mult(auxx, auxz, auxz);
    mat4_mult(auxz, m, m);
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
    mat4_mult(aux, m, m);
}

/**
 * multiplies the left matrix, and the right matrix, and stores in the right matrix
 */
void mat4_mult(mat4 l, mat4 r, mat4 dst)
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

    memcpy(dst, ret, sizeof(mat4));
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
        mat4_mult(m, orig, m); 
    }
}

/**
 * multiplies a vector 'v' by matrix 'm' according to matrix math definitions
 */
void mat4_multVec(mat4 m, vec4 *v)
{
    vec4 temp;
    temp.x = m[MAT_XX] * v->x + m[MAT_YX] * v->y + m[MAT_ZX] * v->z + m[MAT_WX] * v->w;
    temp.y = m[MAT_XY] * v->x + m[MAT_YY] * v->y + m[MAT_ZY] * v->z + m[MAT_WY] * v->w;
    temp.z = m[MAT_XZ] * v->x + m[MAT_YZ] * v->y + m[MAT_ZZ] * v->z + m[MAT_WZ] * v->w;
    temp.w = m[MAT_XW] * v->x + m[MAT_YW] * v->y + m[MAT_ZW] * v->z + m[MAT_WW] * v->w;
    memcpy(v, &temp, sizeof(vec4));
}

/**
 * orients a matrix such that it is aligned with the vectors 'up' and 'fwd'
 * even if 'up' and 'fwd' are not perpindicular, the matrix will still be orthogonal
 */
void mat4_orient(mat4 m_in, vec3 *up, vec3 *fwd)
{
    vec3 s;
    vec3 u;
    vec3 f;
    mat4 m;

    memcpy(&u, up, sizeof(vec3));
    memcpy(&f, fwd, sizeof(vec3));
    vec3_normalizep(&u);
    vec3_normalizep(&f);
    s = vec3_cross(f, u);
    vec3_normalizep(&s);
    u = vec3_cross(s, f);
    vec3_normalizep(&u);

    m[MAT_XX] = s.x; 
    m[MAT_XY] = s.y; 
    m[MAT_XZ] = s.z; 
    m[MAT_XW] = 0;

    m[MAT_YX] = u.x; 
    m[MAT_YY] = u.y; 
    m[MAT_YZ] = u.z; 
    m[MAT_YW] = 0;

    m[MAT_ZX] = -f.x;
    m[MAT_ZY] = -f.y; 
    m[MAT_ZZ] = -f.z; 
    m[MAT_ZW] = 0;

    m[MAT_WX] = 0; 
    m[MAT_WY] = 0; 
    m[MAT_WZ] = 0; 
    m[MAT_WW] = 1;

    mat4_mult(m, m_in, m_in);
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

void mat4_fwdsubstitute(const mat4 const lower, const vec const eq, vec sol)
{
    matn_fwdsubstitute((const matn const) lower, 4, (const vec const) eq, sol);
}

void mat4_bwdsubstitute(const mat4 const upper, const vec4 const *eq, vec4 *sol)
{
    matn_bwdsubstitute((const matn const) upper, 4, (const vec const) eq, (vec) sol);
}

void mat4_lu(mat4 m, mat4 l, mat4 u)
{
    matn_lu(m, 4, l, u);
}

/*
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
}*/


/**
 *
 *
 * NxN dimensional matrix
 *
 *
 */

void matn_init(mat m, int n)
{
    matn_identity(m, n);
}

/**
 * loads the identity matrix into matrix 'm'
 */
void matn_identity(mat m, int n)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            m[i * n + j] = ((float)(i == j));
        }
    }
}

/**
 * component-wise adds two matricies 'a' and 'b', both with a size nxn,
 * and stores the result in 'ret'
 */
void matn_add(matn a, int n, matn b, matn ret)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            matn_set(ret, n, i, j, matn_get(a, n, i, j) + matn_get(b, n, i, j));
        }
    }
}

/**
 * multiplies two matrices 'a' and 'b' and stores the result into 'ret'
 */
void matn_mult(matn a, int n, matn b, matn ret)
{
    matn tmp = alloca(sizeof(float) * n * n);
    int i, j, k;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            float sol_ij = 0.0f;
            for(k = 0; k < n; k++)
            {
                sol_ij += matn_get(a, n, i, k) * matn_get(b, n, k, j);  
            }
            matn_set(tmp, n, i, j, sol_ij);
        }
    }
    memcpy(ret, tmp, sizeof(float) * n * n);
}

/**
 * multiply a vector by m, returning the result into 'ret'
 */
void matn_vmult(matn m, int n, vec v, vec ret)
{
    int i, j;
    for(j = 0; j < n; j++)
    {
        ret[j] = 0.0f;
        for(i = 0; i < n; i++)
        {
            ret[j] += matn_get(m, n, i, j) * v[j];
        }
    }
}

void matn_scale(matn m, int n, float scale)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            matn_set(m, n, i, j, matn_get(m, n, i, j) * scale);
        }
    }
}

void matn_scalev(matn m, int n, float *v)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            matn_set(m, n, i, j, matn_get(m, n, i, j) * v[j]);
        }
    }
}

/**
 * determines of a matrix is lower triangular (all entries above the diagonal are zero)
 */
bool matn_islower(matn a, int n)
{
    bool ret = true;
    int i, j;
    for(j = 0; j < n; j++)
    {
        for(i = j; i < n; i++)
        {
            if(!feq(matn_get(a, n, i, j), 0.0f))
            {
                ret = false;
            }
        }
    }
    return ret;
}

/**
 * determines of a matrix is upper triangular (all entries bellow the diagonal are zero)
 */
bool matn_isupper(matn a, int n)
{
    bool ret = true;
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < i; j++)
        {
            if(!feq(matn_get(a, n, i, j), 0.0f))
            {
                ret = false;
            }
        }
    }
    return ret;
}

/**
 * uses forward substitution to solve a linear equation. of the form "Ax = b",
 * returning x (into the vector 'sol')
 * expects 'lower' to be a lower triangular matrix. representing 'A'
 * eq is a vector representing 'b'
 * sol is a vector representing the solution 'x'
 */
void matn_fwdsubstitute(const matn const lower, int n, const vec const eq, vec sol)
{
    assert(matn_islower(lower, n));

    int i, j;
    for(i = 0; i < n; i++)
    {
        sol[i] = eq[i];
        for(j = 0; j < i; j++)
        {
            sol[i] -= sol[j] * lower[n * i + j];
        }
        assert(!feq(matn_get(lower,n, i, i), 0.0f));
        sol[i] /= matn_get(lower, n, i, i);
    }
}

/**
 * backwards substitution method for solving a set of linear equations of the form "Ax = b"
 *
 */
void matn_bwdsubstitute(const matn const upper, int n, const vec const eq, vec sol)
{
    assert(matn_isupper(upper, n));

    int i, j;
    for(i = n - 1; i >= 0; i--)
    {
        sol[i] = eq[i];
        for(j = i+1; j < n; j++)
        {
            sol[i] -= sol[j] * upper[n * i + j];
        }
        assert(!feq(matn_get(upper, n, i, i), 0.0f));
        sol[i] /= matn_get(upper, n, i, i);
    }
}

/**
 * LU Decomposition using the 'Crout' algorithm
 * TODO: include P matrix, and permutations is case of a diagonal == 0
 */
void matn_lu(matn a, int n, matn l, matn u)
{
    assert(a);
    assert(l);
    assert(u);
    assert(n > 0);

    matn_identity(l, n);
    matn_identity(u, n);
    float alpha;
    int i,j,k;
    for(j = 0; j < n; j++)
    {
        for(i = j; i < n; i++) // lower triangle
        {
            alpha = matn_get(a, n, i, j);
            for(k = 0; k < j; k++)
            {
                alpha -= matn_get(l, n, i, k) * matn_get(u, n, k, j); 
            }
            matn_set(l, n, i, j, alpha); 
        }

        for(i = j; i < n; i++) // upper triangle
        {
            alpha = matn_get(a, n, j, i);
            for(k = 0; k < j; k++)
            {
                alpha -= matn_get(l, n, j, k) * matn_get(u, n, k, i); 
            }

            assert(fabs(matn_get(l, n, j, j)) > FLT_EPSILON && "DET(L) too close to 0");

            matn_set(u, n, j, i, alpha / matn_get(l, n, j, j));
        }
    }
}

/**
 * solves the equation 'Ax = b' for 'x', where 'A' is a square matrix of size n, 
 * and 'x' and 'b' are both vectors of dimension n. this is solved by lu decomposition,
 * and then solving the equations Ly = b, for some unknown vector y found through 
 * forward substitution, and finally Ux = y through back substitution, to get 'x'.
 */
void matn_solvelu(matn a, int n, const vec const b, vec x)
{
    vec y = alloca(n * sizeof(float)); 
    matn l = alloca(n * n *sizeof(float));
    matn u = alloca(n * n * sizeof(float));
    matn_lu(a, n, l, u);
    matn_fwdsubstitute(l, n, b, y); // solve Ly = b
    matn_bwdsubstitute(u, n, y, x); // solve Ux = y
}

/**
 * finds the determinant of the matrix. First, the matrix is decomposed into
 * it's LU triangular matrix decomposition, then the diagonals of the L matrix
 * and multiplied. This works because the determinant of a triangular matrix can
 * simply be found by multiplying the diagonals, and the method of LU decomposition
 * used leaves the U matrix with an identity along the diagonals.
 */
float matn_det(matn m, int n)
{
    float ret = 1.0f;

    matn l = alloca(sizeof(float) * n * n);
    matn u = alloca(sizeof(float) * n * n);
    matn_lu(m, n, l, u);

    int i;
    for(i = 0; i < n; i++)
    {
        ret *= matn_get(u, n, i, i); // NOTE: not need for current method, should be all 1s
        ret *= matn_get(l, n, i, i);       
    }
    return ret;
}

void matn_print(matn m, int n)
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            printf("%f, ", matn_get(m, n, i, j));
        }
        printf("\n");
    }
}

/**
 * retrieves a specific entry in 'm'
 */
float matn_get(matn m, int n, int i, int j)
{
    assert(i < n && i >= 0);
    assert(j < n && i >= 0);

    return m[i * n + j];
}

/**
 * sets a specific entry in 'm'
 */
void matn_set(matn m, int n, int i, int j, float val)
{
    assert(i < n && i >= 0);
    assert(j < n && i >= 0);

    m[i * n + j] = val;
}

/**
 * retrieves a column vector from 'm'.
 */
void matn_getv(matn m, int n, int j, vec v)
{
    assert(j < n && j >= 0);

    int i;
    for(i = 0; i < n; i++)
    {
        v[i] = matn_get(m, n, i, j); 
    }
}

/**
 * sets a column vector in 'm'
 */
void matn_setv(matn m, int n, int j, vec v)
{
    assert(j < n && j >= 0);

    int i;
    for(i = 0; i < n; i++)
    {
        matn_set(m, n, i, j, v[i]);
    }
}
