/**
 * vec.c
 * @file    vec.h
 * obj
 * @date    January 15, 2012
 * @author  Brandon Surmanski
 */

#include <math.h>
#include <float.h>
#include <string.h> //needed for matrix mult mempcy
#include <stdio.h>

#include "util/random.h"
#include "convert.h"
#include "scalar.h"

#include "vec.h"

const float VEC4_ZERO[] = {0.0f, 0.0f, 0.0f, 0.0f};
const float VEC4_ONE[] = {1.0f, 1.0f, 1.0f, 1.0f};
const float VEC4_X[] = {1.0f, 0.0f, 0.0f, 0.0f};
const float VEC4_Y[] = {0.0f, 1.0f, 0.0f, 0.0f};
const float VEC4_Z[] = {0.0f, 0.0f, 1.0f, 0.0f};
const float VEC4_W[] = {0.0f, 0.0f, 0.0f, 1.0f};

const float *VEC3_ZERO = VEC4_ZERO;
const float *VEC3_ONE = VEC4_ONE;
const float *VEC3_X = VEC4_X;
const float *VEC3_Y = VEC4_Y;
const float *VEC3_Z = VEC4_Z;

const float *VEC2_ZERO = VEC4_ZERO;
const float *VEC2_ONE = VEC4_ONE;
const float *VEC2_X = VEC4_X;
const float *VEC2_Y = VEC4_Y;

/*
 ****************************************************
 * 2 DIMENSIONAL HALF PRECISION SIGNED INTEGER VECTOR
 ****************************************************
 */

/**
 * converts a 2 dimensional 16-bit integer vector to a floating point vector
 */
void hvec2_to_vec2(hvec2 src, vec2 dst)
{
    dst[X] = htof(src[X]);
    dst[Y] = htof(src[Y]);
}

/**
 * converts a 2 dimensional floating point vector to a 16-bit signed vector
 */
void vec2_to_hvec2(vec2 src, hvec2 dst)
{
    dst[X] = ftoh(src[X]);
    dst[Y] = ftoh(src[Y]);
}

/**
 * sets the value of a 2 dimensional 16-bit vector
 */
void hvec2_set(hvec2 a, int16_t x, int16_t y)
{
    a[X] = x;
    a[Y] = y;
}
/**
 * 16-bit unsigned 2 dimensional vector equality
 * @returns boolean equality of a == b (component wise)
 */
bool hvec2_eq(hvec2 a, hvec2 b)
{
    return a[X] == b[X] && a[Y] == b[Y];
}

/*
 ******************************************************
 * 2 DIMENSIONAL HALF PRECISION UNSIGNED INTEGER VECTOR
 ******************************************************
 */

/**
 * converts a 2 dimensional 16-bit integer vector to a floating point vector
 */
void uhvec2_to_vec2(uhvec2 src, vec2 dst)
{
    dst[X] = uhtof(src[X]);
    dst[Y] = uhtof(src[Y]);
}

/**
 * converts a 2 dimensional floating point vector to a 16-bit signed vector
 */
void vec2_to_uhvec2(vec2 src, uhvec2 dst)
{
    dst[X] = ftouh(src[X]);
    dst[Y] = ftouh(src[Y]);
}

/**
 * sets the value of a 2 dimensional 16-bit vector
 */
void uhvec2_set(uhvec2 a, uint16_t x, uint16_t y)
{
    a[X] = x;
    a[Y] = y;
}

/**
 * 16-bit signed 2 dimensional vector equality
 * @returns boolean equality of a == b (component wise)
 */
bool uhvec2_eq(uhvec2 a, uhvec2 b)
{
    return a[X] == b[X] && a[Y] == b[Y];
}

/*
 ****************************************************
 * 3 DIMENSIONAL HALF PRECISION SIGNED INTEGER VECTOR
 ****************************************************
 */

/**
 * converts a 3 dimensional 16-bit integer vector to a floating point vector
 */
void hvec3_to_vec3(hvec3 src, vec3 dst)
{
    dst[X] = src[X] / 65535.0f;
    dst[Y] = src[Y] / 65535.0f;
    dst[Z] = src[Z] / 65535.0f;
}

/**
 * converts a 3 dimensional floating point vector to a 16-bit integer vector
 * @see ftoh
 */
