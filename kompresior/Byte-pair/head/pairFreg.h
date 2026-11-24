#ifndef PAIRFREG_H
#define PAIRFREG_H

#include"tokens.h"

// open adresing hash tabel

typedef struct {
    pair* keys;
    int*  vals; // tylkow wartosci 1...max, 0 to null
    size_t size;
    size_t fill;

    pair* newKeys;
    size_t newKeysNum;
} pairFreg;

pairFreg* pairFreg_malloc();

void pairFreg_free(pairFreg* toFree);

void pairFreg_print(pairFreg* toPrint);

size_t pairFreg_mostOftenPair(pairFreg* frag);

// increment val and create when need
void pairFreg_fregUpPair(pairFreg* frag, pair key);

void pairFreg_fregDownPairMin(pairFreg* frag, pair key, int minFreg);

// decrement val and remove when need
void pairFreg_fregDownPair(pairFreg* frag, pair key);

void pairFreg_checkBound(pairFreg* frag, int min);

void pairFreg_removePair(pairFreg* frag, pair key);


#endif