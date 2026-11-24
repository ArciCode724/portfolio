#ifndef TREE_H
#define TREE_H
#include"bitIo.h"

typedef struct node{
    // rozrurznia czy node jest galenzia czy lisciem
    int isWood;
    struct node* left;
    struct node* right;
} tree;

tree* tree_load(bitI* sorse);

tree* tree_init(bitI* sorse);

void tree_free(tree* toFree);

void tree_print(tree* toPrint, int deep);

#endif