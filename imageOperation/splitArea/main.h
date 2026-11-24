#ifndef MAIN_H
#define MAIN_H
#include<stdint.h>
#include"./color.h"

typedef struct {
    color8_3* pixels;
    uint32_t width;
    uint32_t height;
} img;

#endif