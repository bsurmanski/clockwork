/**
 * bits.h
 * obj
 * October 08, 2012
 * Brandon Surmanski
 */

#ifndef _OBJ_BITS_H
#define _OBJ_BITS_H

#include <stdint.h>

// Population Count
int bits_popcnt(uint64_t n);

// Leading Zero Count
int bits_tzcnt(uint64_t n);

// Trailing Zero Count
int bits_lzcnt(uint64_t n);

#endif
