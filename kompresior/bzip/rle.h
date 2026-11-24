#ifndef RLE_H
#define RLE_H

#include <stdint.h>

// 0 R0
// 1 R1
// 2-256 byte type -1
// 257 end of block

#define R0 0
#define R1 1

typedef struct{
    uint16_t* text;
    uint32_t n;
} rleText;

rleText* rleText_init(uint32_t size);

void rleZeros(rleText* save, uint8_t* list, uint32_t n);

// ile znakow odkowoano
uint32_t unRleZeros(uint8_t* save, rleText* list);

// jesli napotkasz trzery takie same znaki nastepy znak pokzuje ile razy powtara sie
// np. aaaaaaaa -> aaaa4; aaaa -> aaaa0
// np. aaaa4 -> aaaaaaaa; aaaa0 -> aaaa

int getRleFore(FILE* in);

void putRleFore(FILE* out, uint8_t outChar);

#endif