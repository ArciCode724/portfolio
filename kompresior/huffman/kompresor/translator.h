#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include"kompiler.h"
#include"tree.h"
#include"strdim.h"

typedef struct{
    // index jest rodzajem wczytanej
    char** inBits;
    int n;
} translator;

void tree_checkLeafs(tree* from, translator* save, strdim* path);

// pamiec nie jest rozszerzna
translator* tree_to_translator(tree* from);

void translator_free(translator* toFree);

void translator_print(translator* toPrint);

double couter_newLen(couter* cou, translator* tra);

#endif