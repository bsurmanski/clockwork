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
    float ww = q[W] * q[W];
    float wx2 = 2.0f * q[W] * q[X];
    float wy2 = 2.0f * q[W] * q[Y];
    float wz2 = 2.0f * q[W] * q[Z];
    float xx = q[X] * q[X];
    float xy2 = 2.0f * q[X] * q[Y];
    float xz2 = 2.0f * q[X] * q[Z];
    float yy = q[Y] * q[Y];
    float yz2 = 2.0f * q[Y] * q[Z];
    float zz = q[Z] * q[Z];

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

void mat4_to_quaternion(mat4 m, quaternion q)
{
    if(m[MAT_XX] + m[MAT_YY] + m[MAT_ZZ] > 0.0f)
    {
        float t = m[MAT_XX] + m[MAT_YY] + m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        q[W] = s * t;
        q[Z] = (m[MAT_YX] - m[MAT_XY]) * s;
        q[Y] = (m[MAT_XZ] - m[MAT_ZX]) * s;
        q[X] = (m[MAT_ZY] - m[MAT_YZ]) * s;
    } else if(m[MAT_XX] + m[MAT_YY] + m[MAT_XX] > m[MAT_ZZ])
    {
        float t = m[MAT_XX] - m[MAT_YY] - m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        q[X] = s * t;
        q[Y] = (m[MAT_YX] + m[MAT_XY]) * s;
        q[Z] = (m[MAT_XZ] + m[MAT_ZX]) * s;
        q[W] = (m[MAT_ZY] - m[MAT_YZ]) * s;
    } else if (m[MAT_YY] > m[MAT_ZZ])
    {
        float t = -m[MAT_XX] + m[MAT_YY] - m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        q[Y] = s * t;
        q[X] = (m[MAT_YX] + m[MAT_XY]) * s;
        q[W] = (m[MAT_XZ] - m[MAT_ZX]) * s;
        q[Z] = (m[MAT_ZY] + m[MAT_YZ]) * s;
    } else {
        float t = -m[MAT_XX] - m[MAT_YY] + m[MAT_ZZ] + 1.0f; 
        float s = fisqrt(t) * 0.5f;

        q[Z] = s * t;
        q[W] = (m[MAT_YX] + m[MAT_XY]) * s;
        q[X] = (m[MAT_XZ] - m[MAT_ZX]) * s;
        q[Y] = (m[MAT_ZY] + m[MAT_YZ]) * s;
    }
    quaternion_normalize(q);
}

void angles_to_quaternion(angles a, quaternion q)
{
    float sp = sin(a[PITCH] / 2.0f); 
    float cp = cos(a[PITCH] / 2.0f);
    float sy = sin(a[YAW] / 2.0f); 
    float cy = cos(a[YAW] / 2.0f);
    float sr = sin(a[ROLL] / 2.0f); 
    float cr = cos(a[ROLL] / 2.0f);

    q[W] = cp * cy * cr + sp * sy * sr;
    q[X] = sp * cy * cr - cp * sy * sr;
    q[Y] = cp * sp * cr + sp * cy * sr;
    q[Z] = cp * cp * sr - sp * sy * cr;
}

void quaternion_to_angles(quaternion q, angles a)
{
    a[PITCH]    = atan2(2.0f * q[W] * q[X] + q[Y] * q[Z],
                        1.0f - (2.0f * (q[X] * q[X] + q[Y] * q[Y])));
    a[YAW]      = asin(2.0f * (q[W] * q[Y] - q[Z] * q[X]));
    a[ROLL]    = atan2(2.0f * q[W] * q[Z] + q[X] * q[Y],
                        1.0f - (2.0f * (q[Y] * q[Y] + q[Z] * q[Z])));
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
