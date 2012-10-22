/**
 * convert.h
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#ifndef _CONVERT_H
#define _CONVERT_H

#include "scalar.h"
#include "vec.h"
#include "angles.h"
#include "matrix.h"


float degrees_to_radians(float d);
float radians_to_degrees(float r);


void mat4_to_quaternion(mat4 m, quaternion q);
void quaternion_to_mat4(quaternion q, mat4 m);

void angles_to_quaternion(angles a, quaternion q);
void quaternion_to_angles(quaternion q, angles a);

void angles_to_mat4(angles a, mat4 m);
void mat4_to_angles(mat4 m, angles a);

float fahrenheit_to_celsius(float f);
float celsius_to_fahrenheit(float f);
float celsius_to_kelvin(float c);
float kelvin_to_celsius(float k);
//extern float (*deg_to_rad)(float d);
//extern float (*rad_to_deg)(float r);
//extern void (*mat4_to_quat)(mat4 m, quaternion q);
//extern void (*quat_to_mat4)(quaternion q, mat4 m);
#endif
