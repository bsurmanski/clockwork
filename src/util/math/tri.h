/*
 * tri.h
 * obj
 * August 06, 2012
 * Brandon Surmanski
 */

#ifndef _TRI_H
#define _TRI_H

#include "util/math/vec.h"

#define TRI_A 0
#define TRI_B 1
#define TRI_C 2

typedef vec2 tri2[3];
typedef vec3 tri3[3];

float tri2_area(tri2 t);
float tri2_signed_area(tri2 t);
void tri2_interp(tri2 t, float *p, float *ret);
bool tri2_contains(tri2 t, float *p);

void tri3_set(tri3 t, vec3 a, vec3 b, vec3 c);
float tri3_area(tri3 t);
float tri3_signed_volume(tri3 t);
void tri3_interp(tri3 t, vec3 p, vec3 v_ret);
bool tri3_contains(tri3 t, float *p);

/*
 * Note, there is no tri3 signed area because the signedness is dependant on perspective.
 * Instead, dot product the known normal with the dot product and use the sign of that.
 */

#endif
