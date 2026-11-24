#include<stdlib.h>
#include <string.h>
#include"strdim.h"

strdim* strdim_init(){
    strdim* retVal = malloc(sizeof(strdim));
    retVal->txt = malloc(8*sizeof(char));
    retVal->txt[0] = '\0';
    retVal->n = 1;
    retVal->nm = 8;

    return retVal;
}

void strdim_free(strdim* toFree){
    free(toFree->txt);
    free(toFree);
};

void strdim_addChar(strdim* wherAdd, char toAdd){
    if(wherAdd->n == wherAdd->nm){
        wherAdd->nm += 8;
        wherAdd->txt = realloc(wherAdd->txt, sizeof(char)*wherAdd->nm);
    }
    wherAdd->txt[wherAdd->n] = '\0';
    wherAdd->txt[wherAdd->n-1] = toAdd;
    wherAdd->n++;
};

char strdim_popChar(strdim* wherPop){
    if(wherPop->n > 1){
        wherPop->n--;
        return wherPop->txt[wherPop->n];
    }
};

char strdim_popFrontChar(strdim* wherPop){
    if(wherPop->n <= 1)
        return '\0';
    char c = wherPop->txt[0];
    for(int i=1;i<wherPop->n; i++)
        wherPop->txt[i-1] = wherPop->txt[i];
    wherPop->n--;
    return c;
}

char* strdim_makeP(strdim* from){
    char* retVal = malloc(from->n*sizeof(char));
    strcpy(retVal, from->txt);
    return retVal;
};