/**
 * scalar.c
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#include "scalar.h"

/**
 * converts a half precision (16-bit) integer value to a float between -1 to 1
 * @param h 16-bit integer
 * @returns floating point value between -1 and 1
 */
float htof(int16_t h)
{
    float ret; 
    if(h >= 0)
    {
       ret = (float) h / (float)INT16_MAX;
    } else {
       ret = (float) h / (float)(INT16_MAX + 1);
    }
    return ret;
}

/**
 * converts a unsigned half precision integer to a value between 0 to 1
 * @param uh 16-bit unsigned value
 * @returns floating point value between 0 to 1
 */
float uhtof(uint16_t uh)
{
    return (float) uh / (float)(UINT16_MAX);
}

/**
 * converts a floating point value between -1 to 1 to a 16-bit signed integer
 * @param f floating point between -1 to 1
 * @returns signed 16-bit integer between INT16_MIN to INT16_MAX
 */
int16_t ftoh(float f)
{
    int16_t ret;
    if(f >= 0.0f){
        ret = (int16_t) (f * INT16_MAX);
    } else {
        ret = (int16_t) (f * (INT16_MAX + 1));
    }
    return ret;
}


/**
 * converts a floating point value between 0 to 1 to an unsigned 16-bit signed integer
 * @param f floating point between 0 to 1
 * @returns signed 16-bit integer between 0 to UINT16_MAX
 */
uint16_t ftouh(float f)
{
    return (uint16_t)(f * UINT16_MAX);
}

/**
 * floating point equality. tests to see if to floating point values are equivilent
 * (within EPSILON)
 * @return boolean equivilency
 */
bool feq(float a, float b)
{
    float ab = a-b;
    return ab <= EPSILON && ab >= -EPSILON;
}

/**
 * returns the sign of a floating point number, normalized to 1.
 */
int fcmp(float a, float b)
{
    float ret = 0;
    if(feq(a, b))
    {
        ret = 0;
    } else if(a < b)
    {
        ret = -1; 
    } else if(a > b)
    {
        ret = 1;
    }
    return ret;
}

/**
 * squares a floating point value
 * @returns val*val. The squared value in the input
 */
float fsq(float val)
{
    return val * val;
}

/**
 * recipricol squareroot approximation
 * @returns 1/sqrt(val), or a close approximation
 */
float frsqrt(float val)
{
#ifdef HIGH_PRECISION
    float r = 1.0 / sqrt(val);
#else
    long i;
    float y, r;

    y = val * 0.5f;
    i = *(long*)(&val);
    i = 0x5f3759df - (i >> 1);
    r = *(float*)(&i);
    r = r * (1.5f - r * r * y);
#endif
    return r;
}

/**
 * floating point clamp. clamps the float pointed to by *val between a and b
 */
float fclamp(float val, float a, float b)
{
    float ret = val;
    if(val < a)
    {
        ret = a;
    } else if (val > b)
    {
        ret = b;
    }
    return ret;
}

float favg(int n, ...)
{
    double sum;

    va_list vl;
    va_start(vl, n);

    int i;
    for(i = 0; i < n; i++)
    {
        sum += va_arg(vl, double); //va_arg auto-promotes to double
    }
    va_end(vl);
    return (float) (sum / (float) n);
}

/**
 * floating point weighted average. each value is preceeded by a weighting, 
 * and the parameter n represents the number of numbers to average
 * so to average {1.0, 2.0, 3.0} with weights of {1.0, 0.5, 0.33} the call
 * will look like: fwavg(3, 1.0, 1.0, 0.5, 2.0, 0.33, 3.0);
 */
float fwavg(int n, ...)
{
    double sum;

    va_list vl;
    va_start(vl, n);

    int i;
    for(i = 0; i < n; i++)
    {
        sum += va_arg(vl, double); //va_arg auto-promotes to double
    }
    va_end(vl);
    return (float) (sum / (float) n);
}

float flavg(int n, float *list)
{
    float ret = 0.0f;
    
    int i;
    for(i = 0; i < n; i++)
    {
        ret += list[i];
    }
    return ret / (float) n;
}

float flwavg(int n, float *list, float *weights)
{
    float ret = 0.0f;

    int i;
    for(i = 0; i < n; i++)
    {
        ret += list[i] * weights[i];
    }
    return ret / (float) n;
}

