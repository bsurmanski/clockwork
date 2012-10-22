/*
 * hash.h
 * @file    hash.h
 * obj
 * @date    July 12, 2012
 * @author  Brandon Surmanski
 */

#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

extern uint16_t (*hash16_text)(const char *a, size_t max);
extern uint16_t (*hash16_data)(const uint8_t *a, size_t sz);
extern uint32_t (*hash32_text)(const char *a, size_t max);
extern uint32_t (*hash32_data)(const uint8_t *a, size_t sz);
extern uint64_t (*hash64_text)(const char *a, size_t max);
extern uint64_t (*hash64_data)(const uint8_t *a, size_t sz);

uint16_t hash_fnv16_text(const char *a, size_t max);
uint16_t hash_fnv16_data(const uint8_t *a, size_t sz);
uint32_t hash_fnv32_text(const char *a, size_t max);
uint32_t hash_fnv32_data(const uint8_t *a, size_t sz);
uint32_t hash_fnv32_file(const char *filenm);
uint64_t hash_fnv64_text(const char *a, size_t max);
uint64_t hash_fnv64_data(const uint8_t *a, size_t sz);

#endif
