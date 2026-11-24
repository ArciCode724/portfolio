#include<stdlib.h>
#include<stdio.h>
#include"byteList.h"
#include"bitIo.h"

byteList* byteList_init(){
    byteList* retVal = malloc(sizeof(byteList));
    retVal->txt = malloc(8*sizeof(BYTE));
    retVal->n = 0;
    retVal->nm = 8;

    return retVal;
}

void byteList_free(byteList* toFree){
    free(toFree->txt);
    free(toFree);
};

void byteList_addByte(byteList* wherAdd, BYTE toAdd){
    if(wherAdd->n == wherAdd->nm){
        wherAdd->nm += 8;
        wherAdd->txt = realloc(wherAdd->txt, sizeof(BYTE)*wherAdd->nm);
    }
    wherAdd->txt[wherAdd->n] = toAdd;
    wherAdd->n++;
};

BYTE byteList_popByte(byteList* wherPop){
    if(wherPop->n >= 1){
        wherPop->n--;
        return wherPop->txt[wherPop->n];
    }
    return '\0';
};

BYTE byteList_popFrontByte(byteList* wherPop){
    if(wherPop->n == 0)
        return '\0';
    char c = wherPop->txt[0];
    for(int i=1;i<wherPop->n; i++)
        wherPop->txt[i-1] = wherPop->txt[i];
    wherPop->n--;
    return c;
}

void byteList_print(byteList* toPrint, int type){
    char* printType;
    if(type==0)
        printType = "%c";
    else
        printType = "%2x ";
    for(int i=0; i < toPrint->n; i++)
        printf(printType, toPrint->txt[i]);
};

// -1 <; 0 =; 1 >
int byteList_comp(byteList* a, byteList* b){
    int n = a->n;
    if(a->n > b->n)
        n = b->n;
    for(int i=0; i < n; i++){
        if(a->txt[i] > b->txt[i])
            return 1;
        else if(a->txt[i] < b->txt[i])
            return -1;
    }
    if(a->n == b->n)
        return 0;
    if(a->n > b->n)
        return 1;
    else
        return -1;
}