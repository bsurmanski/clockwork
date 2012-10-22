/**
 * bits.c
 * obj
 * October 08, 2012
 * Brandon Surmanski
 */

#include <assert.h>

#include "bits.h"

// fast popcount constants
const uint64_t m1  = 0x5555555555555555; //binary: 0101...
const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...

static int fast_popcount(uint64_t x);

/**
 * fast popcount implimentation for when there is no assembly instruction
 */
static int fast_popcount(uint64_t x) 
{
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits 
    x += x >>  8;  //put count of each 16 bits into their lowest 8 bits
    x += x >> 16;  //put count of each 32 bits into their lowest 8 bits
    x += x >> 32;  //put count of each 64 bits into their lowest 8 bits
    return x & 0x7f;
}

int bits_popcnt(uint64_t n)
{
    int ret;
#if defined(__POPCNT__) || defined(__SSE4_2__)
        ret = __builtin_popcount(n);
#else
        ret = fast_popcount(n);
#endif
    return ret;
}

/**
 * trailing zero count. 
 * undefined on argument of zero
 */
int bits_tzcnt(uint64_t n)
{
    assert(n);

    int ret;
    ret = __builtin_ctzl(n);
    return ret;
}

/**
 * leading zero count.
 * undefined on argument of zero
 */
int bits_lzcnt(uint64_t n)
{
    assert(n);

    int ret;
    ret = __builtin_clzl(n);
    return ret;
}
