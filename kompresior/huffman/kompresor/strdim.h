#ifndef STRDIM_H
#define STRDIM_H

typedef struct{
    char* txt;
    int n, nm;
} strdim;

strdim* strdim_init();

void strdim_free(strdim* toFree);

void strdim_addChar(strdim* wherAdd, char toAdd);

void strdim_popChar(strdim* wherAdd);

char* strdim_makeP(strdim* from);

#endif