void vec3_to_hvec3(vec3 src, hvec3 dst)
{
    dst[X] = ftoh(src[X]);
    dst[Y] = ftoh(src[Y]);
    dst[Z] = ftoh(src[Z]);
}

/**
 * sets the value of a 3 dimensional 16-bit integer vector
 */
void hvec3_set(hvec3 a, int16_t x, int16_t y, int16_t z)
{
    a[X] = x;
    a[Y] = y;
    a[Z] = z;
}

/**
 * test the equality of 2 16-bit unsigned 3 dimensional integer vectors
 */
bool hvec3_eq(hvec3 a, hvec3 b)
{
    return a[X] == b[X] && a[Y] == b[Y] && a[Z] == b[Z];
}

/**
 *************************************
 * 2 DIMENSIONAL FLOATING POINT VECTOR
 *************************************
 */

/**
 * Sets the value of a 2 Dimensional vector x, y 
 */
void vec2_set(vec2 a, float x, float y)
{
    a[X] = x;
    a[Y] = y;
}

void vec2_scale(vec2 a, float scale)
{
    a[X] *= scale;
    a[Y] *= scale;
}

/**
 * copys the value of src to dest, component-wise
 */
void vec2_copy(const vec2 src, vec2 dst)
{
    dst[X] = src[X];
    dst[Y] = src[Y];
}

/**
 * 2 dimensional dot-product
 */
float vec2_dot(const vec2 a, const vec2 b)
{
    return a[X] * b[X] + a[Y] * b[Y];
}

void vec2_add(const vec2 a, const vec2 b, vec2 dest)
{
    dest[X] = a[X] + b[X];
    dest[Y] = a[Y] + b[Y];
}

void vec2_sub(const vec2 a, const vec2 b, vec2 dest)
{
    dest[X] = a[X] - b[X];
    dest[Y] = a[Y] - b[Y];
}

/**
 * get the squared length of a 2 dimensional vector
 */
float vec2_lensq(const vec2 a)
{
    return vec2_dot(a, a);
}

/**
 * normalize a 2 dimensional vector
 */
void vec2_normalize(vec2 a)
{
    float sqsum = vec2_lensq(a);
    float sqrtsumInv = 1.0f / sqrt(sqsum);

    a[X] *= sqrtsumInv;
    a[Y] *= sqrtsumInv;
}

void vec2_rand(vec2 res)
{
    res[X] = random_random() - 0.5f;
    res[Y] = random_random() - 0.5f;
    vec2_normalize(res);
}


/*
 *************************************
 * 3 DIMENSIONAL FLOATING POINT VECTOR
 *************************************
 */

/**
 * sets the value of a 3 dimensional vector to (x,y,z)
 */
void vec3_set(vec3 a, float x, float y, float z)
{
    a[X] = x;
    a[Y] = y;
    a[Z] = z;
}

/**
 * copys the value of src to dest, component-wise
 */
void vec3_copy(const vec3 src, vec3 dst)
{
    memcpy(dst, src, sizeof(vec3));
}

bool vec3_eq(const vec3 a, const vec3 b)
{
    return  feq(a[X], b[X]) &&
            feq(a[Y], b[Y]) &&
            feq(a[Z], b[Z]);
}

/**
 * component-wise comparison of a vector 
 */
void vec3_cmp(const vec3 a, const vec3 b, ivec3 cmp)
{
   cmp[X] = fcmp(a[X], b[X]); 
   cmp[Y] = fcmp(a[Y], b[Y]); 
   cmp[Z] = fcmp(a[Z], b[Z]); 
}

/**
 * standard dot product vector function.
 * a, and b are vectors with 3 dimension
 * @return a_dot_b
 */
float vec3_dot(const vec3 a, const vec3 b)
{
    return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];
}

/**
 * finds the squared length of a 3 dimensional vector
 * @returns squared length of a
 */
float vec3_lensq(const vec3 a)
{
    return vec3_dot(a, a);
}

/**
 * finds the lenght of a 3 dimensional vector. Due to the computational
 * cost of the sqrt function, its encouraged to use lensq whenever possible
 * @returns length of a
 */
float vec3_len(const vec3 a)
{
    return sqrt(vec3_lensq(a));
}

