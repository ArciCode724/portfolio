#include<stdlib.h>
#include<stdio.h>
#include"tree.h"
#include"couter.h"

tree* tree_leaf_init(BYTE leter, int value){
    tree* retVal = malloc(sizeof(tree));
    retVal->isWood = 0;
    retVal->leter = leter;
    retVal->value = value;
    retVal->left = NULL;
    retVal->right = NULL;
    return retVal;
}

tree* tree_wood_init(tree* left, tree* right){
    tree* retVal = malloc(sizeof(tree));
    retVal->isWood = 1;
    retVal->leter = 0;
    retVal->value = left->value + right->value;
    retVal->left = left;
    retVal->right = right;
    return retVal;
}

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

tree* couter_to_tree(couter* from){
    int n = from->n;
    tree* treeList[n];

    couter_qsort(from, 0, from->n);
    for(int i=0; i<from->n; i++){
        treeList[i] = tree_leaf_init(from->what[i], from->ile[i]);
    }

    while (1){
        tree* t1 = treeList[n-1];
        tree* t2 = treeList[n-2];
        treeList[n-2] = tree_wood_init(t1, t2);
        n--;
        
        if(n == 1)
            break;
        for(int i=n-1; i>0 && treeList[i]->value>treeList[i-1]->value; i--){
            tree* temp = treeList[i];
            treeList[i] = treeList[i-1];
            treeList[i-1] = temp;
        }
    }
    return treeList[0];
}

int tree_size(tree* whatCount){
    if(whatCount->isWood == 0){
        return 1;
    }
    return 1 + tree_size(whatCount->left) + tree_size(whatCount->right);
}

void tree_saveNode(tree* toSave, bitO* with){
    if(toSave->isWood){
        bitO_putBit(with,'1');
        tree_saveNode(toSave->left, with);
        tree_saveNode(toSave->right, with);
    }else{
        bitO_putBit(with,'0');
    }
}

void tree_saveLeafOrder(tree* toSave, bitO* with){
    if(toSave->isWood){
        tree_saveLeafOrder(toSave->left, with);
        tree_saveLeafOrder(toSave->right, with);
    }else{
        // printf("%2x ", toSave->leter);
        bitO_putByte(with, toSave->leter);
    }
};

void tree_save(tree* toSave, bitO* with){
    int freeBits = 8 - (tree_size(toSave)+1) %8;
    for(int i=0; i<freeBits; i++)
        bitO_putBit(with,'0');
    bitO_putBit(with,'1');
    tree_saveNode(toSave, with);
    tree_saveLeafOrder(toSave, with);
};