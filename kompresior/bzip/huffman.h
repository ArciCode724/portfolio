#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<stdint.h>
#include"bitIO.h"

// 0 R0
// 1 R1
// 2-256 byte type -1
// 257 end of block

#define EOB 257
#define CODESNUM 258

typedef struct{
    uint8_t* bits;
    uint16_t n;
} bitList;

typedef struct tree{
    uint16_t value;
    struct tree *left;
    struct tree *right;
} kodeTree;

// lista ma dlugosc 258
// index listy odpowiada znakowi
bitList* CanonicalHuffman_innit();

void koding_print(bitList* koding);

kodeTree* kodeTree_init(uint16_t value);

void kodeTree_free(kodeTree* toFree);

void kodeTree_print(kodeTree* toPrint, uint16_t deep);

void TreeAddValue(kodeTree* tree, uint16_t value, bitList path, uint16_t deep);

kodeTree* CanonicalHuffmanToTree(bitList* code);

// deep zaczyna sie od 0
void kodeTree_readLeafLens(bitList* save, kodeTree* tree, uint16_t deep);

// generuje numerly w liscie CanonicalHuffman na bazie ustawionych rozmiarow
// zaklada ze przestrzen jest zalokowana
void generateCanonicalHuffman(bitList* save);

void makeCanonicalHuffman(bitList* save, uint16_t* list, uint32_t n);

void writeInCanonicalHuffman(bitO* out, bitList* kode, uint16_t leter);

void writeCanonicalHuffman(bitO* out, bitList* list);

uint16_t readInCanonicalHuffman(bitI* in, kodeTree* kode);

void readCanonicalHuffman(bitI* in, bitList* list);

#endif