float vec3_distsq(const vec3 a, const vec3 b)
{
    vec3 tmp;
    vec3_sub(a, b, tmp);
    return vec3_lensq(tmp);
}

/**
 *  standard component-wise vector addition. 
 *  the result a + b is stored in dest. 
 *  a, b and dest are expected to be a vector
 *  of length 'len'
 *  @param a the first vector to add
 *  @param b the second vector to add
 *  @param dest the destination to store the addition of a+b
 */
void vec3_add(const vec3 a, const vec3 b, vec3 dest)
{
    dest[X] = a[X] + b[X];
    dest[Y] = a[Y] + b[Y];
    dest[Z] = a[Z] + b[Z];
}

/**
 *  standard vector subtraction. the result a - b is stored
 *  in dest. a, b and dest are expected to be a vector
 *  of length 'len'
 *  @param a the vector to subtract from
 *  @param b the vector to subtract
 *  @param dest the destination to store the subtraction of a-b
 */
void vec3_sub(const vec3 a, const vec3 b, vec3 dest)
{
    dest[X] = a[X] - b[X];
    dest[Y] = a[Y] - b[Y];
    dest[Z] = a[Z] - b[Z];
}

/**
 * normalizes the vector given by 'a'. After the operation, 'a' will be of unit
 * length. 
 */
void vec3_normalize(vec3 a)
{
    float sqsum = vec3_lensq(a);
    if(!feq(sqsum, 0.0f))
    {
        float sqrtsumInv = 1.0f / sqrt(sqsum);

        a[X] *= sqrtsumInv;
        a[Y] *= sqrtsumInv;
        a[Z] *= sqrtsumInv;
    }
}

/**
 * scales a 3 dimensional vector by the factor 'val'
 */
void vec3_scale(vec3 a, float val) 
{
   a[X] *= val; 
   a[Y] *= val; 
   a[Z] *= val; 
}

/**
 * standard vector cross product. a, b, and dest are
 * expected to be a 3-dimensional vector (cross product
 * is undefined for vectors more or less components than 3).
 * the cross product of axb is computed and stored in dest.
 */
void vec3_cross(const vec3 a, const vec3 b, vec3 dest)
{
    vec3 tmp;
    tmp[X] = a[Y] * b[Z] - a[Z] * b[Y];
    tmp[Y] = a[Z] * b[X] - a[X] * b[Z];
    tmp[Z] = a[X] * b[Y] - a[Y] * b[X];
    vec3_copy(tmp, dest);
}

/**
 * projects vector 'v' onto the reference axis 'refaxis'.
 * the result is stored into dest
 * @param v vector to project
 * @param refaxis the axis in which to project 'v' onto
 * @param dest storage for the result proj_refaxis(v)
 */
void vec3_proj(const vec3 v, const vec3 refaxis, vec3 dest)
{
    vec3_copy(refaxis, dest);
    float numer = vec3_dot(v, refaxis);
    float denom = vec3_dot(refaxis, refaxis);
    vec3_scale(dest, numer / denom);
}

/**
 * orthogonalizes 'v' to 'refaxis'. afterwards, v and refaxis will be perfectly
 * perpendicular such that a-dot-refaxis = 0.
 * @param v the vector to orthogonalize
 * @param refaxis the axis to orthogonalize relative to
 * @param dest the result of the orthogonalization
 */
void vec3_orth(const vec3 v, const vec3 refaxis, vec3 dest)
{
    vec3 tmp;
    vec3_proj(v, refaxis, tmp);
    vec3_sub(v, tmp, dest);
}

/**
 * retrieves the component wise average of the provided vectors.
 * @param result the vector to store the results. This will be clobbered
 * @param n the number of vectors to average, there must be at least this many vectors provided
 */
void vec3_avg(vec3 result, int n, ...)
{
    vec3_set(result, 0.0f, 0.0f, 0.0f);

    va_list vl;
    va_start(vl, n);
    
    int i;
    for(i = 0; i < n; i++)
    {
        vec3_add(result, va_arg(vl, vec), result);
    }
    va_end(vl);
    vec3_scale(result, 1.0f/n);
}

