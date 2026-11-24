#ifndef POLY_H
#define POLY_H

#include<stdint.h>

// wszystko jest robinoe w GF(256)

typedef struct _poli{
    uint8_t deg;
    uint8_t* coef;
} poli;

poli mallocPoli(uint8_t deg);

void freePoli(poli toFree);

void poliResize(poli* p, uint8_t deg);

void poliShorten(poli* p);

void printPoli(poli p, char end); 

void poliMinus(poli p1, poli p2, poli* out);

void poliMulti(poli p1, poli p2, poli* out);

void poliDivis(poli p1, poli p2, poli* out, poli* outRest);

uint8_t poliValuAt(poli p, uint8_t x);

#endif