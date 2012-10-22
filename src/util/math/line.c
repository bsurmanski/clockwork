/*
 * line.c
 * obj
 * August 06, 2012
 * Brandon Surmanski
 */

#include <math.h>

#include "line.h"

float line2_len(line2 l)
{
    float x = l[LINE2_AX] - l[LINE2_BX];
    float y = l[LINE2_AY] - l[LINE2_BY];
    return sqrt(x*x + y*y);
}

float line3_len(line3 l)
{
    float x = l[LINE3_AX] - l[LINE3_BX];
    float y = l[LINE3_AY] - l[LINE3_BY];
    float z = l[LINE3_AZ] - l[LINE3_BZ];
    return sqrt(x*x + y*y + z*z);
}
