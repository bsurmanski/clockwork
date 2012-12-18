/**
 * stats.h
 * clockwork
 * November 17, 2012
 * Brandon Surmanski
 */

#ifndef _STATS_H
#define _STATS_H

#include <stdint.h>

uint64_t stats_choose(unsigned int n, unsigned int k);
float stats_mean(int n, float *vals);
float stats_stddev(int n, float *vals);
float stats_pmfpoisson(float mean, unsigned int expected);
float stats_cdfpoisson(float mean, unsigned int expected);

#endif
