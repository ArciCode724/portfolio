#ifndef COMPRESCOLORS_H
#define COMPRESCOLORS_H

#include"color.h"

typedef struct _wheighedColor{
    color8_3 color;
    uint32_t wheight;
} wheighedColor;

wheighedColor* mallocCompresData(color8_3* data, int dataNum, uint32_t* newSize);

typedef struct _wheighedColor1D{
    uint8_t color;
    uint32_t wheight;
} wheighedColor1D;

wheighedColor1D* mallocCompresData1D(uint8_t* data, int dataNum, uint32_t* newSize);

#endif