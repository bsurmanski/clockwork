/**
 * hash.c
 * @file    hash.h
 * obj
 * @date    July 12, 2012
 * @author  Brandon Surmanski
 */

#include "hash.h"

#define MASK8  0xff
#define MASK16 0xffff
#define MASK24 0xffffff
#define MASK32 0xffffffff
#define MASK64 0xffffffffffffffff

#define FNV32_PRIME 16777619u
#define FNV64_PRIME 1099511628211ull

#define FNV32_OFFSET 2166136261u
#define FNV64_OFFSET 14695981039346656037ull



// Hash algorithm defaults to FNV
uint16_t (*hash16_text)(const const char *a, size_t max) = hash_fnv16_text;
uint16_t (*hash16_data)(const uint8_t *a, size_t sz)  = hash_fnv16_data;
uint32_t (*hash32_text)(const const char *a, size_t max) = hash_fnv32_text;
uint32_t (*hash32_data)(const uint8_t *a, size_t sz)  = hash_fnv32_data;
uint64_t (*hash64_text)(const char *a, size_t max) = hash_fnv64_text;
uint64_t (*hash64_data)(const uint8_t *a, size_t sz)  = hash_fnv64_data;

/**
 * FNV-1a hashing algorithm for 16-bits. For calculations of 16 bits,
 * A 32 bit hash is folded to get a 16 bit value.
 */
uint16_t hash_fnv16_text(const char *a, size_t max)
{
    uint32_t hash = hash_fnv32_text(a, max);
    return (uint16_t) ((hash >> 16) ^ (hash & MASK16));
}

/**
 * FNV-1a hashing algorithm for 16-bits. Unlike the text hashing, this
 * will ignore the significance of null bytes
 */
uint16_t hash_fnv16_data(const uint8_t *a, size_t sz)
{
    uint32_t hash = hash_fnv32_data(a, sz);
    return (uint16_t) ((hash >> 16) ^ (hash & MASK16));
}

/**
 * FNV-1a hashing algorithm for 32-bits. 
 */
uint32_t hash_fnv32_text(const char *a, size_t max)
{
    uint32_t hash = FNV32_OFFSET;
    uint32_t next;

    while ((next = (uint32_t) *a++) && max--)
    {
        hash = hash ^ next; 
        hash = hash * FNV32_PRIME;
    }
    return hash;
}

/**
 * FNV-1a hashing algorithm for 32-bits. Unlike the text hashing,
 * this method will ignore any significance of null bytes
 */
uint32_t hash_fnv32_data(const uint8_t *a, size_t sz)
{
    uint32_t hash = FNV32_OFFSET;
    uint32_t next;

    while (sz--)
    {
        next = (uint8_t) *a++;
        hash = hash ^ next; 
        hash = hash * FNV32_PRIME;
    }
    return hash;
}

uint32_t hash_fnv32_file(const char *filenm)
{
    uint32_t hash = 0;
    uint8_t buf[1024];
    FILE *f = fopen(filenm, "rb");
    while(fread(buf, 1, 1024, f))
    {
        uint32_t next = hash_fnv32_data(buf, 1024);
        hash = hash ^ next;
        hash = hash * FNV32_PRIME;
    }
    fclose(f);
    return hash;
}

/**
 * FNV-1a hashing algorithm for 64-bits. 
 */
uint64_t hash_fnv64_text(const char *a, size_t max)
{
    uint64_t hash = FNV64_OFFSET;
    uint64_t next;

    while ((next = (uint64_t) *a++) && max--)
    {
        hash = hash ^ next; 
        hash = hash * FNV64_PRIME;
    }
    return hash;
}

/**
 * FNV-1a hashing algorithm for 64-bits. 
 */
uint64_t hash_fnv64_data(const uint8_t *a, size_t sz)
{
    uint64_t hash = FNV64_OFFSET;
    uint64_t next;

    while (sz--)
    {
        next = (uint8_t) *a++;
        hash = hash ^ next; 
        hash = hash * FNV64_PRIME;
    }
    return hash;
}
