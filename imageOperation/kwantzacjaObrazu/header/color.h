#ifndef COLOR_H
#define COLOR_H

#include<stdint.h>

typedef struct _color8_3{
    uint8_t c[3];
} color8_3;

typedef struct _colorLF_5{
    double d[5]; // r g b x y
} colorLF_5;

int colorEqual(color8_3 colorA, color8_3 colorB);

// zwraca w ^2
long long colorDis(color8_3 colorA, color8_3 colorB);

int findClosect(color8_3* palte, int plateNum, color8_3 find);

int findClosect5D(colorLF_5* palte, int plateNum, colorLF_5 find);

#endif