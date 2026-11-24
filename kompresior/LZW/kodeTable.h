#ifndef KODETABLE_H
#define KODETABLE_H
#include"byteList.h"

typedef struct{
    unsigned int* kode;
    byteList** text;
    unsigned int n, mn;
} kodeTable;

kodeTable* kodeTable_init();

void kodeTable_free(kodeTable* toFree);

void kodeTable_print(kodeTable* toPrint);

// wymaga sortedBy by było 1
// zwraca -1 jak nie ma
int kodeTable_findIndex(kodeTable* whereFind, byteList* find);

// "coAdd" NIE jest kopiowanie
// 1 oznacza ze zwiekszono zromar byta potrzebnego do pomiszczenia indexu
int kodeTable_addText(kodeTable* gdzieAdd, byteList* coAdd);

#endif