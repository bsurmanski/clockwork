/**
 * noise.c
 * @file    noise.h
 * noise
 * @date    November 14, 2011
 * @author  Brandon Surmanski
 */


#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "util/random.h"
#include "util/math/scalar.h"

#include "noise.h"

#define DOT2(a,b,x,y) ((a)*(x)+(b)*(y))
#define DOT3(a,b,c,x,y,z) ((a)*(x)+(b)*(y)+(c)*(z))


static float DEFAULT_GRAD[16 * 3] = 
{
    1, 1, 0,
    -1, 1, 0,
    1, -1, 0,
    -1, -1, 0,
    1, 0, 1,
    -1, 0, 1,
    1, 0, -1,
    -1, 0, -1,
    0, 1, 1,
    0, -1, 1,
    0, 1, -1,
    0, -1, -1,

    1, 1, 0,
    -1, 1, 0,
    0, -1, 1,
    0, -1, -1
};

static int GRAD_MASK = 0x0F; //MUST be a power of 2 - 1
static float *GRAD = DEFAULT_GRAD;
static int initialized = 0;

static int seed = 55;

/**
 * sets a seed for the noise functions to follow
 */
void noise_init(int s)
{
    if(initialized)
    {
        noise_finalize();
    }
    initialized = 1;
    seed = s;
    GRAD = malloc(sizeof(float) * 3 *1024);
    GRAD_MASK = 0x03FF;

    int i;
    for(i = 0; i < 1024; i++)
    {
        float z = (random_random() - 0.5f) * 2.0f;
        float r = sqrtf(1.0f - z*z);
        float t = 4.0f * PI * (random_random() * 0.5f);
        float x = r * cosf(t);
        float y = r * sinf(t);
        GRAD[i * 3 + 0] = x;
        GRAD[i * 3 + 1] = y;
        GRAD[i * 3 + 2] = z;
    }
}

void noise_finalize(void)
{
    free(GRAD);
    GRAD = DEFAULT_GRAD;
}

/**
 * linear interpolation between a and b at t%, where 't'
 * is a value between 0 and 1. 'a' is returned if 't' is 0,
 * and 'b' is returned if 't' is 1. A value inbetween 'a' and 'b'
 * is returned for values of t between 0 and 1.
 */
static inline float lerp(float t, float a, float b)
{
    return (a+t*(b-a));
}

/**
 * applys a smoothing function to the value of t. t must be between
 * 0 and 1. The value returned will be between 0 and 1, with the value
 * being slightly closer to the nearest integer(0/1). 
 */
static inline float s_curve(float t)
{
    return (t*t*t*(10+t*(-15+(6*t))));
}

/**
 * produces a random number from -1 to 1. The number is constant when the seed is the same
 */