/**
 * finds the weighted component wise averge of the provided vectors.
 * the varargs passed should be in the format (float scale, vec3 vec)...,
 * alternating between the scale of the next vector, and the corrisponding
 * vector
 */
void vec3_wavg(vec3 result, int n, ...)
{
    vec3_set(result, 0.0f, 0.0f, 0.0f);

    va_list vl;
    va_start(vl, n);
    
    float scale;
    vec v;
    int i;
    for(i = 0; i < n; i+=2)
    {
        scale = (float) va_arg(vl, double);
        v = va_arg(vl, vec);
        vec3_scale(v, scale);
        vec3_add(result, v, result);
    }
    va_end(vl);
    vec3_scale(result, 1.0f/n);
}

void vec3_lavg(vec3 result, int n, vec *list)
{
    vec3_set(result, 0.0f, 0.0f, 0.0f);

    int i;
    for(i = 0; i < n; i++)
    {
        vec3_add(result, list[i], result); 
    }
    vec3_scale(result, 1.0f/n);
}

void vec3_lwavg(vec3 result, int n, vec *list, float *weight)
{
    vec3_set(result, 0.0f, 0.0f, 0.0f);

    vec3 v;
    int i;
    for(i = 0; i < n; i++)
    {
        vec3_copy(list[i], v);
        vec3_scale(v, weight[i]);
        vec3_add(result, v, result);
    }
    vec3_scale(result, 1.0f/n);
}

void vec3_rand(vec3 res)
{
    res[X] = random_random() - 0.5f;
    res[Y] = random_random() - 0.5f;
    res[Z] = random_random() - 0.5f;
    vec3_normalize(res);
}


/**
 * prints out a 3 dimensional vector to STDOUT
 * useful for debugging
 */
void vec3_print(const vec3 a)
{
    printf("%f, %f, %f\n", a[X], a[Y], a[Z]);
}

/*
 *************************************
 * 4 DIMENSIONAL FLOATING POINT VECTOR
 *************************************
 */

/**
 * sets the value of a 4 dimensional vector to (x,y,z,w)
 */
void vec4_set(vec4 a, float x, float y, float z, float w)
{
    a[X] = x;
    a[Y] = y;
    a[Z] = z;
    a[W] = w;
}

/**
 * copys a 4 dimensional vector from src to dst. src remains unmoddified
 */
void vec4_copy(const vec4 src, vec4 dst)
{
    dst[X] = src[X];
    dst[Y] = src[Y];
    dst[Z] = src[Z];
    dst[W] = src[W];
}

/**
 * stores the component-wise addition of a+b into dest
 * @param a the first vector to add
 * @param b the second vector to add
 * @param dest the destination of the addition, can be a unique vector 
 * or either of a and b
 */
void vec4_add(const vec4 a, const vec4 b, vec4 dest)
{
    dest[W] = a[W] + b[W];
    dest[X] = a[X] + b[X];
    dest[Y] = a[Y] + b[Y];
    dest[Z] = a[Z] + b[Z];
}

/**
 * stores the component-wise subtration of a-b into dest
 * @param a the vector to subtract from
 * @param b the vector to subtract
 * @param dest the destination of the subtraction, can be a unique vector 
 * or either of a and b
 */
void vec4_sub(const vec4 a, const vec4 b, vec4 dest)
{
    dest[W] = a[W] - b[W];
    dest[X] = a[X] - b[X];
    dest[Y] = a[Y] - b[Y];
    dest[Z] = a[Z] - b[Z];
}

/**
 * applies a dot-product (inner-product) to a pair of vectors or quaternions.
 * @returns a-dot-b
 */
float vec4_dot(const vec4 a, const vec4 b)
{
    return a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z] + a[W] * b[W];
}

/**
 * @returns the length squared of the vector
 */
float vec4_lensq(const vec4 a)
{
    return vec4_dot(a, a);
}

/**
 * normalizes a vector to be unit length. if the vector is of length zero, the vector 
 * remains unchanged.
 * @param a the vector to normalize
 */
void vec4_normalize(vec4 a)
{
    float sqsum = vec4_lensq(a);
    if(!feq(sqsum, 0.0f))
    {
        float sqrtsumInv = 1.0f / sqrt(sqsum);

        a[W] *= sqrtsumInv;
        a[X] *= sqrtsumInv;
        a[Y] *= sqrtsumInv;
        a[Z] *= sqrtsumInv;
    }
}

