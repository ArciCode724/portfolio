#ifndef GF256_H
#define GF256_H

#include<stdint.h>

void printGaloisMutiCtx();

uint8_t galoisAdd(uint8_t a, uint8_t b);

uint8_t galoisMinus(uint8_t a, uint8_t b);

uint8_t galoisPow2(int a);

uint8_t galoisLog2(uint8_t a);

uint8_t galoisMulti(uint8_t a, uint8_t b);

uint8_t galoisDivis(uint8_t a, uint8_t b);

#endif