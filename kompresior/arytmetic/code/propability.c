#include<stdlib.h>
#include<stdio.h>
#include"..\header\propability.h"

prop* prop_init(uint16_t n){
    prop* retVal = malloc(sizeof(prop));
    retVal->n = n;
    retVal->steps = calloc(n+1, sizeof(uint32_t));
    return retVal;
}

void prop_free(prop* list){
    free(list->steps);
    free(list);
}

void prop_inkris(prop* list, uint16_t what, int by){
    for(int i = what+1; i<=list->n; i++){
        if(list->steps[i] == 0xffffffff){
            printf("overflow ");
        }
        list->steps[i]+=by;
    }
}

void prop_off(prop* list, uint16_t off, uint32_t* down, uint32_t* up, uint32_t* sum){
    *down = list->steps[off];
    *up = list->steps[off+1];
    *sum = list->steps[list->n];
}

propLavels* propLavels_init(){
    propLavels* retVal = malloc(sizeof(propLavels));

    retVal->lowLevels[0] = prop_init(257);
    for(int i=0; i<257; i++){
        prop_inkris(retVal->lowLevels[0], i, 1);
    }

    retVal->lowLevels[1] = prop_init(257);
    prop_inkris(retVal->lowLevels[1], EOP, 1);

    int listSize = 1;
    for(int i = 0; i < MAXKONTEKST; i++){ //MAXKONTEKST
        listSize*=256;
        retVal->highLevels[i] = malloc(listSize*sizeof(prop*));
        for(long j = 0; j<listSize; j++){
            retVal->highLevels[i][j] = prop_init(257);
            prop_inkris(retVal->highLevels[i][j], EOP, 1);
        }
    }

    return retVal;
}

prop* propLavels_findProp(propLavels* list, int level, uint8_t* kontekst, int kontekstSize){
    if(kontekstSize < level-1){
        return NULL;
    }
    if(level<=1){
        return list->lowLevels[level];
    }
    unsigned long index = 0;
    for(int i=0; i<level-1; i++){
        index = index<<8 | kontekst[i];
    }
    return list->highLevels[level-2][index];
}

void propLavels_inkris(propLavels* list, uint16_t what, uint8_t* kontekst, int kontekstSize){
    prop_inkris(list->lowLevels[1], what, 1);
    unsigned long index = 0;
    for(int i=0; i<kontekstSize; i++){
        index = 0;
        for(int j=kontekstSize-1-i; j<kontekstSize; j++){
            index = index<<8 | kontekst[j];
        }
        prop_inkris(list->highLevels[i][index], what, 1);
    }
}

void propLavels_lower(propLavels* list, uint8_t* soonRemowdKontekst, int kontekstSize){
    if(!kontekstSize){
        return;
    }
    prop_inkris(list->lowLevels[1], soonRemowdKontekst[0], -1);
    unsigned long index;
    for(int i = 0; i<kontekstSize-1; i++){
        index = 0;
        for(int j=0; j<=i; j++){
            index = index<<8 | soonRemowdKontekst[j];
        }
        prop_inkris(list->highLevels[i][index], soonRemowdKontekst[i+1], -1);
    }
}