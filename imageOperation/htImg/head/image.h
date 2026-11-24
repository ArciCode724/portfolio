#ifndef IMAGE_H
#define IMAGE_H

#include<stdint.h>

typedef struct _color8_3{
    uint8_t c[3];
} color8_3;

typedef struct _image{
    color8_3* pixels;
    uint32_t width;
    uint32_t height;
} image;

void loadBlock(image img, uint32_t x, uint32_t y, color8_3* out, uint32_t outSize);

void saveBlock(image img, uint32_t x, uint32_t y, color8_3* in, uint32_t inSize);

#endif