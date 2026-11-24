#include<stdio.h>
#include<stdlib.h>
#include"bitIo.h"
#include"tree.h"
#include"translator.h"

void translator_loadValues(translator* save, tree* base, bitI* sorse, strdim* path){
    if(base->isWood == 1){
        strdim_addChar(path, '0');
        translator_loadValues(save, base->left, sorse, path);
        strdim_popChar(path); 

        strdim_addChar(path, '1');
        translator_loadValues(save, base->right, sorse, path);
        strdim_popChar(path);
    }else{
        // printf("%s\n", path->txt);
        BYTE c;
        bitI_getByte(sorse, &c);
        save->leter[save->n] = c;
        save->inBits[save->n] = strdim_makeP(path);
        save->n++;
    }
}

translator* translator_init(bitI* sorse){
    translator* retVal = malloc(sizeof(translator));
    retVal->leter = malloc(256*sizeof(BYTE));
    retVal->inBits = malloc(256*sizeof(char*));
    retVal->n = 0;
    retVal->bitLaded = 0;

    strdim* path = strdim_init();
    tree* keyTree = tree_init(sorse);
    tree_print(keyTree, 0);
    translator_loadValues(retVal, keyTree, sorse, path);
    retVal->low = 0;
    retVal->hie = retVal->n;
    strdim_free(path);
    tree_free(keyTree);

    return retVal;
}

void translator_free(translator* toFree){
    for (int i = 0; i < toFree->n; i++)
        free(toFree->inBits[i]);
    free(toFree->inBits);
    free(toFree->leter);
    free(toFree);
}

int translator_addBit(translator* base, char bit){
    if(bit == '1')
        for(;base->inBits[base->low][base->bitLaded] != '1'; base->low++);
    else if(bit == '0')
        for(;base->inBits[base->hie-1][base->bitLaded] != '0'; base->hie--);
    else
        return -1;
    base->bitLaded++;
    if(base->hie - base->low <= 1)
        return 1;
    return 0;
}

BYTE translator_getLeter(translator* from){
    BYTE retVal = from->leter[from->low];
    from->bitLaded=0;
    from->low=0;
    from->hie=from->n;
    return retVal;
}