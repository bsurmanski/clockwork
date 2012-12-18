/**
 * file.c
 * obj
 * January 12, 2012
 * Brandon Surmanski
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

/**
 * will read a file into a string. The string will
 * contain the entire file contents.
 * The file is expected to be in ascii format
 */
char *file_toString(char *filename)
{
    FILE *file = fopen(filename, "r");
    if(!file) return 0;

    fseek(file, 0, SEEK_END);
    size_t file_sz = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *str = malloc((file_sz) + 1);
    memset(str, 0, file_sz + 1);
    fread(str, file_sz, sizeof(char), file);
    fclose(file);
    return str;
}

/**
 * reads a line from f into buf. A maximum of cnt characters
 * are read. if a newline character is reached, the function returns.
 * if an EOF is reached, the function returns.
 *
 * Returns: the number of characters read
 */
int file_readLine(FILE *f, char *buf, size_t cnt)
{
    char tmp;
    int max = cnt;
    if(feof(f)) return -1;
    while((tmp = fgetc(f)) != '\n' && !feof(f) && cnt){
        *buf++ = tmp;
        cnt--;
    }
    return max - cnt;
}

/**
 * returns the size of the file in bytes. the file pointer position
 * will remain unchanged.
 */
size_t file_size(FILE *fp)
{
    int cur = ftell(fp);
    int size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, cur, SEEK_SET);
    return size;
}

char *file_ext(char *filename)
{
    char *p;
    if ((p=strchr(filename, '.')))
        return p+1;
    return 0;
}

bool file_exists(char *filenm)
{
    int eno = errno; //preserve errno
    bool ret = access(filenm, F_OK);
    errno = eno;
    return ret;
}
