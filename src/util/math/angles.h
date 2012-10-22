/**
 * angles.h
 * obj
 * August 20, 2012
 * Brandon Surmanski
 */

#ifndef _ANGLES_H
#define _ANGLES_H

typedef float angles[3];

#define PITCH   0
#define YAW     1
#define ROLL    2

void angles_set(angles a, float pitch, float yaw, float roll);
void angles_add(angles a, angles b, angles dst);
void angles_sub(angles a, angles b, angles dst);
void angles_clamp(angles a, angles max, angles min);
void angles_normalize360(angles a);
void angles_normalize180(angles a);

extern void (*angles_normalize)(angles a);

#endif
