#ifndef BOUNDS_H
#define BOUNDS_H
#include<stdint.h>
#include"propability.h"
#include"bitIO.h"

typedef struct{
    uint64_t low;
    uint64_t high;
    uint64_t underflow;
} bounds;

bounds* bounds_init();

void bounds_encode(bounds* data, prop* kodeTable, uint16_t znak);

uint16_t bounds_findZnak(bounds* data, prop* kodeTable, uint64_t kode);

void bounds_moveLeft(bounds* data);

int bounds_wrapUnderflow(bounds* data);

void bounds_printFront(bounds* data, bitO* out);

// zwraca o ile przesuną
void bounds_printWsplne(bounds* data, bitO* out);

// nie ignorowac. dokancza niezapisane dane 
void bounds_close(bounds* data, bitO* out);

uint16_t bounds_removeSame(bounds* data);

#endif