void vec4_scale(vec4 a, const float scale)
{
    a[W] *= scale;
    a[X] *= scale;
    a[Y] *= scale;
    a[Z] *= scale;
}

/**
 * projects vector 'v' onto the reference axis 'refaxis'.
 * the result is stored into dest
 * @param v vector to project
 * @param refaxis the axis in which to project 'v' onto
 * @param dest storage for the result proj_refaxis(v)
 */
void vec4_proj(const vec4 v, const vec4 refaxis, vec4 dest)
{
    vec4_copy(refaxis, dest);
    float numer = vec4_dot(v, refaxis);
    float denom = vec4_dot(refaxis, refaxis);
    vec4_scale(dest, numer / denom);
}

/**
 * orthogonalizes 'v' to 'refaxis'. afterwards, v and refaxis will be perfectly
 * perpendicular such that a-dot-refaxis = 0.
 * @param v the vector to orthogonalize
 * @param refaxis the axis to orthogonalize relative to
 * @param dest the result of the orthogonalization
 */
void vec4_orth(const vec4 v, const vec4 refaxis, vec4 dest)
{
    vec4 tmp;
    vec4_proj(v, refaxis, tmp);
    vec4_sub(v, tmp, dest);
}

/**
 * retrieves the component wise average of the provided vectors.
 * @param result the vector to store the results. This will be clobbered
 * @param n the number of vectors to average, there must be at least this many vectors provided
 */
void vec4_avg(vec4 result, int n, ...)
{
    vec4_set(result, 0.0f, 0.0f, 0.0f, 0.0f);

    va_list vl;
    va_start(vl, n);
    
    int i;
    for(i = 0; i < n; i++)
    {
        vec4_add(result, va_arg(vl, vec4), result);
    }
    vec4_scale(result, 1.0f/n);
}

void vec4_swizzle(vec4 v, int x, int y, int z, int w)
{
    vec4 dst;
    dst[X] = v[x];
    dst[Y] = v[y];
    dst[Z] = v[z];
    dst[W] = v[w];
    vec4_copy(dst, v);
}

/**
 * prints out a 4 dimensional vector to STDOUT
 */
void vec4_print(vec4 a)
{
    printf("%f, %f, %f, %f\n", a[X], a[Y], a[Z], a[W]);
}

/*
 *************
 * QUATERNIONS
 ************* 
 */

//Quaternion alias functions
void  (*quaternion_copy)(const quaternion src, quaternion dst)                      = vec4_copy;
float (*quaternion_dot)(const quaternion a, const quaternion b)                     = vec4_dot;
void  (*quaternion_add)(const quaternion a, const quaternion b, quaternion dest)    = vec4_add;
void  (*quaternion_sub)(const quaternion a, const quaternion b, quaternion dest)    = vec4_sub;
void  (*quaternion_normalize)(quaternion a)                                         = vec4_normalize;
float (*quaternion_lensq)(const quaternion a)                                       = vec4_lensq;

/**
 * sets a quaternion to (w,x,y,z)
 */
void quaternion_set(quaternion a, float w, float x, float y, float z)
{
    a[W] = w;
    a[X] = x;
    a[Y] = y;
    a[Z] = z;
}

/**
 * sets a quaternion to a rotation by 'angle' around the axis 'axis'
 * @param a the quaternion to clear and rotate
 * @param angle the angle to rotate
 * @param axis the axis to rotate about
 */
void quaternion_set_rotation(quaternion a, float angle, vec3 axis)
{
    vec3 naxis; // normalized axis
    vec3_copy(axis, naxis);
    vec3_normalize(naxis);
    float scale = sin(angle / 2.0f);

    a[W] = cos(angle / 2.0f);
    a[X] = naxis[X] * scale;
    a[Y] = naxis[Y] * scale;
    a[Z] = naxis[Z] * scale;
}

/**
 * loads the quaternion identity (1,0,0,0) into a quaternion
 */
void quaternion_identity(quaternion a)
{
    quaternion_set(a, 1.0f, 0.0f, 0.0f, 0.0f);
}

