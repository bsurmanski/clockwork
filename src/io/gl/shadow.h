/**
 * shadow.h
 * clockwork
 * January 02, 2013
 * Brandon Surmanski
 */

#ifndef _SHADOW_H

struct Mesh;

/**
 * Shadow volume for a given mesh
 */
typedef struct Shadow
{
    float light[3];
    struct Mesh *m;
} Shadow;

void shadow_init(Shadow *s, Mesh *m);
void shadow_finalize(Shadow *s);

void shadow_update(Shadow *s, float light[3]);

#endif
