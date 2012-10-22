/**
 * angles.c
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#include "util/math/scalar.h"

#include "angles.h"

void (*angles_normalize)(angles a) = angles_normalize360;

void angles_set(angles a, float pitch, float yaw, float roll)
{
    a[PITCH]    = pitch;
    a[YAW]      = yaw;
    a[ROLL]     = roll;
}

void angles_add(angles a, angles b, angles dst)
{
    dst[PITCH]  = a[PITCH] + b[PITCH];
    dst[YAW]    = a[YAW] + b[YAW];
    dst[ROLL]   = a[ROLL] + b[ROLL];
}

void angles_sub(angles a, angles b, angles dst)
{
    dst[PITCH]  = a[PITCH] - b[PITCH];
    dst[YAW]    = a[YAW] - b[YAW];
    dst[ROLL]   = a[ROLL] - b[ROLL];
}

void angles_clamp(angles a, angles min, angles max)
{
    a[PITCH]    = fclamp(a[PITCH], min[PITCH], max[PITCH]);
    a[YAW]      = fclamp(a[YAW], min[YAW], max[YAW]);
    a[ROLL]     = fclamp(a[ROLL], min[ROLL], max[ROLL]);
}

void angles_normalize360(angles a)
{

}

void angles_normalize180(angles a)
{

}


