#ifndef TREE_H
#define TREE_H
#include"kompiler.h"
#include"couter.h"
#include"bitIo.h"

typedef struct node{
    // rozrurznia czy node jest galenzia czy lisciem
    int isWood;
    int value;
    // dla gałezi jest puste
    BYTE leter;
    // dla liscia sa puste
    struct node* left;
    struct node* right;
} tree;

// koniet dzewa
tree* tree_leaf_init(BYTE leter, int value);

// punk laczacy
tree* tree_wood_init(tree* left, tree* right);

void tree_free(tree* toFree);

void tree_print(tree* toPrint, int deep);

tree* couter_to_tree(couter* from);

// ile punktow jest
int tree_size(tree* whatCount);

void tree_saveNode(tree* toSave, bitO* with);

void tree_saveLeafOrder(tree* toSave, bitO* with);

void tree_save(tree* toSave, bitO* with);

#endif