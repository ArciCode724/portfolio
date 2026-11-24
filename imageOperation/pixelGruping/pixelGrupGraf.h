#ifndef pixelGrupGraf_h
#define pixelGrupGraf_h

#include<stdint.h>

typedef struct _color8_3{
    uint8_t c[3];
} color8_3;

typedef struct _pixelGrupe{
    color8_3 color;

    uint64_t* pixelsId;
    uint64_t pixelsNum;

    struct _pixelGrupe** neighborsId;
    uint64_t neighborsNum;
} pixelGrupe;

typedef struct _connection{
    pixelGrupe* between[2]; // [0] < [1] && [0] != [1]
    uint32_t distance;
} connection;

typedef struct _pixelGrupGraf{
    pixelGrupe* grups;

    connection* conHeap;
    uint64_t heapNum;

    struct {pixelGrupe* key[2]; uint64_t val;}* conHashMap; // position of specific conection
    uint64_t hashNum;
} pixelGrupGraf;

pixelGrupGraf* create_pixelGrupGraf(color8_3* imageData, uint32_t width, uint32_t height);

void free_pixelGrupGraf(pixelGrupGraf* toFree);

connection minDistance(pixelGrupGraf* graf);

void pixelGrupGraf_mergeConnection(pixelGrupGraf* graf, connection con);

void pixelGrupGrafToimage(pixelGrupGraf* graf, color8_3* imageDataOut);

#endif