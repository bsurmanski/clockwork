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
vec3 hvec3_to_vec3(hvec3 src)
{
    vec3 ret;
    ret.x = src[X] / 65535.0f;
    ret.y = src[Y] / 65535.0f;
    ret.z = src[Z] / 65535.0f;
    return ret;
}

/**
 * converts a 3 dimensional floating point vector to a 16-bit integer vector
 * @see ftoh
 */
void vec3_to_hvec3(vec3 src, hvec3 dst)
{
    dst[X] = ftoh(src.x);
    dst[Y] = ftoh(src.y);
    dst[Z] = ftoh(src.z);
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
vec3 vec3_setp(float x, float y, float z)
{
    vec3 ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}

/**
 * copys the value of src to dest, component-wise
 */
vec3 vec3_copyp(const vec3 *src)
{
    vec3 ret = *src;
    return ret;
}

bool vec3_eqp(const vec3 *a, const vec3 *b)
{
    return  feq(a->x, b->x) &&
            feq(a->y, b->y) &&
            feq(a->z, b->z);
}

/**
 * component-wise comparison of a vector 
 */
ivec3 vec3_cmpp(const vec3 *a, const vec3 *b)
{
    ivec3 cmp;
    cmp.x = fcmp(a->x, b->x); 
    cmp.y = fcmp(a->y, b->y); 
    cmp.z = fcmp(a->z, b->z); 
    return cmp;
}

/**
 * standard dot product vector function.
 * a, and b are vectors with 3 dimension
 * @return a_dot_b
 */
float vec3_dotp(const vec3 *a, const vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

/**
 * finds the squared length of a 3 dimensional vector
 * @returns squared length of a
 */
float vec3_lensqp(const vec3 *a)
{
    return vec3_dotp(a, a);
}

/**
 * finds the lenght of a 3 dimensional vector. Due to the computational
 * cost of the sqrt function, its encouraged to use lensq whenever possible
 * @returns length of a
 */
float vec3_lenp(const vec3 *a)
{
    return sqrt(vec3_lensqp(a));
}

float vec3_distsqp(const vec3 *a, const vec3 *b)
{
    vec3 tmp;
    tmp = vec3_subp(a, b);
    return vec3_lensq(tmp);
}

/**
 *  standard component-wise vector addition. 
 *  the result a + b is stored in dest. 
 *  a, b and dest are expected to be a vector
 *  of length 'len'
 *  @param a the first vector to add
 *  @param b the second vector to add
 *  @returns the addition of a+b
 */
vec3 vec3_addp(const vec3 *a, const vec3 *b)
{
    vec3 ret;
    ret.x = a->x + b->x;
    ret.y = a->y + b->y;
    ret.z = a->z + b->z;
    return ret;
}

/**
 *  standard vector subtraction. the result a - b is stored
 *  in dest. a, b and dest are expected to be a vector
 *  of length 'len'
 *  @param a the vector to subtract from
 *  @param b the vector to subtract
 *  @param dest the destination to store the subtraction of a-b
 */
vec3 vec3_subp(const vec3 *a, const vec3 *b)
{
    vec3 ret;
    ret.x = a->x - b->x;
    ret.y = a->y - b->y;
    ret.z = a->z - b->z;
    return ret;
}

/**
 * normalizes the vector given by 'a'. After the operation, 'a' will be of unit
 * length. 
 */
void vec3_normalizep(vec3 *a)
{
    float sqsum = vec3_lensqp(a);
    if(!feq(sqsum, 0.0f))
    {
        float sqrtsumInv = 1.0f / sqrt(sqsum);

        a->x *= sqrtsumInv;
        a->y *= sqrtsumInv;
        a->z *= sqrtsumInv;
    }
}

/**
 * scales a 3 dimensional vector by the factor 'val'
 */
void vec3_scalep(vec3 *a, float val) 
{
   a->x *= val; 
   a->y *= val; 
   a->z *= val; 
}

/**
 * standard vector cross product. a, b, and dest are
 * expected to be a 3-dimensional vector (cross product
 * is undefined for vectors more or less components than 3).
 * the cross product of axb is computed and stored in dest.
 */
vec3 vec3_crossp(const vec3 *a, const vec3 *b)
{
    vec3 ret;
    ret.x = a->y * b->z - a->z * b->y;
    ret.y = a->z * b->x - a->x * b->z;
    ret.z = a->x * b->y - a->y * b->x;
    return ret;
}

/**
 * projects vector 'v' onto the reference axis 'refaxis'.
 * the result is stored into dest
 * @param v vector to project
 * @param refaxis the axis in which to project 'v' onto
 * @param dest storage for the result proj_refaxis(v)
 */
vec3 vec3_projp(const vec3 *v, const vec3 *axis)
{
    vec3 ret = *axis;
    float numer = vec3_dotp(v, axis);
    float denom = vec3_dotp(axis, axis);
    if(fabs(denom) > FLT_EPSILON)
    {
        vec3_scale(ret, numer / denom);
    }
    return ret;
}

/**
 * orthogonalizes 'v' to 'refaxis'. afterwards, v and refaxis will be perfectly
 * perpendicular such that a-dot-refaxis = 0.
 * @param v the vector to orthogonalize
 * @param refaxis the axis to orthogonalize relative to
 * @param dest the result of the orthogonalization
 */
vec3 vec3_orthp(const vec3 *v, const vec3 *axis)
{
    vec3 ret = vec3_projp(v, axis);
    ret = vec3_subp(v, &ret);
    return ret;
}

/**
 * retrieves the component wise average of the provided vectors.
 * @param result the vector to store the results. This will be clobbered
 * @param n the number of vectors to average, there must be at least this many vectors provided
 */
/*
vec3 vec3_avg(int n, ...)
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
}*/

/**
 * finds the weighted component wise averge of the provided vectors.
 * the varargs passed should be in the format (float scale, vec3 vec)...,
 * alternating between the scale of the next vector, and the corrisponding
 * vector
 */
/*
vec3 vec3_wavg(int n, ...)
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

vec3 vec3_lavg(int n, vec *list)
{
    vec3_set(result, 0.0f, 0.0f, 0.0f);

    int i;
    for(i = 0; i < n; i++)
    {
        vec3_add(result, list[i], result); 
    }
    vec3_scale(result, 1.0f/n);
}

vec3 vec3_lwavg(int n, vec *list, float *weight)
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
*/

vec3 vec3_randp(void)
{
    vec3 v;
    v.x = random_random() - 0.5f;
    v.y = random_random() - 0.5f;
    v.z = random_random() - 0.5f;
    vec3_normalizep(&v);
    return v;
}


/**
 * prints out a 3 dimensional vector to STDOUT
 * useful for debugging
 */
void vec3_printp(const vec3 *a)
{
    printf("%f, %f, %f\n", a->x, a->y, a->z);
}

/*
 *************************************
 * 4 DIMENSIONAL FLOATING POINT VECTOR
 *************************************
 */

/**
 * sets the value of a 4 dimensional vector to (x,y,z,w)
 */
vec4 vec4_setp(float x, float y, float z, float w)
{
    vec4 a;
    a.x = x;
    a.y = y;
    a.z = z;
    a.w = w;
    return a;
}

/**
 * copys a 4 dimensional vector from src to dst. src remains unmoddified
 */
vec4 vec4_copyp(const vec4 *src)
{
    vec4 dst = *src;
    return dst;
}

/**
 * stores the component-wise addition of a+b into dest
 * @param a the first vector to add
 * @param b the second vector to add
 * @param dest the destination of the addition, can be a unique vector 
 * or either of a and b
 */
vec4 vec4_addp(const vec4 *a, const vec4 *b)
{
    vec4 ret;
    ret.x = a->x + b->x;
    ret.x = a->y + b->y;
    ret.x = a->z + b->z;
    ret.x = a->w + b->w;
    return ret;
}

/**
 * stores the component-wise subtration of a-b into dest
 * @param a the vector to subtract from
 * @param b the vector to subtract
 * @param dest the destination of the subtraction, can be a unique vector 
 * or either of a and b
 */
vec4 vec4_subp(const vec4 *a, const vec4 *b)
{
    vec4 ret;
    ret.x = a->x - b->x;
    ret.x = a->y - b->y;
    ret.x = a->z - b->z;
    ret.x = a->w - b->w;
    return ret;
}

/**
 * applies a dot-product (inner-product) to a pair of vectors or quaternions.
 * @returns a-dot-b
 */
float vec4_dotp(const vec4 *a, const vec4 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

/**
 * @returns the length squared of the vector
 */
float vec4_lensqp(const vec4 *a)
{
    return vec4_dotp(a, a);
}

/**
 * normalizes a vector to be unit length. if the vector is of length zero, the vector 
 * remains unchanged.
 * @param a the vector to normalize
 */
void vec4_normalizep(vec4 *a)
{
    float sqsum = vec4_lensqp(a);
    if(!feq(sqsum, 0.0f))
    {
        float sqrtsumInv = 1.0f / sqrt(sqsum);

        a->w *= sqrtsumInv;
        a->x *= sqrtsumInv;
        a->y *= sqrtsumInv;
        a->z *= sqrtsumInv;
    }
}

/**
 * normalizes a vector to be unit length. if the vector is of length zero, the vector 
 * remains unchanged.
 * @param a the vector to normalize
 */
vec4 vec4_normalizedp(vec4 *a)
{
    vec4 ret;
    float sqsum = vec4_lensqp(a);
    if(!feq(sqsum, 0.0f))
    {
        float sqrtsumInv = 1.0f / sqrt(sqsum);

        ret.x = sqrtsumInv * a->x;
        ret.y = sqrtsumInv * a->y;
        ret.z = sqrtsumInv * a->z;
        ret.w = sqrtsumInv * a->w;
    } else 
    {
        ret.x = 0.0f; 
        ret.y = 0.0f; 
        ret.z = 0.0f; 
        ret.w = 0.0f; 
    }
    return ret;
}

void vec4_scalep(vec4 *a, const float scale)
{
    a->x *= scale;
    a->y *= scale;
    a->z *= scale;
    a->w *= scale;
}

/**
 * projects vector 'v' onto the reference axis 'refaxis'.
 * the result is stored into dest
 * @param v vector to project
 * @param refaxis the axis in which to project 'v' onto
 * @param dest storage for the result proj_refaxis(v)
 */
vec4 vec4_projp(const vec4 *v, const vec4 *refaxis)
{
    vec4 ret  = *refaxis;
    float numer = vec4_dotp(v, refaxis);
    float denom = vec4_dotp(refaxis, refaxis);
    vec4_scalep(&ret, numer / denom);
    return ret;
}

/**
 * orthogonalizes 'v' to 'refaxis'. afterwards, v and refaxis will be perfectly
 * perpendicular such that a-dot-refaxis = 0.
 * @param v the vector to orthogonalize
 * @param refaxis the axis to orthogonalize relative to
 * @param dest the result of the orthogonalization
 */
vec4 vec4_orthp(const vec4 *v, const vec4 *refaxis)
{
    vec4 ret;
    ret = vec4_projp(v, refaxis);
    ret = vec4_subp(v, &ret);
    return ret;
}

/**
 * retrieves the component wise average of the provided vectors.
 * @param result the vector to store the results. This will be clobbered
 * @param n the number of vectors to average, there must be at least this many vectors provided
 */
/*
vec4 vec4_avgp(int n, ...)
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
}*/

/* XXX todo
void vec4_swizzle(vec4 v, int x, int y, int z, int w)
{
    vec4 dst;
    dst[X] = v[x];
    dst[Y] = v[y];
    dst[Z] = v[z];
    dst[W] = v[w];
    vec4_copy(dst, v);
}*/

/**
 * prints out a 4 dimensional vector to STDOUT
 */
void vec4_printp(vec4 *a)
{
    printf("%f, %f, %f, %f\n", a->x, a->y, a->z, a->w);
}

/*
 *************
 * QUATERNIONS
 ************* 
 */

//quat alias functions

quat  (*quaternion_copyp)(const quat *src)             = vec4_copyp;
float (*quaternion_dotp)(const quat *a, const quat *b) = vec4_dotp;
quat  (*quaternion_addp)(const quat *a, const quat *b) = vec4_addp;
quat  (*quaternion_subp)(const quat *a, const quat *b) = vec4_subp;
void  (*quaternion_normalizep)(quat *a)                = vec4_normalizep;
float (*quaternion_lensqp)(const quat *a)              = vec4_lensqp;


#if 1

/**
 * loads the quat identity (1,0,0,0) into a quaternion
 */
quat quaternion_identity(void)
{
    quat ret;
    ret.w = 1.0f;
    ret.x = 0.0f;
    ret.y = 0.0f;
    ret.z = 0.0f;
    return ret;
}

/**
 * sets a quat to (w,x,y,z)
 */
quat quaternion_set(float w, float x, float y, float z)
{
    quat ret;
    ret.w = w;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}

/**
 * sets a quat to a rotation by 'angle' around the axis 'axis'
 * @param a the quat to clear and rotate
 * @param angle the angle to rotate
 * @param axis the axis to rotate about
 */
quat quaternion_set_rotation(float angle, vec3 axis)
{
    quat ret;
    vec3 naxis; // normalized axis
    naxis = axis;
    vec3_normalizep(&naxis);
    float scale = sin(angle / 2.0f);

    ret.w = cos(angle / 2.0f);
    ret.x = naxis.x * scale;
    ret.y = naxis.y * scale;
    ret.z = naxis.z * scale;
    return ret;
}

quat quaternion_lerpp(quat *a, quat *b, float t)
{
    quat ret;
    ret.x = a->x * t + b->x * (1.0 - t);
    ret.y = a->y * t + b->y * (1.0 - t);
    ret.z = a->z * t + b->z * (1.0 - t);
    ret.w = a->w * t + b->w * (1.0 - t);
    return ret;
}

quat quaternion_slerpp(quat *a, quat *b, float t)
{
    //TODO slerp
    quat q = quaternion_set(0,0,0,0);
    return q;
}

/**
 * rotates a quat by 'angle' around the axis 'axis'
 * all previous transformations remain, and the rotation is compounded
 * on top from a global reference point
 * @param a the quat to rotate. Must have a coherent value, 
 * to set a base rotation, @see quaternion_set_rotation
 * @param angle the angle to rotate
 * @param axis the axis to rotate about
 */
void quaternion_rotatep(quat *a, float angle, vec3 axis)
{
    quat tmp;
    tmp = quaternion_set_rotation(angle, axis);
    *a = quaternion_multp(a, &tmp);
}

/**
 * the quat norm. (equal to the length squared, or a-dot-a)
 * @returns norm(a)
 */
float quaternion_normp(const quat *a)
{
    return quaternion_dotp(a, a);
}

/**
 * stores the complex conjugate of 'a' into variable 'dest'.
 * The conjugate is equal to (w, -x, -y, -z)
 * The complex conjugate is useful for rotating vectors
 * @param a the quat to find the conjugate of
 * @param dest where to store the conjugate
 */
quat quaternion_conjugatep(const quat *a)
{
    quat ret;
    ret.x = -a->x;
    ret.y = -a->y;
    ret.z = -a->z;
    ret.w = a->w;
    return ret;
}

/**
 * retrives the inverse of the quat 'a'.
 * the inverse is equal to conjugate(a)/norm(a)
 * @param a the quat to invert
 * @param dest where to store the inversion
 */
quat quaternion_inversep(const quat *a)
{
    quat ret;
    float inv_norm = 1.0f / quaternion_normp(a);
    ret = quaternion_conjugatep(a);
    ret.x *= inv_norm;
    ret.y *= inv_norm;
    ret.z *= inv_norm;
    ret.w *= inv_norm;
    return ret;
}

/**
 * retrieves the 'real' value from the quaternion. this is 
 * equal to (w, 0, 0, 0).
 */
quat quaternion_realp(const quat *a)
{
    quat ret = quaternion_set(a->w, 0.0f, 0.0f, 0.0f);
    return ret;
}

/**
 * retrieves the 'imaginary' value from the quaternion. this is 
 * equal to (0, x, y, z)
 */
quat quaternion_imaginaryp(const quat *a)
{
    quat ret = quaternion_set(0.0f, a->x, a->y, a->z);
    return ret;
}

/**
 * multiplies 2 quaternions and stores the result in dest.
 * Multiplication is performed according to quat math
 * @param a the first quat to multiply
 * @param b the second quat to multiply
 * @param dest the result of a*b. May be a pointer to a,b, or a unique quaternion
 */
quat quaternion_multp(const quat *a, const quat *b)
{
    quat ret;
    ret.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    ret.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y; 
    ret.y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z; 
    ret.z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x; 
    return ret;
}

/**
 * the result of multiplying a*inverse(b). 
 * @param a the dividend of the division
 * @param b the divisor of the division
 * @param dest the result of a/b. May be a pointer to a,b, or a unique quaternion
 */
quat quaternion_divp(const quat *a, const quat *b)
{
    quat inv;
    quat ret;
    inv = quaternion_inversep(b);
    ret = quaternion_multp(a, &inv);
    return ret;
}

/**
 * rotates the vector b by the quat a, acording to quat math
 */
quat quaternion_vecRotatep(quat *a, vec4 *b)
{
    float btmp[4]; memcpy(&btmp, b, sizeof(float) * 4); //TODO fix quaternions
    quat ret;
    quat con;
    con = quaternion_conjugatep(a);
    ret = quaternion_multp(a, b);
    ret = quaternion_mult(ret, con);
    return ret;
}

quat quaternion_orientp(vec3 up, vec3 fwd)
{
    quat ret;
    mat4 m;
    mat4_identity(m);
    mat4_orient(m, &fwd, &up);
    ret = mat4_to_quaternion(m);
    return ret;
}

#endif
