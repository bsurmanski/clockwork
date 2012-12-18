/**
 * str.h
 * @file    str.h
 * obj
 * @date    January 14, 2012
 * @author  Brandon Surmanski
 */

#ifndef _STR_H
#define _STR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct str 
{
    int len;
    int max;
    char *str;
} str;

void str_init(str *s, const char *data);
str *str_clone(str *src);
void str_reset(str *s, const char *data);
void str_finalize(str *s);
int str_len(str *s);
const char* str_cstr(str *s);
int str_find(str *s, str *b);
int str_find_cstr(str *s, const char *b);
bool str_startswith(str *s, str *b);
bool str_endswith(str *s, str *b);
bool str_contains(str *s, str *b);
void str_concat(str *a, str *b);
void str_concat_cstr(str *a, const char *b);
int str_dist(str *a, str *b);

void str_filebase(char *filenm, char *buf);
void str_fileext(char *filename, char *buf);
char *str_newFromFile(char *filename);

#endif
