/*
 * line.h
 * obj
 * August 06, 2012
 * Brandon Surmanski
 */

#ifndef _LINE_H
#define _LINE_H

#define LINE2_AX 0
#define LINE2_AY 1 
#define LINE2_BX 2
#define LINE2_BY 3

#define LINE3_AX 0
#define LINE3_AY 1 
#define LINE3_AZ 2 
#define LINE3_BX 3
#define LINE3_BY 4
#define LINE3_BZ 5

typedef float line2[4];
typedef float line3[6];


float line2_len(line2 l);

float line3_len(line3 l);


#endif
