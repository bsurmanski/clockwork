/*
 * scalar.h
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#ifndef _SCALAR_H
#define _SCALAR_H

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#define EPSILON FLT_EPSILON

float       htof(int16_t h);
float       uhtof(uint16_t uh);
int16_t     ftoh(float f);
uint16_t    ftouh(float f);
bool        feq(float a, float b);
int         fcmp(float a, float b);
float       fsq(float val);
float       frsqrt(float val);
float       fclamp(float val, float a, float b);
float       favg(int n, ...);
float       fwavg(int n, ...);
float       flavg(int n, float *list);
float       flwavg(int n, float *list, float *weights);

#endif
