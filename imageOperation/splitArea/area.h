#ifndef AREA_H
#define AREA_H
#include<stdint.h>
#include"./color.h"
#include"./main.h"

typedef struct {
    uint32_t start_width;
    uint32_t start_height;

    uint32_t width;
    uint32_t height;

    uint64_t variance;
} area;

void printArea(area toPrint);

area areaOfImg(img imgData);

void avgColorInArea(img imgData, area areaOfIntrest);

void spitArea(img imgData, area toSplit, area* out);

#endif