/**
 * spline.c
 * clockwork
 * November 18, 2012
 * Brandon Surmanski
 */

#include "spline.h"

void spline_cspline2(float t, vec2 p0, vec2 p1, vec2 m0, vec2 m1, vec2 ret)
{
    vec2 tmp;
    float p0_scale = (1.0f + (2.0f * t)) * (1.0f - t) * (1.0f - t); // 2t^3 - 3t^2 + 1 (factorized)
    float p1_scale = t * (1.0f - t) * (1.0f - t);                   // t^3 - 2t^2 + t (factorized)
    float m0_scale = t * t * (3.0f - (2.0f * t));                   //-2t^3 + 3t^2    (factorized)
    float m1_scale = t * t * (t - 1.0f);                            //t^3 - t^2      (factorized)
    vec2_set(ret, 0.0f, 0.0f);
    
    vec2_copy(p0, tmp);
    vec2_scale(tmp, p0_scale);
    vec2_add(ret, tmp, ret);

    vec2_copy(p1, tmp);
    vec2_scale(tmp, p1_scale);
    vec2_add(ret, tmp, ret);

    vec2_copy(m0, tmp);
    vec2_scale(tmp, m0_scale);
    vec2_add(ret, tmp, ret);

    vec2_copy(m1, tmp);
    vec2_scale(tmp, m1_scale);
    vec2_add(ret, tmp, ret);
}

void spline_cspline3(float t, vec3 p0, vec3 p1, vec3 m0, vec3 m1, vec3 ret)
{
    vec3 tmp;
    float p0_scale = (1.0f + (2.0f * t)) * (1.0f - t) * (1.0f - t); // 2t^3 - 3t^2 + 1 (factorized)
    float p1_scale = t * (1.0f - t) * (1.0f - t);                   // t^3 - 2t^2 + t (factorized)
    float m0_scale = t * t * (3.0f - (2.0f * t));                   //-2t^3 + 3t^2    (factorized)
    float m1_scale = t * t * (t - 1.0f);                            //t^3 - t^2      (factorized)
    vec3_set(ret, 0.0f, 0.0f, 0.0f);
    
    vec3_copy(p0, tmp);
    vec3_scale(tmp, p0_scale);
    vec3_add(ret, tmp, ret);

    vec3_copy(p1, tmp);
    vec3_scale(tmp, p1_scale);
    vec3_add(ret, tmp, ret);

    vec3_copy(m0, tmp);
    vec3_scale(tmp, m0_scale);
    vec3_add(ret, tmp, ret);

    vec3_copy(m1, tmp);
    vec3_scale(tmp, m1_scale);
    vec3_add(ret, tmp, ret);
}
