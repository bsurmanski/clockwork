/**
 * sort.c
 * obj
 * October 03, 2012
 * Brandon Surmanski
 */

#include <alloca.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "sort.h"

#define offset(a,sz,b) ((void*)(((char*)(a)) + ((sz) * (b))))

/**
 * comparator for integer sorting in asscending order
 */
int sort_int_asc(const void *a, const void *b)
{
    int *ia = (int*) a;
    int *ib = (int*) b;
    return *ia - *ib; 
}

/**
 * comparator for integer sorting in descending order
 */
int sort_int_desc(const void *a, const void *b)
{
    int *ia = (int*) a;
    int *ib = (int*) b;
    return *ib - *ia; 
}

/**
 * will test if an array is sorted
 */
int sort_issorted(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*))
{
    int ret = 1;

    int i;
    for(i = 1; i < n; i++)
    {
        void *a = offset(base, sz, i - 1);
        void *b = offset(base, sz, i);
        if(compar(a, b) > 0)
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

/**
 * simple swap of 2 values
 */
void sort_swap(void *a, void *b, size_t sz)
{
    void *tmp = alloca(sz);
    memcpy(tmp, a, sz);
    memcpy(a, b, sz);
    memcpy(b, tmp, sz);
}

void sort_pswap(void *a, void *b)
{
    void *tmp;
    tmp = a;
    a = b;
    b = tmp;
}

int sort_cswap(void *a, void *b, size_t sz, int (*compar)(const void*, const void*))
{
    int to_swap = (compar(a,b) > 0);
    if(to_swap)
    {
        sort_swap(a, b, sz);
    }
    return to_swap;
}

/*
void sort_pcswap(void *a, void *b, int (*compar)(const void*, const void*))
{
    if(compar(a,b) > 0)
    {
        sort_pswap(a, b);
    }
}
*/

/**
 * basic bubble sort
 */
void sort_bubble(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*))
{
    int i,j,sorted = false;
    for(i = n - 1; i > 0 && !sorted; i--)
    {
        sorted = true;
        for(j = 0; j < i; j++)
        {
            void *a = offset(base, sz, j);
            void *b = offset(base, sz, j + 1);

            if(compar(a,b) > 0)
            {
                sort_swap(a,b,sz);
                sorted = false;
            }
        }
    }
}

/**
 * cocktail sort
 */
void sort_cocktail(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*))
{
    int i,j,sorted = false;
    for(i = 0; i < n && !sorted; i++)
    {
        sorted = true;
        for(j = i; j < n-i-1; j++)
        {
            void *a = offset(base, sz, j);
            void *b = offset(base, sz, j + 1);

            if(compar(a,b) > 0)
            {
                sort_swap(a,b,sz);
                sorted = false;
            }
        }

        for(j = n-i-1; j > i; j--)
        {
            void *a = offset(base, sz, j - 1);
            void *b = offset(base, sz, j);

            if(compar(a,b) > 0)
            {
                sort_swap(a,b,sz);
                sorted = false;
            }
        }
    }
}

/**
 * insertion sort
 */
void sort_insertion(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*))
{
    int i,j;
    for(i = 1; i < n; i++)
    {
        void *a = offset(base, sz, i - 1);
        void *b = offset(base, sz, i);
        for(j = i; j > 0 && compar(a,b) > 0; j--)
        {
            sort_swap(a,b,sz);
            a = offset(base, sz, j - 2);
            b = offset(base, sz, j - 1);
        }
    }
}

/**
 * selection sort
 */
void sort_selection(void *base, size_t n, size_t sz, int (*compar)(const void*, const void*))
{
    int i,j;
    for(i = 0; i < n; i++)
    {
        void *a = offset(base, sz, i);
        void *least = offset(base, sz, i);
        for(j = i + 1; j < n; j++)
        {
            void *b = offset(base, sz, j);
            if (compar(least, b) > 0)
            {
                least = b;
            }
        }
        if(a != least)
        {
            sort_swap(a, least, sz);
        }
    }
}
