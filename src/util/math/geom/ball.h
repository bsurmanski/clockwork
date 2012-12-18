/**
 * ball.h
 * clockwork
 * November 25, 2012
 * Brandon Surmanski
 */

#ifndef _BALL_H
#define _BALL_H

#include <stdbool.h>

///Circle
typedef struct Ball2
{
    float radius;
    float center[2];
} Ball2;

///Sphere
typedef struct Ball3
{
    float radius;
    float center[3];
} Ball3;

//Ball2 (circle)
void ball2_init(Ball2 *b, float center[2], float radius);
void ball2_setcenter(Ball2 *b, float center[2]);
void ball2_setradius(Ball2 *b, float radius);
void ball2_scale(Ball2 *b, float scale);
void ball2_move(Ball2 *b, float dv[2]);
bool ball2_collides(Ball2 *a, Ball2 *b);

//Ball3 (sphere)
void ball3_init(Ball3 *b, float center[3], float radius);
void ball3_setcenter(Ball3 *b, float center[3]);
void ball3_setradius(Ball3 *b, float radius);
void ball3_scale(Ball3 *b, float scale);
void ball3_move(Ball3 *b, float dv[3]);
bool ball3_collides(Ball3 *a, Ball3 *b);
void ball3_enclosing(Ball3 *ret, float *points, int n);

#endif
