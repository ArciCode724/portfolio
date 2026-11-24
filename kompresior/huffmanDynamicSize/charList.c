#include<stdlib.h>
#include<stdio.h>
#include"charList.h"

charList* charList_init(){
    charList* retVal = malloc(sizeof(charList));
    retVal->txt = malloc(8*sizeof(char));
    retVal->n = 0;
    retVal->nm = 8;

    return retVal;
}

void charList_free(charList* toFree){
    free(toFree->txt);
    free(toFree);
};

void charList_addChar(charList* wherAdd, char toAdd){
    if(wherAdd->n == wherAdd->nm){
        wherAdd->nm += 8;
        wherAdd->txt = realloc(wherAdd->txt, sizeof(char)*wherAdd->nm);
    }
    wherAdd->txt[wherAdd->n] = toAdd;
    wherAdd->n++;
};

char charList_popChar(charList* wherPop){
    if(wherPop->n >= 1){
        wherPop->n--;
        return wherPop->txt[wherPop->n];
    }
    return '\0';
};

char charList_popFrontChar(charList* wherPop){
    if(wherPop->n == 0)
        return '\0';
    char c = wherPop->txt[0];
    for(int i=1;i<wherPop->n; i++)
        wherPop->txt[i-1] = wherPop->txt[i];
    wherPop->n--;
    return c;
}

void charList_print(charList* toPrint){
    for(int i=0; i < toPrint->n; i++)
        printf("%c", toPrint->txt[i]);
    printf("\n");
};

int charList_comp(charList* a, charList* b){
    if(a->n != b->n)
        return 0;
    for(int i=0; i < a->n; i++)
        if(a->txt[i] != b->txt[i])
            return 0;
    return 1;
}