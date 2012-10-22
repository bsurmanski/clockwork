/**
 * tmesh.c
 * obj
 * October 05, 2012
 * Brandon Surmanski
 */

#include <stdlib.h>

#include "tmesh.h"

#define FACE_STEP 16
#define VERT_STEP 32

#define MAX(a,b) ((a) < (b) ? (b) : (a));
#define MIN(a,b) ((a) > (b) ? (b) : (a));

static int reservespace(Tmesh *m, int df, int dv)
{
    int new_nfaces = m->nfaces + df;
    int new_nverts = m->nverts + dv;

    if(m->maxfaces <= new_nfaces + df)
    {
        int step_faces = MAX(new_nfaces, m->nfaces + FACE_STEP);
        m->faces = realloc(sizeof(Tmesh_face) * step_faces);
        m->maxfaces = step_faces;
    }

    if(m->maxverts <= new_nverts + dv)
    {
        int step_faces = MAX(new_nverts , m->nverts + VERT_STEP);
        m->faces = realloc(sizeof(Tmesh_vert) * step_verts);
        m->maxverts = step_verts;
    }

    return || m->maxverts <= m->nverts;
}

void tmesh_init(Tmesh *m)
{
    m->maxfaces = 32;
    m->maxverts = 64;
    m->nfaces = 0;
    m->nverts = 0;
    m->faces = malloc(sizeof(Tmesh_face) * m->maxfaces);
    m->verts = malloc(sizeof(Tmesh_vert) * m->maxverts);
}

void tmesh_finalize(Tmesh *m)
{
    free(m->faces);
    free(m->verts);
}

void tmesh_prealloc(Tmesh *m, int f, int v)
{
    if(f != m->maxfaces)
    {
        m->maxfaces = f;
        m->faces = realloc(m->faces, sizeof(Tmesh_face) * f);
    }

    if(v != m->maxverts)
    {
        m->maxverts = v;
        m->verts = realloc(m->verts, sizeof(Tmesh_vert) * v);
    }
}

void tmesh_addquad(Tmesh *m, float *verts)
{
    reservespace(m, 2, 4);
    memcpy(m->verts[m->nverts], vert, sizeof(Tmesh_vert));
    memcpy(m->verts[m->nverts + 1], &vert[3], sizeof(Tmesh_vert));
    memcpy(m->verts[m->nverts + 2], &vert[6], sizeof(Tmesh_vert));
    memcpy(m->verts[m->nverts + 3], &vert[6], sizeof(Tmesh_vert));
    m->faces[m->nfaces][0] = (uint16_t) m->nverts;
    m->faces[m->nfaces][1] = (uint16_t) m->nverts + 1;
    m->faces[m->nfaces][2] = (uint16_t) m->nverts + 2;
    m->nfaces++;
    m->faces[m->nfaces][0] = (uint16_t) m->nverts;
    m->faces[m->nfaces][1] = (uint16_t) m->nverts + 2;
    m->faces[m->nfaces][2] = (uint16_t) m->nverts + 3;
    m->nfaces++;
    m->nverts += 3;
}

void tmesh_addtri(Tmesh *m, float *verts)
{
    reservespace(m, 1, 3);
    memcpy(m->verts[m->nverts], vert, sizeof(Tmesh_vert));
    memcpy(m->verts[m->nverts + 1], &vert[3], sizeof(Tmesh_vert));
    memcpy(m->verts[m->nverts + 2], &vert[6], sizeof(Tmesh_vert));
    m->faces[m->nfaces][0] = (uint16_t) m->nverts;
    m->faces[m->nfaces][1] = (uint16_t) m->nverts + 1;
    m->faces[m->nfaces][2] = (uint16_t) m->nverts + 2;
    m->nfaces++;
    m->nverts += 3;
}

int tmesh_addvertices(Tmesh *m, float *vert)
{
    reservespace(m, 0, 1);
    memcpy(m->verts[m->nverts], vert, sizeof(Tmesh_vert));
    m->nverts++;
}

int tmesh_addface(Tmesh *m, int a, int b, int c)
{
    reservespace(m, 1, 0);
    m->faces[m->nfaces][0] = (uint16_t) a;
    m->faces[m->nfaces][1] = (uint16_t) b;
    m->faces[m->nfaces][2] = (uint16_t) c;
    m->nfaces++;
}
