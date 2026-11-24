#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include"bitIo.h"
#include"tree.h"
#include"strdim.h"

typedef struct{
    // index jest rodzajem wczytanej
    BYTE* leter;
    char** inBits;
    int n;
    int bitLaded;
    int low, hie;
} translator;

void translator_loadValues(translator* save, tree* base, bitI* sorse, strdim* path);

translator* translator_init(bitI* sorse);

void translator_free(translator* toFree);

// zwaraca czy znalazł odpowieć
int translator_addBit(translator* base, char bit);

BYTE translator_getLeter(translator* from);

#endif