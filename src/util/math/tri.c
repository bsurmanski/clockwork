/*
 * tri.c
 * obj
 * August 06, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util/math/vec.h"
#include "util/math/line.h"

#include "tri.h"

static float tri2_sqarea(tri2 t);
static float tri3_sqarea(tri3 t);

/*
 * ***********************
 * 2 Dimensional Functions
 * ***********************
 */

float tri2_area(tri2 t)
{
    return sqrt(tri2_sqarea(t));
}

/**
 * heron's formula to find the area of a triangle, left squared. sqrt to find actual area 
 */
static float tri2_sqarea(tri2 t)
{
    vec2 ab_v, bc_v, ca_v; 
    vec2_sub(t[TRI_A], t[TRI_B], ab_v);
    vec2_sub(t[TRI_B], t[TRI_C], bc_v);
    vec2_sub(t[TRI_C], t[TRI_A], ca_v);

    float ab = sqrt(vec2_lensq(ab_v));
    float bc = sqrt(vec2_lensq(bc_v));
    float ca = sqrt(vec2_lensq(ca_v));

    float s = (ab + bc + ca) * 0.5f;
    return (s * (s-ab) * (s-bc) * (s-ca));
}

float tri2_signed_area(tri2 t)
{
    return 0.5f * 
        (- t[TRI_B][X] * t[TRI_A][Y]
         + t[TRI_C][X] * t[TRI_A][Y]
         + t[TRI_A][X] * t[TRI_B][Y]
         - t[TRI_C][X] * t[TRI_B][Y]
         - t[TRI_A][X] * t[TRI_C][Y]
         + t[TRI_B][X] * t[TRI_C][Y]);
}

/**
 *
void raster_tri_interp2(float *a, float *b, float *c, float *p, float *ret)
{
    float aT = raster_tri_area2(a, b, c);
    float aA = raster_tri_area2(b, c, p);
    float aB = raster_tri_area2(c, p, a);
    float aC = raster_tri_area2(p, a, b);
    ret[0] = aA / aT;
    ret[1] = aB / aT;
    ret[2] = aC / aT;
}*/

/*
 * ***********************
 * 3 Dimensional Functions
 * ***********************
 */

void tri3_set(tri3 t, vec3 a, vec3 b, vec3 c)
{
    memcpy(&t[TRI_A], a, sizeof(vec3));
    memcpy(&t[TRI_B], b, sizeof(vec3));
    memcpy(&t[TRI_C], c, sizeof(vec3));
}

/**
 * heron's formula to find the area of a triangle
 */
float tri3_area(tri3 t)
{
    return sqrt(tri3_sqarea(t));
}

static float tri3_sqarea(tri3 t)
{
    vec3 ab_v, bc_v, ca_v; 
    vec3_sub(t[TRI_A], t[TRI_B], ab_v);
    vec3_sub(t[TRI_B], t[TRI_C], bc_v);
    vec3_sub(t[TRI_C], t[TRI_A], ca_v);

    float ab = sqrt(vec3_lensq(ab_v));
    float bc = sqrt(vec3_lensq(bc_v));
    float ca = sqrt(vec3_lensq(ca_v));

    float s = (ab + bc + ca) * 0.5f;
    return (s * (s-ab) * (s-bc) * (s-ca));
}

float tri3_signed_volume(tri3 t)
{
    float cba = t[TRI_C][X] * t[TRI_B][Y] * t[TRI_A][Z];
    float bca = t[TRI_B][X] * t[TRI_C][Y] * t[TRI_A][Z];
    float cab = t[TRI_C][X] * t[TRI_A][Y] * t[TRI_B][Z];
    float acb = t[TRI_A][X] * t[TRI_C][Y] * t[TRI_B][Z];
    float bac = t[TRI_B][X] * t[TRI_A][Y] * t[TRI_C][Z];
    float abc = t[TRI_A][X] * t[TRI_B][Y] * t[TRI_C][Z];

    return (1.0f / 6.0f) * (-cba + bca + cab - acb - bac + abc);
}

/**
 * find the interpolation values 
 */
void tri3_interp(tri3 t, vec3 p, vec3 v_ret)
{
    tri3 tA;
    tri3 tB;
    tri3 tC;
    tri3_set(tA, t[TRI_B], t[TRI_C], p);
    tri3_set(tA, t[TRI_C], p, t[TRI_A]);
    tri3_set(tA, p, t[TRI_A], t[TRI_B]);
    float aT = tri3_area(t);
    float aA = tri3_area(tA);
    float aB = tri3_area(tB);
    float aC = tri3_area(tC);
    v_ret[0] = aA / aT;
    v_ret[1] = aB / aT;
    v_ret[2] = aC / aT;
    vec3_normalize(v_ret); //normalize, for if p is not perfectly contained in t
}
