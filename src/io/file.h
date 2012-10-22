/**
 * file.h
 * obj
 * January 12, 2012
 * Brandon Surmanski
 */

#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>
#include <stdbool.h>

char *file_toString(char *filename);
int file_readLine(FILE *fp, char *buf, size_t cnt);
int file_size(FILE *fp);
char *file_ext(char *filename);
bool file_exists(char *filenm);

#endif
