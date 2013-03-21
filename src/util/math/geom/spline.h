/**
 * spline.h
 * clockwork
 * November 18, 2012
 * Brandon Surmanski
 */

#include "util/math/vec.h"

void spline_cspline2(float t, vec2 p0, vec2 p1, vec2 m0, vec2 m1, vec2 ret);
vec3 spline_cspline3(float t, vec3 p0, vec3 p1, vec3 m0, vec3 m1);
