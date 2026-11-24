#ifndef CHARLIST_H
#define CHARLIST_H

// opereowanie beze ograniczenie \0
typedef struct{
    char* txt;
    int n, nm;
} charList;

charList* charList_init();

void charList_free(charList* toFree);

void charList_addChar(charList* wherAdd, char toAdd);

char charList_popChar(charList* wherPop);

char charList_popFrontChar(charList* wherPop);

void charList_print(charList* toPrint);

int charList_comp(charList* a, charList* b);

#endif