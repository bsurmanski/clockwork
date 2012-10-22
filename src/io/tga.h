/**
 * tga.h
 * obj
 * May 29, 2012
 * Brandon Surmanski
 */

#ifndef _TGA_H
#define _TGA_H

#include <stdint.h>

typedef struct tga_header {
    uint8_t id_len;
    uint8_t cmap_type;
    uint8_t img_type;

    //Colormap Specification
    struct tga_header_cmap {
        uint16_t offset;
        uint16_t len;
        uint8_t entry_sz;
    } cmap;

    //Image specification
    struct tga_header_img {
        uint16_t xorg;
        uint16_t yorg;
        uint16_t w;
        uint16_t h;
        uint8_t depth;
        uint8_t discriptor;
    } img;
} tga_header;

int tga_pxl_sz(tga_header *h);
int tga_colormap_sz(tga_header *h);
int tga_image_sz(tga_header *h);

int tga_header_is_valid(tga_header *h);

int tga_header_read(int fd, tga_header *h);
int tga_image_read(int fd, tga_header *h, void *buf);


#endif
