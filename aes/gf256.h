#ifndef GF256_H
#define GF256_H

#include<stdint.h>

void printgfMutiCtx();

uint8_t gfAdd(uint8_t a, uint8_t b);

uint8_t gfPow3(int a);

uint8_t gfLog3(uint8_t a);

uint8_t gfMulti(uint8_t a, uint8_t b);

uint8_t gfDivis(uint8_t a, uint8_t b);

#endif