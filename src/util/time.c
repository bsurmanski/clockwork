/**
 * time.c
 * obj
 * September 18, 2012
 * Brandon Surmanski
 */

#include <time.h>
#include <sys/time.h>

#include "time.h"

float timeval_tick(struct timeval *t)
{
    float dt;
    struct timeval t2;
    gettimeofday(&t2, NULL);

    dt = (1000.0f * (t2.tv_sec - t->tv_sec) +
         ((t2.tv_usec - t->tv_usec) / 1000.0f));

    t->tv_sec = t2.tv_sec;
    t->tv_usec = t2.tv_usec;
    return dt;
}
