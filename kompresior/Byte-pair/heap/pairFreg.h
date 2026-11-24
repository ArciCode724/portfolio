#ifndef PAIRFREG_H
#define PAIRFREG_H

#include"tokens.h"

// open adresing hash tabel

typedef struct {
    pair key;
    int val;
} fregEntry;

typedef struct {
     // key and pos in heap, pos, pos == -1 -> null
    fregEntry* hashMap;
    int hashFill;
    int hashSize;

    fregEntry* heap;
    int heapFill;
    int heapSize;
} pairFreg;

pairFreg* pairFreg_malloc();

void pairFreg_free(pairFreg* toFree);

void pairFreg_print(pairFreg* toPrint);

fregEntry pairFreg_mostOftenPair(pairFreg* frag);

// increment val and create when need
void pairFreg_fregUpPair(pairFreg* frag, pair key);

// decrement val and remove when need
void pairFreg_fregDownPair(pairFreg* frag, pair key);

void pairFreg_pruneHeap(pairFreg* frag, int min);

// void pairFreg_removePair(pairFreg* frag, pair key);


#endif