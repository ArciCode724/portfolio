#ifndef LINEMEM_H
#define LINEMEM_H

#include<stdint.h>

typedef struct{
    uint8_t* list;
    uint32_t n, mn, pos;
} lineMem;

lineMem* lineMem_init(uint32_t size);

// -1 nic nie usunieto reszta co usunieto
int16_t lineMem_add(lineMem* list, uint8_t addC);

int lineMem_getSoonRemoveKontekst(lineMem* list, uint8_t* out, int size);

#endif