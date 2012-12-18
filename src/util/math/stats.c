/**
 * stats.c
 * clockwork
 * November 17, 2012
 * Brandon Surmanski
 */

#include <math.h>
#include "stats.h"


uint64_t stats_choose(unsigned int n, unsigned int k)
{
    uint64_t ret  = 1;
    if(n < k)
    {
        ret = 0;
    } else 
    {
        int i;
        for(i = 1; i <= k; i++)
        {
            ret = (ret  * (n - (k - i))) / i; 
        }
    }
    return ret;
}

/**
 * returns the mean (average) of the values
 */
float stats_mean(int n, float *vals)
{
    float sum = 0.0f;
    int i;
    for(i = 0; i < n; i++)
    {
        sum += vals[i];
    }
    return sum / (float) n;
}

/**
 * returns the standard deviation of the values
 */
float stats_stddev(int n, float *vals)
{
    float mean = stats_mean(n, vals);
    float sum = 0.0f;
    int i;
    for(i = 0; i < n; i++)
    {
        sum += (vals[i] - mean) * (vals[i] - mean); 
    }
    return sqrt(sum / (float) n);
}

/**
 * calculates the poisson probability mass function.
 */
float stats_pmfpoisson(float mean, unsigned int expected)
{
    float pn = exp(-mean); 
    int i;
    for(i = 1; i <= expected; i ++)
    {
        pn = pn * (mean / expected); 
    }
    return pn;
}

/**
 * calculates the cumulative distrubution function for the poisson process with a given mean.
 */
float stats_cdfpoisson(float mean, unsigned int expected)
{
    float pn = exp(-mean); 
    float cd = pn; 
    int i;
    for(i = 1; i <= expected; i ++)
    {
        pn = pn * (mean / expected); 
        cd += pn;
    }
    return cd;
}