/**
 * rotates a quaternion by 'angle' around the axis 'axis'
 * all previous transformations remain, and the rotation is compounded
 * on top from a global reference point
 * @param a the quaternion to rotate. Must have a coherent value, 
 * to set a base rotation, @see quaternion_set_rotation
 * @param angle the angle to rotate
 * @param axis the axis to rotate about
 */
void quaternion_rotate(quaternion a, float angle, vec3 axis)
{
    quaternion tmp;
    quaternion_set_rotation(tmp, angle, axis);
    quaternion_mult(a, tmp, a);
}

/**
 * the quaternion norm. (equal to the length squared, or a-dot-a)
 * @returns norm(a)
 */
float quaternion_norm(const quaternion a)
{
    return quaternion_dot(a, a);
}

/**
 * stores the complex conjugate of 'a' into variable 'dest'.
 * The conjugate is equal to (w, -x, -y, -z)
 * The complex conjugate is useful for rotating vectors
 * @param a the quaternion to find the conjugate of
 * @param dest where to store the conjugate
 */
void quaternion_conjugate(const quaternion a, quaternion dest)
{
    dest[X] = -a[X];
    dest[Y] = -a[Y];
    dest[Z] = -a[Z];
    dest[W] = a[W];
}

/**
 * retrives the inverse of the quaternion 'a'.
 * the inverse is equal to conjugate(a)/norm(a)
 * @param a the quaternion to invert
 * @param dest where to store the inversion
 */
void quaternion_inverse(const quaternion a, quaternion dest)
{
    float inv_norm = 1.0f / quaternion_norm(a);
    quaternion_conjugate(a, dest);
    dest[W] *= inv_norm;
    dest[X] *= inv_norm;
    dest[Y] *= inv_norm;
    dest[Z] *= inv_norm;
}

/**
 * retrieves the 'real' value from the quaternion. this is 
 * equal to (w, 0, 0, 0).
 */
void quaternion_real(const quaternion a, quaternion dest)
{
    quaternion_set(dest, a[W], 0.0f, 0.0f, 0.0f);
}

/**
 * retrieves the 'imaginary' value from the quaternion. this is 
 * equal to (0, x, y, z)
 */
void quaternion_imaginary(const quaternion a, quaternion dest)
{
    quaternion_set(dest, 0.0f, a[X], a[Y], a[Z]);
}

/**
 * multiplies 2 quaternions and stores the result in dest.
 * Multiplication is performed according to quaternion math
 * @param a the first quaternion to multiply
 * @param b the second quaternion to multiply
 * @param dest the result of a*b. May be a pointer to a,b, or a unique quaternion
 */
void quaternion_mult(const quaternion a, const quaternion b, quaternion dest)
{
    quaternion tmp;
    tmp[W] = a[W] * b[W] - a[X] * b[X] - a[Y] * b[Y] - a[Z] * b[Z];
    tmp[X] = a[W] * b[X] + a[X] * b[W] + a[Y] * b[Z] - a[Z] * b[Y]; 
    tmp[Y] = a[W] * b[Y] + a[Y] * b[W] + a[Z] * b[X] - a[X] * b[Z]; 
    tmp[Z] = a[W] * b[Z] + a[Z] * b[W] + a[X] * b[Y] - a[Y] * b[X]; 
    quaternion_copy(tmp, dest);
}

/**
 * the result of multiplying a*inverse(b). 
 * @param a the dividend of the division
 * @param b the divisor of the division
 * @param dest the result of a/b. May be a pointer to a,b, or a unique quaternion
 */
void quaternion_div(const quaternion a, const quaternion b, quaternion dest)
{
    quaternion inv;
    quaternion_inverse(b, inv);
    quaternion_mult(a, inv, dest);
}

/**
 * rotates the vector b by the quaternion a, acording to quaternion math
 */
void quaternion_vecRotate(quaternion a, vec4 b)
{
    quaternion con;
    quaternion_conjugate(a, con);
    quaternion_mult(a, b, b);
    quaternion_mult(b, con, b);
}

void quaternion_orient(quaternion a, vec3 up, vec3 fwd)
{
    mat4 m;
    mat4_identity(m);
    mat4_orient(m, fwd, up);
    mat4_to_quaternion(m, a);
}
