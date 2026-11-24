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

void strdim_popChar(strdim* wherAdd){
    if(wherAdd->n>1){
        wherAdd->n--;
    }
};

char* strdim_makeP(strdim* from){
    char* retVal = malloc(from->n*sizeof(char));
    strcpy(retVal, from->txt);
    return retVal;
};