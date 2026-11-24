#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"translator.h"
#include"tree.h"
#include"kompiler.h"
#include"strdim.h"

void tree_checkLeafs(tree* from, translator* save, strdim* path){
    if(from->isWood){
        strdim_addChar(path, '0');
        tree_checkLeafs(from->left, save, path);
        strdim_popChar(path);
        strdim_addChar(path, '1');
        tree_checkLeafs(from->right, save, path);
        strdim_popChar(path);
    }else{
        save->inBits[from->leter] = strdim_makeP(path);
    }
};

translator* tree_to_translator(tree* from){
    translator* retVal = malloc(sizeof(translator));
    retVal->n = (int)pow(2, 8*sizeof(BYTE));
    retVal->inBits = calloc(retVal->n,sizeof(char*));

    strdim* path = strdim_init();
    tree_checkLeafs(from, retVal, path);
    strdim_free(path);

    return retVal;
};

void translator_free(translator* toFree){
    for (int i = 0; i < toFree->n; i++)
        free(toFree->inBits[i]);
    free(toFree->inBits);
    free(toFree);
};

void translator_print(translator* toPrint){
    for (int i = 0; i < toPrint->n; i++){
        if(!toPrint->inBits[i])
            continue;
        printf("%2x: %s\n", i, toPrint->inBits[i]);
    }
    
}

double couter_newLen(couter* cou, translator* tra){
    double suma = 0;
    for(int i=0; i<cou->n; i++)
        suma += cou->ile[i] * strlen(tra->inBits[cou->what[i]]);
    return suma;
}