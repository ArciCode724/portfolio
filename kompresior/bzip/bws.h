#ifndef BWS_H
#define BWS_H

#include <stdint.h>

void printCircle(uint8_t* base, uint32_t n, uint32_t ofset);

// 1 a > b; 0 a = b; -1 a < b
// dziala na kole
short compOfset(uint8_t* base, uint32_t n, uint32_t ofsetA, uint32_t ofsetB);

uint32_t bws_qSortSplit(uint32_t* list, uint8_t* base, uint32_t n, uint32_t start, uint32_t end);

void bws_qSort(uint32_t* list, uint8_t* base, uint32_t n, uint32_t start, uint32_t end);

void bws_shellSort(uint32_t* list, uint8_t* base, uint32_t n);

void bws_radix(uint32_t* list, uint8_t* base, uint32_t n);

// przeksztalaca text z pomoca Burrows–Wheeler transform i zwraca origin
uint32_t make_bws(uint8_t* text, uint32_t n);

void unMake_bws(uint8_t* text, uint32_t n, uint32_t origin);

#endif