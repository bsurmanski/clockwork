/**
 * armature.h
 * clockwork
 * February 08, 2013
 * Brandon Surmanski
 */

#ifndef _ARMATURE_H
#define _ARMATURE_H

#include <stdint.h>
#include "util/math/matrix.h"

//XXX colesce bone data into one place?

struct Armature;
struct Pose;

typedef struct Bone
{
    float head[3];
    float tail[3];
    int id;
    int nchildren;
    struct Bone *parent;
    struct Bone **children;
    struct Armature *armature;
    struct Pose *poses;
} Bone;

typedef struct Bone_header
{
    float head[3];
    float tail[3];
    uint8_t id;
    uint8_t parent_id;
    uint8_t nchildren;
    uint8_t PADDING[5];
} Bone_header;

typedef struct Pose
{
    float rotation[4];  ///< Rotation Quaternion from default pose
    float position[3];  ///< Offset from default pose
    float scale;        ///< scale from default pose
} Pose;

typedef struct Armature_header
{
    uint8_t magic[3];
    uint8_t version;
    uint8_t nbones;
    uint8_t nframes; //TODO: more than 255 frames?
    uint8_t name[16];
    uint8_t padding[10];
} Armature_header;

typedef struct Armature
{
    uint8_t nbones;
    uint8_t nframes; //TODO: more than 255 frames
    struct Bone *bones; 
    struct Pose *poses; //array of poses
} Armature;

int armature_read(Armature *a, const char *filenm);
void armature_lerp(Armature *a, int frame1, int frame2, float step, mat4 *matrices);
int armature_addpose(Armature *a);
void armature_removepose(Armature *a, int pose_i);
void armature_posecopy(Armature *a, int frame_to, int frame_from);
void armature_matrices(Armature *a, int frame, mat4 *matrices);
void armature_finalize(Armature *a);

#endif
