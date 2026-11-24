#ifndef KDTREE_H
#define KDTREE_H

typedef struct _kdtree{
    void* data;
    int* split; // // poczatek czesic >= wszysko przed <

    int numData;
    int elemSize;
    int dimention; // liczba wymiarow

    int (*comp)(const void* a, const void* b, int d); // wieksze/mniesze w jednym wymiarze
} kdtree;

kdtree* createKdtree(void* data, int numData, int elemSize, int dimention, int (*comp)(const void* a, const void* b, int d));

void freeKdtree(kdtree* toFree);

void kdtreePrint(kdtree* tree, void (*printElem)(const void* a));

void kdtreeFindClosest(void* out, const void* closTo, kdtree* tree, double (*dist)(const void* a, const void* b, int d));

#endif