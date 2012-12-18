/**
 * random.h
 * clockwork
 * November 15, 2012
 * Brandon Surmanski
 */

#ifndef _RANDOM_H
#define _RANDOM_H

#include <stdint.h>

void     random_init(uint32_t seed);
uint32_t random_int(void);
uint32_t random_below(uint32_t max);
uint32_t random_range(uint32_t min, uint32_t max);
float    random_random(void);
float    random_uniform(float min, float max);
float    random_gauss(float mu, float sigma);

#endif
