/**
 * convert.c
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#include <math.h>

#include "convert.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float degrees_to_radians(float d)
{
    return (d * M_PI / 180.0);
}

float radians_to_degrees(float r)
{
    return (r * 180.0 / M_PI);
}

/**
 * retrieves the equivilent transformation matrix of a quaternion
 */
void quaternion_to_mat4(quaternion q, mat4 m)
{
    float ww = q.w * q.w;
    float wx2 = 2.0f * q.w * q.x;
    float wy2 = 2.0f * q.w * q.y;
    float wz2 = 2.0f * q.w * q.z;
    float xx = q.x * q.x;
    float xy2 = 2.0f * q.x * q.y;
    float xz2 = 2.0f * q.x * q.z;
    float yy = q.y * q.y;
    float yz2 = 2.0f * q.y * q.z;
    float zz = q.z * q.z;

    m[MAT_XX] = ww + xx - yy - zz;
    m[MAT_XY] = xy2 + wz2;
    m[MAT_XZ] = xz2 - wy2;
    m[MAT_XW] = 0.0f;

    m[MAT_YX] = xy2 - wz2;
    m[MAT_YY] = ww - xx + yy - zz;
    m[MAT_YZ] = yz2 + wx2;
    m[MAT_YW] = 0.0f;

    m[MAT_ZX] = xz2 + wy2;
    m[MAT_ZY] = yz2 - wx2;
    m[MAT_ZZ] = ww - xx - yy + zz;
    m[MAT_ZW] = 0.0f;

    m[MAT_WX] = 0.0f;
    m[MAT_WY] = 0.0f;
    m[MAT_WZ] = 0.0f;
    m[MAT_WW] = 1.0f;
}

quat mat4_to_quaternion(mat4 m)
{
    quat ret;
    if(m[MAT_XX] + m[MAT_YY] + m[MAT_ZZ] > 0.0f)
    {
        float t = m[MAT_XX] + m[MAT_YY] + m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        ret.w = s * t;
        ret.z = (m[MAT_YX] - m[MAT_XY]) * s;
        ret.y = (m[MAT_XZ] - m[MAT_ZX]) * s;
        ret.x = (m[MAT_ZY] - m[MAT_YZ]) * s;
    } else if(m[MAT_XX] + m[MAT_YY] + m[MAT_XX] > m[MAT_ZZ])
    {
        float t = m[MAT_XX] - m[MAT_YY] - m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        ret.x = s * t;
        ret.y = (m[MAT_YX] + m[MAT_XY]) * s;
        ret.z = (m[MAT_XZ] + m[MAT_ZX]) * s;
        ret.w = (m[MAT_ZY] - m[MAT_YZ]) * s;
    } else if (m[MAT_YY] > m[MAT_ZZ])
    {
        float t = -m[MAT_XX] + m[MAT_YY] - m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        ret.y = s * t;
        ret.x = (m[MAT_YX] + m[MAT_XY]) * s;
        ret.w = (m[MAT_XZ] - m[MAT_ZX]) * s;
        ret.z = (m[MAT_ZY] + m[MAT_YZ]) * s;
    } else {
        float t = -m[MAT_XX] - m[MAT_YY] + m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        ret.z = s * t;
        ret.w = (m[MAT_YX] + m[MAT_XY]) * s;
        ret.x = (m[MAT_XZ] - m[MAT_ZX]) * s;
        ret.y = (m[MAT_ZY] + m[MAT_YZ]) * s;
    }
    quaternion_normalizep(&ret);
    return ret;
}

quat angles_to_quaternion(angles a)
{
    quat ret;
    float sp = sin(a[PITCH] / 2.0f); 
    float cp = cos(a[PITCH] / 2.0f);
    float sy = sin(a[YAW] / 2.0f); 
    float cy = cos(a[YAW] / 2.0f);
    float sr = sin(a[ROLL] / 2.0f); 
    float cr = cos(a[ROLL] / 2.0f);

    ret.w = cp * cy * cr + sp * sy * sr;
    ret.x = sp * cy * cr - cp * sy * sr;
    ret.y = cp * sp * cr + sp * cy * sr;
    ret.z = cp * cp * sr - sp * sy * cr;
    return ret;
}

void quaternion_to_angles(quaternion q, angles a)
{
    a[PITCH]    = atan2(2.0f * q.w * q.x + q.y * q.z,
                        1.0f - (2.0f * (q.x * q.x + q.y * q.y)));
    a[YAW]      = asin(2.0f * (q.w * q.y - q.z * q.x));
    a[ROLL]    = atan2(2.0f * q.w * q.z + q.x * q.y,
                        1.0f - (2.0f * (q.y * q.y + q.z * q.z)));
}

void angles_to_mat4(angles a, mat4 m)
{
    float sp = sin(a[PITCH]); 
    float cp = cos(a[PITCH]);
    float sy = sin(a[YAW]); 
    float cy = cos(a[YAW]);
    float sr = sin(a[ROLL]); 
    float cr = cos(a[ROLL]);

    m[MAT_XX] = cy * cr;
    m[MAT_XY] = -cy * sr;
    m[MAT_XZ] = sy;
    m[MAT_XW] = 0.0f;

    m[MAT_YX] = cp * cr + sp * sy * cr;
    m[MAT_YY] = cp * cr - sp * sy * sr;
    m[MAT_YZ] = -sp * sy;
    m[MAT_YW] = 0.0f;

    m[MAT_ZX] = sp * sr - cp * sy * cr;
    m[MAT_ZY] = sp * cr + cp * sy * sr;
    m[MAT_ZZ] = cp * cy;
    m[MAT_ZW] = 0.0f;

    m[MAT_WX] = 0.0f;
    m[MAT_WY] = 0.0f;
    m[MAT_WZ] = 0.0f;
    m[MAT_WW] = 1.0f;
}

void mat4_to_angles(mat4 m, angles a)
{
    a[PITCH]    = atan2(m[MAT_XZ], m[MAT_YZ]);
    a[YAW]      = acos(m[MAT_ZZ]); //TODO: if MAT_ZZ == 0
    a[ROLL]     = -atan2(m[MAT_ZX], m[MAT_ZY]);
}


float fahrenheit_to_celsius(float f)
{
    return (f - 32.0f) * 1.8f;
}

float celsius_to_fahrenheit(float c)
{
    return (c / 1.8f) + 32.0f;
}

float celsius_to_kelvin(float c)
{
    return c - 273.15f;
}

float kelvin_to_celsius(float k)
{
    return k + 273.15f;
}


float (*deg_to_rad)(float d) = degrees_to_radians;
float (*rad_to_deg)(float r) = radians_to_degrees;

void (*mat4_to_quat)(mat4 m, quaternion q) = mat4_to_quaternion;
void (*quat_to_mat4)(quaternion q, mat4 m) = quaternion_to_mat4;
