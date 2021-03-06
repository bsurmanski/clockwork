/**
 * armature.h
 * clockwork
 * February 08, 2013
 * Brandon Surmanski
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <alloca.h>

#include "util/math/matrix.h"
#include "util/math/convert.h"
#include "armature.h"

/*
static float *armature_poserotation(Armature *a, int frame, int bone)
{
    return a->poses[frame * a->nbones + bone].rotation;
}

static float *armature_poseposition(Armature *a, int frame, int bone)
{
    return a->poses[frame * a->nbones + bone].position;
}

static float armature_posescale(Armature *a, int frame, int bone)
{
    return a->poses[frame * a->nbones + bone].scale;
}*/

static Pose *armature_getpose(Armature *a, int bone, int frame)
{
    return &a->poses[a->nframes * bone + frame];
}

/*
static void armature_poselerp(Pose *p_out, Pose *p1, Pose *p2)
{
    vec4_avg(p_out->rotation, 2, p1->rotation, p2->rotation); 
    vec3_avg(p_out->position, 2, p1->position, p2->position);
    p_out->scale = (p1->scale + p2->scale) / 2.0f;
}
*/

static void print_armature(Armature *a)
{
    int i, j;
    for(i = 0; i < a->nframes; i++)
    {
        printf("POSE %d:\n", i);
        for(j = 0; j < a->nbones; j++)
        {
            Pose *p = armature_getpose(a, j, i);
            printf("BONE %d: Q(%f, %f, %f, %f), P(%f,%f,%f), S(%f)\n", j,
                    p->rotation[0], p->rotation[1], p->rotation[2], p->rotation[3],
                    p->position[0], p->position[1], p->position[2], p->scale);
        }
    }
    printf("\n");
}

static Bone *armature_getbone(Armature *a, int bone)
{
    Bone *ret = NULL;
    if(bone >= 0 && bone < a->nbones)
    {
        ret = &a->bones[bone]; 
    }
    return ret;
}

static void armature_unpackbones(Armature *a, Armature_header *header, Bone_header *bones)
{
    int i, j;
    for(i = 0; i < header->nbones; i++)
    {
        Bone *bone = &a->bones[i];
        Bone_header *bh = &bones[i]; 
        memcpy(bone->head, bh->head, sizeof(float) * 3);
        memcpy(bone->tail, bh->tail, sizeof(float) * 3);
        bone->id = bh->id;
        bone->nchildren = bh->nchildren;
        bone->parent = armature_getbone(a, bh->parent_id);
        bone->children = NULL; //TODO, set children
        bone->poses = &a->poses[i * header->nframes];
    }
}

int armature_read(Armature *a, const char *filenm)
{
    FILE *file = fopen(filenm, "rb");
    assert(file);

    struct Armature_header h;
    struct Bone_header *bone_headers;
    int nbyte = fread(&h, 1, sizeof(Armature_header), file);
    assert(nbyte == sizeof(Armature_header));

    size_t b_sz;
    a->nbones = h.nbones;
    a->nframes = h.nframes;
    bone_headers = alloca(sizeof(Bone_header) * a->nbones);
    a->bones = malloc(sizeof(Bone) * a->nbones);
    a->poses = malloc(sizeof(Pose) * a->nframes * a->nbones);

    fread(bone_headers, 1, sizeof(Bone_header) * a->nbones, file);
    fread(a->poses, 1, sizeof(Pose) * a->nframes * a->nbones, file);
    fclose(file);

    armature_unpackbones(a, &h, bone_headers);

    //print_armature(a);
    return 0;
}

int armature_addpose(Armature *a)
{
    int index = a->nframes;
    a->nframes++;
    a->poses = realloc(a->poses, sizeof(Pose) * a->nframes * a->nbones);
    int i;
    for(i = a->nbones-1; i >= 0; i--)
    {
        memmove(&a->poses[i * a->nframes], &a->poses[i * index], sizeof(Pose) * index);

        //initialize new pose to default (no rotation, translation, scale)
        a->poses[i * a->nframes + index].rotation[0] = 0.0f;
        a->poses[i * a->nframes + index].rotation[1] = 0.0f;
        a->poses[i * a->nframes + index].rotation[2] = 0.0f;
        a->poses[i * a->nframes + index].rotation[3] = 1.0f;
        a->poses[i * a->nframes + index].position[0] = 0.0f;
        a->poses[i * a->nframes + index].position[1] = 0.0f;
        a->poses[i * a->nframes + index].position[2] = 0.0f;
        a->poses[i * a->nframes + index].scale = 1; 
    }
    return index;
}

void armature_removepose(Armature *a, int pose_i)
{
    a->poses = realloc(a->poses, sizeof(Pose) * (a->nframes-1) * a->nbones);

    int i;
    for(i = 0; i < a->nbones; i++)
    {
        if(pose_i > 0)
        {
            memmove(&a->poses[i * (a->nframes-1)], &a->poses[i * a->nframes], sizeof(Pose) * pose_i);
        }
        if(pose_i < a->nframes-1)
        {
            memmove(&a->poses[i * (a->nframes-1) + pose_i+1], 
                    &a->poses[i * a->nframes + pose_i+1], sizeof(Pose) * ((a->nframes-1) - pose_i));
        }
    }
}

static bool bone_hasparent(Bone *bone)
{
    return bone->parent;
}

void armature_lerp(Armature *a, int frame1, int frame2, float step, mat4 *matrices)
{
    assert(step + FLT_EPSILON >= 0.0f && step <= 1.0f);
    int i;
    for(i = 0; i < a->nbones; i++)
    {
        vec4 q;
        vec3 pos;
        int scale;
        /*
           TODO
        vec4_avg(quat, 2, armature_poserotation(a, frame1, i), armature_poserotation(a, frame2, i));
        vec3_avg(pos, 2, armature_poseposition(a, frame1, i), armature_poseposition(a, frame2, i));
        scale = (armature_posescale(a, frame1, i) + armature_posescale(a, frame2, i)) / 2.0f;
        */
    }
}

void armature_matrices(Armature *a, int frame, mat4 *matrices)
{
    int i;
    for(i = 0; i < a->nbones; i++)
    {
        Bone *bone = &a->bones[i];
        Pose *bpose = &bone->poses[frame];
        mat4 tmp;
        mat4_identity(matrices[i]);

        quat q;
        memcpy(&q, bpose->rotation, sizeof(float) * 4);
             
        quaternion_to_mat4(q, tmp);

        mat4_translate(matrices[i], -bone->head[0], -bone->head[1], -bone->head[2]);
        mat4_mult(matrices[i], tmp, matrices[i]);
        mat4_translate(matrices[i], bone->head[0], bone->head[1], bone->head[2]);
        mat4_translate(matrices[i], bpose->position[0], bpose->position[1], bpose->position[2]);

        if(bone_hasparent(bone))
        {
            mat4_mult(matrices[bone->parent->id], matrices[i], matrices[i]);
        }
    }
}

void armature_finalize(Armature *a)
{

}
