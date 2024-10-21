#ifndef BMP_H
#define BMP_H

#include <stdio.h>

#define BMP_HEADER_SIZE sizeof(struct bmp_header)

enum compression_level { BI_RGB, BI_RLE8, BI_REL4 };

#pragma pack(push, 1)
struct bmp_header {
    // Header
    unsigned short signature;
    unsigned int filesize;
    unsigned int _reserved;
    unsigned int data_offset;

    // Info header
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bits_per_pixel;
    enum compression_level compression;
    unsigned int image_size;
    unsigned int x_pixels_per_meter;
    unsigned int y_pixels_per_meter;
    unsigned int colors_used;
    unsigned int colors_important;
};
#pragma pack(pop)

struct bmp_header bmp_get_header(char *bitmap_data);

#endif
