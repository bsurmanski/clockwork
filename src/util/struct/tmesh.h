/**
 * tmesh.h
 * obj
 * October 05, 2012
 * Brandon Surmanski
 */

#ifndef _TMESH_H
#define _TMESH_H

#include <stdint.h>

typedef uint16_t Tmesh_face[3];

typedef float Tmesh_vert[3];

typedef struct Tmesh {
    int maxfaces;
    int maxverts;
    int nfaces;
    int nverts;
    Tmesh_face *faces;
    Tmesh_vert *verts;
} Tmesh;

void tmesh_init(Tmesh *m);
void tmesh_finalize(Tmesh *m);
void tmesh_prealloc(Tmesh *m, int f, int v);
void tmesh_addquad(Tmesh *m, float *verts);
void tmesh_addtri(Tmesh *m, float *verts);
int tmesh_addvertices(Tmesh *m, float *vert);
int tmesh_addface(Tmesh *m, int a, int b, int c);

#endif
