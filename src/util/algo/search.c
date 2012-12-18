/**
 * search.c
 * obj
 * October 09, 2012
 * Brandon Surmanski
 */

#include "search.h"

#define OFFSET(a,sz,b) ((void*)(((char*)(a)) + ((sz) * (b))))

static int midpoint(int a, int b)
{
    return a + ((b - a) / 2);
}

void *search_linear(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*))
{
    void *ret = NULL;

    int i;
    for(i = 0; i < n; i++)
    {
        void *b = OFFSET(base, sz, i);
        if(compar(key, b) == 0)
        {
            ret = b;
            break;
        }
    }
    return ret;
}

void *search_binary(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*))
{
    void *ret = NULL;
    int low = 0;
    int high = n - 1;
    int mid;

    while(low < high)
    {
        mid = midpoint(low, high);
        void *b = OFFSET(base, sz, mid);
        int cmp = compar(key, b);
        if (cmp < 0)
        {
            low = mid + 1; 
        } else if (cmp > 0)
        {
            high = mid - 1; 
        } else 
        {
            ret = b;
            break;
        }
    }
    return ret;
}

/*
void *search_interp(const void *key, void *base, 
                size_t n, size_t sz, 
                int (*compar)(const void*, const void*))
{
    void *ret;
    int low = 0;
    int high = n - 1;
    int mid;

    while(low < high)
    {
        void *a = OFFSET(base, sz, low);
        void *b = OFFSET(base, sz, high);
        int acmp = compar(key, a);
        int bcmp = compar(key, b);
    }
}*/
