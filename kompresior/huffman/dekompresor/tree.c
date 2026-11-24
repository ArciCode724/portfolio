#include<stdlib.h>
#include<stdio.h>
#include"tree.h"

tree* tree_load(bitI* sorse){
    tree* retVal = malloc(sizeof(tree));
    if(bitI_getBit(sorse) == '1'){
        retVal->isWood = 1;
        retVal->left = tree_load(sorse);
        retVal->right = tree_load(sorse);
    }else{
        retVal->isWood = 0;
        retVal->left = NULL;
        retVal->left = NULL;
    }
    return retVal;
};

tree* tree_init(bitI* sorse){
    while(bitI_getBit(sorse) != '1');
    tree* retVal = tree_load(sorse);
    return retVal;
};

void tree_free(tree* toFree){
    if(toFree->isWood){
        tree_free(toFree->left);
        tree_free(toFree->right);
    }
    free(toFree);
}

void tree_print(tree* toPrint, int deep){
    if(toPrint->isWood){
        tree_print(toPrint->left, deep+1);
        printf("%*c\n", deep+1, '#');
        tree_print(toPrint->right, deep+1);
    }else{
        printf("%*d\n", deep+1, deep);
    }
}