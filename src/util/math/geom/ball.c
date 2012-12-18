/**
 * ball.c
 * clockwork
 * December 1, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>
#include <string.h>

// required for smallest enclosing bounding ball
#include "util/math/matrix.h"
#include "util/struct/list.h"

#include "ball.h"

/*
 * BALL2
 */

/*
 * BALL3
 */

void ball3_init(Ball3 *b, float center[3], float radius)
{
    memcpy(b->center, center, sizeof(float) * 3);
    b->radius = radius;
}

void ball3_setcenter(Ball3 *b, float center[3])
{
    memcpy(b->center, center, sizeof(float) * 3);
}

void ball3_setradius(Ball3 *b, float radius)
{
    b->radius = radius;
}

void ball3_scale(Ball3 *b, float scale)
{
    b->radius *= scale;
}

void ball3_move(Ball3 *b, float dv[3])
{
    b->center[0] += dv[0];
    b->center[1] += dv[1];
    b->center[2] += dv[2];
}

bool ball3_collides(Ball3 *a, Ball3 *b)
{
    float drsq = (a->radius - b->radius) * (a->radius - b->radius);
    float dv = (a->center[0] - b->center[0]) * (a->center[0] - b->center[0]) +
                (a->center[1] - b->center[1]) * (a->center[1] - b->center[1]) +
                (a->center[2] - b->center[2]) * (a->center[2] - b->center[2]);
    return drsq > dv;
}

/*
 * 
 * SEBB: Smallest Enclosing Bounding Ball problem bellow
 *
 */

struct Encball3
{
    float rsq;
    float center[3];
};

static void enclosing_r(Ball3 *ret, List *points, Iterator *support_end, Iterator *it)
{
    list_first(points, support_end); //move support to front
}

void ball3_enclosing(Ball3 *ret, float *points, int n)
{
    List p_list; // point list
    Iterator support_end;  // ending iterator for support points; 
                            // all points before this (inclsv) in list are support points
                            
    list_init(&p_list, sizeof(float) * 3);
    list_reserve(&p_list, n);
    
    //populate point list with points
    int i;
    for(i = 0; i < n; i++)
    {
        list_addback(&p_list, points + 3 * i);
    }

    Iterator it_start;
    list_first(&p_list, &it_start);
    list_first(&p_list, &support_end);
    enclosing_r(ret, &p_list, &support_end, &it_start);
}
