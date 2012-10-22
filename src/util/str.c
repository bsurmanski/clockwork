/**
 * str.c
 * @file    str.h
 * obj
 * @date    January 14, 2012
 * @author  Brandon Surmanski
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "str.h"

#define ROUND_TO_NEAREST_32(a) ((((a) / 32) + 1) * 32)

/**
 * initializes a new 'str' string. This will allocate new space
 * for a copy of 'data' to be stored. subsequent changes to 'data'
 * will not affect this string
 */
void str_init(str *s, const char *data)
{
    if(data)
    {
        s->len = strlen(data);
        s->max = ROUND_TO_NEAREST_32(s->len+1); //round to nearest 32 bytes
        s->str = malloc(s->max);
        memcpy(s->str, data, s->len + 1);
    } else
    {
        s->len = 0;
        s->max = 0;
        s->str = NULL;
    }
}

/**
 * will create a copy of this string. The string will contain the same data, 
 * and have the same length but each pointer will point to unique structures
 */
str *str_clone(str *src)
{
    str *dst = malloc(sizeof(str));
    dst->len = src->len;
    dst->max = src->max;
    dst->str = malloc(dst->max);
    memcpy(dst->str, src->str, dst->len + 1);
    return dst;
}

/**
 * will reset the value of the string data to that of 'data'.
 * previous values will be discarded. The string must have been
 * previously initialized and since have not been finalized.
 */
void str_reset(str *s, const char *data)
{
    str_finalize(s);
    str_init(s, data);
}

/**
 * will finalize the string such that any allocations made by the module will be
 * released. if storage for the 'str' type was dynamically allocated outside this
 * module, it must still be freed manually.
 */
void str_finalize(str *s)
{
    if(s->str)
    {
        free(s->str);
    }

    s->len = 0;
    s->max = 0;
    s->str = NULL;
}

/**
 * returns the length of the string
 */
int str_len(str *s)
{
    return s->len;
}

/**
 * get a char pointer with the data that this string represents
 */
const char* str_cstr(str *s)
{
    return s->str;
}

/**
 * Naieve string matching. Will upgrade to a better algo when needed
 */
int str_find(str *s, str *b)
{
    int ret = -1;

    if(!s || !b || s->len < b->len)
        return -1;

    int i;
    int j;
    for (i = 0; i < s->len; i++)
    {
        for(j = 0; j < b->len; j++)
        {
            if(s->str[i+j] != b->str[j])
            {
                break;
            }
        }
        if(j == b->len)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

/**
 * same as str_find, except a char pointer can be passed instead.
 * The string 'b' must be null terminating
 * @see str_find
 */
int str_find_cstr(str *s, const char *b)
{
    int blen = strlen(b);
    str search = {blen, blen, (char*)b};
    return str_find(s, &search);
}

/**
 * this will test if 's' starts with 'b'. 
 * s must start with the ENTIRE string of b,
 * otherwise this will return false
 * @returns: true if 's' starts with 'b'.
 */
bool str_startswith(str *s, str *b)
{
    bool ret = true;

    if(!s || !b || s->len < b->len)
        return false;
    
    int i;
    for(i = 0; i <= b->len; i++)
    {
        if(s->str[i] != b->str[i])
        {
            ret = false;
            break;
        }
    }
    return ret;
}

/**
 * This will test if 's' ends with 'b'
 * 's' must end with the ENTIRE string *b for 
 * this function to return true
 * @returns true if 's' ends with the entire string 'b'
 */
bool str_endswith(str *s, str *b)
{
    bool ret = true;

    if(!s || !b || s->len < b->len)
        return false;
    
    int i;
    for(i = 1; i <= b->len; i++)
    {
        if(s->str[s->len - i] != b->str[b->len - i])
        {
            ret = false;
            break;
        }
    }
    return ret;
}

/**
 * checks if string 's' the entire string 'b'.
 * uses the str_find method
 * @see str_find
 */
bool str_contains(str *s, str *b)
{
    return str_find(s, b) != -1;
}

/**
 * concatenates 2 strings. The result will be put in string a
 */
void str_concat(str *a, str *b)
{
    int newlen = a->len + b->len; 
    if(newlen+1 > a->max)
    {
        a->str = realloc(a->str, ROUND_TO_NEAREST_32(newlen + 1));
    }
    strcat(a->str, b->str);
}

void str_concat_cstr(str *a, const char *b)
{
    int newlen = a->len + strlen(b);
    a->str = realloc(a->str, newlen + 1);
    strcat(a->str, b);
}

/*
 * *******
 * OLD STRING METHODS
 * *******
 */

/**
 * retrieves the base name of the file. This is 
 * the file without preceding path and extension
 */
void str_filebase(char *filenm, char *buf)
{
   const char *ext = strchr(filenm, '.');
   const char *sep = strchr(filenm, '/');
   if(!ext)
   {
        ext = strchr(filenm, '\0');
   }
   if(!sep)
   {
        sep = strchr(filenm, '\\');
        if(!sep)
        {
            sep = filenm;
        }
   }
   strncpy(buf, sep, (size_t)(ext-sep));
   buf[(size_t)(ext-sep)+1] = '\0';
}

/**
 * gets the extension of the file
 */
void str_fileext(char *filename, char *buf)
{
    char *p;
    if ((p=strchr(filename, '.')))
    {
        strcpy(buf, p+1);
    } else 
    {
    buf[0] = '\0';
    }
}

/**
 * will read a file into a string. The string will
 * contain the entire file contents.
 * The file is expected to be in ascii format
 */
char *str_newFromFile(char *filenm)
{
    FILE *file = fopen(filenm, "r");
    if(!file) return 0;

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *s = malloc((file_size) + 1);
    memset(s, 0, file_size + 1);
    fread(s, file_size, sizeof(char), file);
    return s;
}