inline float noise1_random(int x)
{
    x += seed;
    x = (x<<13) ^ x;
    double t = (1.0f - ((x*(x*x*15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    return (float)t;
}

/**
 * produces a smooth interpolated random number from -1 to 1. The value of integer
 * coordinates of 'x' will have a random value, and each fractional coordinate of 
 * x will be an interpolation of the closest integer values
 */
float noise1_value(float x)
{
    int flx = floor(x);
    float dx0 = noise1_random(flx);
    float dx1 = noise1_random(flx + 1);
    return lerp(x-flx, dx0, dx1);
}

/**
 * 1-dimensional perlin noise from -1 to 1.
 */
float noise1_perlin(float x)
{
    int flx = floor(x);
    float dx0 = noise1_random(flx);
    float dx1 = noise1_random(flx + 1);

    float u = (x-flx) * dx0;
    float v = ((x-flx)-1) * dx1;
    return lerp(s_curve(x-flx),u,v);
}

/**
 * 1-dimensional fractal perlin noise from -1 to 1. n 
 * defines the number of different 'octaves' to repeat
 * for the noise
 */
float noise1_fracPerlin(float x, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1){
        sum += (noise1_perlin(x * i)/i);
    }
    return sum;
}

/**
 * terbulent noise. Perlin noise with discontinuous rates of change.
 * value between -1 and 1. n defines the number of 'octaves' to repeat
 */
float noise1_terbulence(float x, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1)
        sum += fabs(noise1_perlin(x*i)/i);
    return sum * 2.0f - 1.0f;
}



/*
 **************
 * 2D Functions
 **************
 */

/**
 * random value from 2 given seeds. See noise_random1 above
 */
inline float noise2_random(int x, int y)
{
    x += seed;
    y += seed;
    x += y * 71;
    x = (x >> 13) ^ x;
    return (1.0f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

/**
 * same as noise_random, except giving a different value for each given point
 */
static void noise2_randGrad(int x, int y, float *xo, float *yo)
{
    x += seed;
    y += seed;
    int x1 = x  * 17 + y * 89;
    int y1 = x  * 31 + y * 71;
    x1 = ((x1 >> 17) ^ x1) & 0x7fffffff;
    y1 = ((y1 >> 17) ^ y1) & 0x7fffffff;
    float *grad = &GRAD[(x1 * 3) & GRAD_MASK];
    *xo = grad[0];
    *yo = grad[1];
}

/**
 * 2 dimensional value noise. See 1 dimensional value noise above.
 * quick noise function with the downside being rectangular artifacts
 */
float noise2_value(float x, float y)
{
    int flx = floor(x);
    int fly = floor(y);
    float v0 = noise2_random(flx, fly);
    float v1 = noise2_random(flx+1,fly);
    float v2 = noise2_random(flx+1,fly+1);
    float v3 = noise2_random(flx,fly+1);

    float px0 = lerp(s_curve(x-flx),v0,v1);
    float px1 = lerp(s_curve(x-flx),v3,v2);
    return lerp(s_curve(y-fly),px0,px1);
}

/**
 * produces 2 dimensional perlin noise
 */ 
float noise2_perlin(float x, float y)
{
    int flx = floor(x);
    int fly = floor(y);
    float frx = x-flx;
    float fry = y-fly;


    float vx, vy;
    float s, t, u, v;
    noise2_randGrad(flx, fly, &vx, &vy);
    s = DOT2(vx, vy, frx, fry);

    noise2_randGrad(flx+1, fly, &vx, &vy); 
    t = DOT2(vx, vy, frx - 1.0f, fry);

    noise2_randGrad(flx+1, fly+1, &vx, &vy); 
    u = DOT2(vx, vy, frx - 1.0f, fry - 1.0f);

    noise2_randGrad(flx, fly+1, &vx, &vy); 
    v = DOT2(vx, vy, frx, fry - 1.0f);

    float px0 = lerp(s_curve(frx), s, t);
    float px1 = lerp(s_curve(frx), v, u);

    return lerp(s_curve(fry), px0, px1);
}

/**
 * produces 2 dimensional perlin fractal noise
 */
float noise2_fracPerlin(float x, float y, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1){
        sum += (noise2_perlin(x * i, y * i)/i);
    }
    return sum;
}

/**
 * produces 2 dimensional perlin terbulent noise. 
 */
float noise2_terbulence(float x, float y, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1)
        sum += fabs(noise2_perlin(x * i, y * i)/((float)i));
    return sum * 2.0f - 1.0f;
}

/*
 * ************
 * 3D Functions
 * ************
 */

static void noise3_randGrad(int x, int y, int z, float *xo, float *yo, float *zo)
{
    x += seed;
    y += seed;
    z += seed;
    int x1 = x * 71 + y * 17 + z * 53;
    x1 = ((x1 >> 17) ^ x1) & 0x7fffffff;
    float *grad = &GRAD[(x1 * 3) & GRAD_MASK];
    *xo = grad[0];
    *yo = grad[1];
    *zo = grad[2];
}

float noise3_perlin(float x, float y, float z)
{
    int flx = floor(x);
    int fly = floor(y);
    int flz = floor(z);
    float frx = x-flx;
    float fry = y-fly;
    float frz = z-flz;

    float vx, vy, vz;
    float o, p, q, r, s, t, u, v;
    noise3_randGrad(flx, fly, flz, &vx, &vy, &vz);
    o = DOT3(vx, vy, vz, frx, fry, frz);

    noise3_randGrad(flx+1, fly, flz, &vx, &vy, &vz); 
    p = DOT3(vx, vy, vz, frx - 1.0f, fry, frz);

    noise3_randGrad(flx+1, fly+1, flz, &vx, &vy, &vz); 
    q = DOT3(vx, vy, vz, frx - 1.0f, fry - 1.0f, frz);

    noise3_randGrad(flx, fly+1, flz, &vx, &vy, &vz); 
    r = DOT3(vx, vy, vz, frx, fry - 1.0f, frz);

    noise3_randGrad(flx, fly, flz+1, &vx, &vy, &vz);
    s = DOT3(vx, vy, vz, frx, fry, frz - 1.0f);

    noise3_randGrad(flx+1, fly, flz+1, &vx, &vy, &vz); 
    t = DOT3(vx, vy, vz, frx - 1.0f, fry, frz - 1.0f);

    noise3_randGrad(flx+1, fly+1, flz+1, &vx, &vy, &vz); 
    u = DOT3(vx, vy, vz, frx - 1.0f, fry - 1.0f, frz - 1.0f);

    noise3_randGrad(flx, fly+1, flz+1, &vx, &vy, &vz); 
    v = DOT3(vx, vy, vz, frx, fry - 1.0f, frz - 1.0f);

    float px0 = lerp(s_curve(frx), s, t);
    float px1 = lerp(s_curve(frx), v, u);
    float px2 = lerp(s_curve(frx), o, p);
    float px3 = lerp(s_curve(frx), r, q);

    float py0 = lerp(s_curve(fry), px0, px1);
    float py1 = lerp(s_curve(fry), px2, px3);


    return lerp(s_curve(frz), py1, py0);
}

/**
 * produces 3 dimensional perlin fractal noise
 */
float noise3_fracPerlin(float x, float y, float z, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1){
        sum += (noise3_perlin(x * i, y * i, z * i)/i);
    }
    return sum;
}

/**
 * produces 3 dimensional perlin terbulent noise. 
 */
float noise3_terbulence(float x, float y, float z, int n)
{
    float sum = 0;
    int i;
    for(i = 1<<n; i>0; i = i>>1)
        sum += fabs(noise3_perlin(x * i, y * i, z * i)/((float)i));
    return sum * 2.0f - 1.0f;
}

