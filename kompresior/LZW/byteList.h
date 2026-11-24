#ifndef byteList_H
#define byteList_H
#include"bitIo.h"

// opereowanie beze ograniczenie \0
typedef struct{
    BYTE* txt;
    int n, nm;
} byteList;

byteList* byteList_init();

void byteList_free(byteList* toFree);

void byteList_addByte(byteList* wherAdd, BYTE toAdd);

BYTE byteList_popByte(byteList* wherPop);

BYTE byteList_popFrontByte(byteList* wherPop);

// 0 as char; 1 as hex
void byteList_print(byteList* toPrint, int type);

int byteList_comp(byteList* a, byteList* b);

#endif