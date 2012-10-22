/**
 * search.h
 * obj
 * October 09, 2012
 * Brandon Surmanski
 */

#include <stddef.h>

void *search_linear(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*));

void *search_binary(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*));

void *search_interp(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*));
