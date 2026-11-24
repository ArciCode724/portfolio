#ifndef KWANT_h
#define KWANT_h

#include<stdint.h>
#include"color.h"

void kwantLight(color8_3* data, int dataNum, int grupNum);

void kwant1dGrow(uint8_t* data, int numData, uint8_t* outPalet, int* paletSize);

void kmeanKwant(color8_3* data, int dataNum, color8_3* outPalet, int* pointPaletSize);

void kmeanKwant5D(color8_3* data, int dataWitdh, int dataHeight, colorLF_5* outPalet, int* pointPaletSize);

// void mediumKwant1D(uint8_t* data, int dataNum, uint8_t* outPalet, int* pSizePalet)

void mediumKwant3D(color8_3* data, int dataNum, color8_3* outPalet, int* paletSize);

void meanShift(color8_3* data, int dataNum, int range);

void hierarchicalKwant1D(uint8_t* data, int dataNum, uint8_t* outPalet, int* paletSize);

void hierarchicalKwant3D(color8_3* data, int dataNum, color8_3* outPalet, int* paletSize);

#endif