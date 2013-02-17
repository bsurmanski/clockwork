/**
 * noise.h
 * @file    noise.h
 * noise
 * @date    November 14, 2011
 * @author  Brandon Surmanski
 */

#ifndef _NOISE_H
#define _NOISE_H

void noise_init(int s);
void noise_finalize(void);
float noise1_random(int x);
float noise1_value(float x);
float noise1_perlin(float x);
float noise1_fracPerlin(float x, int n);
float noise1_terbulence(float x, int n);

float noise2_random(int x, int  y);
float noise2_value(float x, float y);
float noise2_perlin(float x, float y);
float noise2_fracPerlin(float x, float y, int n);
float noise2_terbulence(float x, float y, int n);

float noise3_random(int x, int  y, int z);
float noise3_value(float x, float y, float z);
float noise3_perlin(float x, float y, float z);
float noise3_fracPerlin(float x, float y, float z, int n);
float noise3_terbulence(float x, float y, float z, int n);

#endif
