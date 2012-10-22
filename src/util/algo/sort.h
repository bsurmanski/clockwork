/**
 * sort.h
 * obj
 * October, 03, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>

int sort_int_asc(const void *a, const void *b);
int sort_int_desc(const void *a, const void *b);

int sort_issorted(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*));

void sort_swap(void *a, void *b, size_t sz);
void sort_pswap(void *a, void *b);
int sort_cswap(void *a, void *b, size_t sz, int (*compar)(const void*, const void*));

void sort_bubble(void *base, size_t n, size_t size, int (*compar)(const void*, const void*));
void sort_cocktail(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*));
void sort_insertion(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*));
void sort_selection(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*));
