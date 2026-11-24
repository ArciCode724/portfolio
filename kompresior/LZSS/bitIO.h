#ifndef BITIO_H
#define BITIO_H
// #include<stdlib.h>
#include<stdio.h>
typedef unsigned char BYTE;

typedef struct{
    FILE* file;
    BYTE loaded;
    int alerdyRead;
} bitI;

bitI* bitI_init(char* fileName);

//left to right
char bitI_getBit(bitI* reader);

// zwraca EOF
int bitI_getMulkiBits(bitI* reader, int size);

typedef struct{
    FILE* file;
    BYTE loaded;
    int alerdyWriten;
} bitO;

bitO* bitO_init(char* fileName);

// what to tylko '0' bąć '1'
void bitO_putBit(bitO* writer, char what);

void bitO_putMulkiBits(bitO* writer, int what, int size);

void bitI_close(bitI* toFree);

void bitO_close(bitO* toFree);


#endif