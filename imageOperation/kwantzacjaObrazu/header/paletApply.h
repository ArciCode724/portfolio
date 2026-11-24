#ifndef PALETAPPLY_h
#define PALETAPPLY_h

// #include<stdint.h>
#include"color.h"

void simpleApply1D(uint8_t* data, int dataNum, uint8_t* palet, int paletNum);

void simpleApply(color8_3* data, int dataNum, color8_3* palet, int paletNum);

void simpleApply5D(color8_3* data, int dataWitdh, int dataHeight, colorLF_5* palet, int paletNum);

void ditherApply(color8_3* data, int dataWitdh, int dataHeight, color8_3* palet, int paletNum);

void ditherApply1D(uint8_t* data, int dataWitdh, int dataHeight, uint8_t* palet, int paletNum);

void orderApply1D(uint8_t* data, int dataWitdh, int dataHeight, uint8_t* palet, int paletNum);

void orderApply3D(color8_3* data, int dataWitdh, int dataHeight, color8_3* palet, int paletNum);

#endif