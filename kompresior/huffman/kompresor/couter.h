#ifndef COUTER_H
#define COUTER_H
#include<stdio.h>
#include"kompiler.h"

typedef struct couter{
    BYTE* what;
    int* ile;
    int n;
} couter;

couter* couter_init();

void couter_free(couter* toTree);

void couter_count(couter* write, char* fileName);

void couter_swap(couter* base, int a, int b);

// zwaca gdznie pierwasza partycja się konczy w sposub otwarty 
int couter_qsort_split(couter* sort, int start, int end);

// sort po ->ile DESC
void couter_qsort(couter* sort, int start, int end);

void couter_print(couter* toPrint);

void couter_remowEmpy(couter* renmowFrom);

double couter_entory(couter* of);

#endif