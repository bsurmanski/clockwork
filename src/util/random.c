/**
 * random.c
 * clockwork
 * November 15, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "util/math/const.h"
#include "random.h"

// Mersenne Twister constants
#define MT_LEN              624
#define MT_GENPARAM         397
#define MT_DEFAULTSEED      5489
#define MT_GENODDMASK       0x9908b0df
#define MT_INIT_CONST       0x6c078965
#define MT_TEMPER_CONST1    0x9d2c5680
#define MT_TEMPER_CONST2    0xefc60000
#define MT_32BITMASK        0xffffffff
#define MT_HIGHBIT          0x80000000
#define MT_LOWBITS          0x7fffffff

uint32_t MT[MT_LEN];
int MT_index = MT_LEN + 1; //fresh MT values need to be generated, (MT_LEN + 1) means uninitialized

/**
 * initialize mersenne twister random number generator
 */
void random_init(uint32_t seed)
{
    MT[0] = seed;

    int i;
    for(i = 1; i < MT_LEN; i++)
    {
        MT[i] = (MT_INIT_CONST * (MT[i -1] ^ ((MT[i - 1] >> 30) + i))) & MT_32BITMASK; 
    }
}

/**
 * generate new mersenne twister values (624 at a time).
 */
static void generate_new_MT_values(void)
{
    static uint32_t GENMASK[] = {0, MT_GENODDMASK};

    uint32_t y;
    int i;
    for(i = 0; i < MT_LEN - MT_GENPARAM; i++)
    {
        y = (MT[i] & MT_HIGHBIT) | (MT[i+1] & MT_LOWBITS);
        MT[i] = MT[i + MT_GENPARAM] ^ (y >> 1) ^ (GENMASK[y & 0x1]); 
    }
    
    for(;i < MT_LEN - 1; i++)
    {
        y = (MT[i] & MT_HIGHBIT) | (MT[i+1] & MT_LOWBITS);
        MT[i] = MT[i + MT_GENPARAM - MT_LEN] ^ (y >> 1) ^ (GENMASK[y & 0x1]); 
    }
    y = (MT[MT_LEN - 1] & MT_HIGHBIT) | (MT[0] & MT_LOWBITS);
    MT[MT_LEN - 1] = MT[MT_GENPARAM - 1] ^ (y >> 1) ^ (GENMASK[y & 0x1]);
}

/**
 * generates an evenly distributed 32-bit integer using the mersenne twister
 * algorithm
 */
uint32_t random_int(void)
{
    if(MT_index >= MT_LEN)
    {
        if(MT_index == MT_LEN + 1)
        {
            random_init(MT_DEFAULTSEED);
        }

        generate_new_MT_values();
        MT_index = 0;
    }
    uint32_t y = MT[MT_index];
    MT_index++;

    y ^= (y >> 11);
    y ^= (y << 7) & MT_TEMPER_CONST1;
    y ^= (y << 15) & MT_TEMPER_CONST2;
    y ^= (y >> 18);
    return y;
}

/**
 * produces a random integer between [0, max)
 */
uint32_t random_below(uint32_t max)
{
    return floorf(random_random() * max);
}

/**
 * produces a random integer between [min, max)
 */
uint32_t random_range(uint32_t min, uint32_t max)
{
    return min + random_below(max - min);
}

/**
 * random floating point value [0,1]
 */
float random_random(void)
{
    return (((float) random_int()) * (1.0f / (float) UINT32_MAX));
}

/**
 * produces floating point random values uniformly distributed between [min,max]
 */
float random_uniform(float min, float max)
{
    return min + ((max - min) * random_random());
}

/**
 * gaussian distribution random. translated from Python 3.3 random module.
 *
 * Note: a second value can be retrieved at the same time by calculating:
 * float y = sin(x2pi) * g2rad; This value will be independant from the 
 * current sole return value
 */
float random_gauss(float mu, float sigma)
{
    float x2pi = random_random() * TWOPI; 
    float g2rad = sqrt(-2.0f * log(1.0f - random_random()));
    float z = cos(x2pi) * g2rad;
    return mu + (z * sigma);
}
