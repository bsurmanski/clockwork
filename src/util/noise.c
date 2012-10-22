/**
 * noise.c
 * @file    noise.h
 * noise
 * @date    November 14, 2011
 * @author  Brandon Surmanski
 */


#include <math.h>
#include <float.h>

#include "noise.h"

float GRAD[16][3] = 
{
    {1, 1, 0},
    {-1, 1, 0},
    {1, -1, 0},
    {-1, -1, 0},
    {1, 0, 1},
    {-1, 0, 1},
    {1, 0, -1},
    {-1, 0, -1},
    {0, 1, 1},
    {0, -1, 1},
    {0, 1, -1},
    {0, -1, -1},

    {1, 1, 0},
    {-1, 1, 0},
    {0, -1, 1},
    {0, -1, -1}
};

static int seed = 55;

static void normalize2(float *a, float *b);
static void normalize3(float *x, float *y, float *z);

/**
 * sets a seed for the noise functions to follow
 */
void noise_seed(int s)
{
    seed = s;
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
 * simple vector normalization. if 'a' and 'b' are both 0,
 * an arbitarary value will be chosen.
 */
static void normalize2(float *a, float *b)
{
    if((*a < FLT_EPSILON && *a > -FLT_EPSILON) && (*b < FLT_EPSILON && *b > -FLT_EPSILON)){
        *a = 0.7f; *b = 0.3f;
    }
    float s = 1.0f / sqrt((*a) * (*a) + (*b) * (*b));
    (*a) = (*a) * s;
    (*b) = (*b) * s;
}

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
    float *grad = GRAD[x1 & 15];
    *xo = grad[0];
    *yo = grad[1];

    //test = ((x1 * (x1 * x1 * 24421 + 544367) + 982451653) & 0x7fffffff);
    //double xd = (1.0f - ((x1 * (x1 * x1 * 24421 + 544367) + 982451653) & 0x7fffffff) / 1073741824.0f);
    //double yd = (1.0f - ((y1 * (y1 * y1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    //*xo = (float) xd;
    //*yo = (float) yd;
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


    float vx0, vy0;
    noise2_randGrad(flx, fly, &vx0, &vy0);
    normalize2(&vx0, &vy0);

    float vx1, vy1;
    noise2_randGrad(flx+1, fly, &vx1, &vy1); 
    normalize2(&vx1, &vy1);

    float vx2, vy2;
    noise2_randGrad(flx+1, fly+1, &vx2, &vy2); 
    normalize2(&vx2, &vy2);

    float vx3, vy3;
    noise2_randGrad(flx, fly+1, &vx3, &vy3); 
    normalize2(&vx3, &vy3);

    float s = vx0 * (frx)        + vy0 * (fry);
    float t = vx1 * ((frx)-1.0f) + vy1 * (fry);
    float u = vx2 * ((frx)-1.0f) + vy2 * ((fry)-1.0f);
    float v = vx3 * (frx)        + vy3 * ((fry)-1.0f);

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
    int x1 = x  * 1 + y * 89 + z * 61;
    int y1 = x * 71 + y * 17 + z * 53;
    int z1 = x * 29 + y * 97 + z * 19;
    x1 = (x1 << 17) ^ x1;
    y1 = (y1 << 17) ^ y1;
    z1 = (z1 << 17) ^ z1;
    *xo = (1.0f - ((x1 * (x1 * x1 * 24421 + 544367) + 982451653) & 0x7fffffff) / 1073741824.0f);
    *yo = (1.0f - ((y1 * (y1 * y1 * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    *zo = (1.0f - ((z1 * (z1 * z1 * 12391 + 331999) + 479001599) & 0x7fffffff) / 1073741824.0f);
}

static void normalize3(float *x, float *y, float *z)
{
    float inv = ((*x)*(*x) + (*y)*(*y) + (*z)*(*z));
    if(inv < FLT_EPSILON)
        inv = FLT_EPSILON;
    float s = 1.0f / inv;
    *x *= s;
    *y *= s;
    *z *= s;
}

float noise3_perlin(float x, float y, float z)
{
    int flx = floor(x);
    int fly = floor(y);
    int flz = floor(z);
    float frx = x-flx;
    float fry = y-fly;
    float frz = z-flz;

    float vx0, vy0, vz0;
    noise3_randGrad(flx, fly, flz, &vx0, &vy0, &vz0);
    normalize3(&vx0, &vy0, &vz0);

    float vx1, vy1, vz1;
    noise3_randGrad(flx+1, fly, flz, &vx1, &vy1, &vz1); 
    normalize3(&vx1, &vy1, &vz1);

    float vx2, vy2, vz2;
    noise3_randGrad(flx+1, fly+1, flz, &vx2, &vy2, &vz2); 
    normalize3(&vx2, &vy2, &vz2);

    float vx3, vy3, vz3;
    noise3_randGrad(flx, fly+1, flz, &vx3, &vy3, &vz3); 
    normalize3(&vx3, &vy3, &vz3);

    float vx4, vy4, vz4;
    noise3_randGrad(flx, fly, flz+1, &vx4, &vy4, &vz4);
    normalize3(&vx4, &vy4, &vz4);

    float vx5, vy5, vz5;
    noise3_randGrad(flx+1, fly, flz+1, &vx5, &vy5, &vz5); 
    normalize3(&vx5, &vy5, &vz5);

    float vx6, vy6, vz6;
    noise3_randGrad(flx+1, fly+1, flz+1, &vx6, &vy6, &vz6); 
    normalize3(&vx6, &vy6, &vz6);

    float vx7, vy7, vz7;
    noise3_randGrad(flx, fly+1, flz+1, &vx7, &vy7, &vz7); 
    normalize3(&vx7, &vy7, &vz7);

    float o = vx0 * (frx)        + vy0 * (fry)          + vz0 * (frz);
    float p = vx1 * ((frx)-1.0f) + vy1 * (fry)          + vz1 * (frz);
    float q = vx2 * ((frx)-1.0f) + vy2 * ((fry)-1.0f)   + vz2 * (frz);
    float r = vx3 * (frx)        + vy3 * ((fry)-1.0f)   + vz3 * (frz);

    float s = vx4 * (frx)        + vy4 * (fry)          + vz4 * (frz-1.0f);
    float t = vx5 * ((frx)-1.0f) + vy5 * (fry)          + vz5 * (frz-1.0f);
    float u = vx6 * ((frx)-1.0f) + vy6 * ((fry)-1.0f)   + vz6 * (frz-1.0f);
    float v = vx7 * (frx)        + vy7 * ((fry)-1.0f)   + vz7 * (frz-1.0f);

    float px0 = lerp(s_curve(frx), s, t);
    float px1 = lerp(s_curve(frx), v, u);
    float px2 = lerp(s_curve(frx), o, p);
    float px3 = lerp(s_curve(frx), q, r);

    float py0 = lerp(s_curve(fry), px0, px1);
    float py1 = lerp(s_curve(fry), px2, px3);


    return lerp(s_curve(frz), py0, py1);
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
