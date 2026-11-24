#include<stdlib.h>
#include<stdio.h>
#include"kodeTable.h"

kodeTable* kodeTable_init(int sortedBy){
    // tworzy liste i wepelnia jas znakami <0;256)
    kodeTable* retVal = malloc(sizeof(kodeTable));
    retVal->n = 0;
    // miejsce na nowe klucze;
    retVal->mn = 512;
    retVal->kode = malloc(sizeof(unsigned int)*retVal->mn);
    retVal->text = malloc(sizeof(byteList*)*retVal->mn);
    for(int i=0; i<256;i++){
        retVal->kode[i] = i;
        byteList* list = byteList_init();
        byteList_addByte(list, i);
        retVal->text[i] = list;
        retVal->n++;
    }
    return retVal;
}

void kodeTable_free(kodeTable* toFree){
    free(toFree->kode);
    for (int i = 0; i < toFree->n; i++){
        byteList_free(toFree->text[i]);
    }
    free(toFree->text);
    free(toFree);
}

void kodeTable_print(kodeTable* toPrint){
    int len = 0;
    for(unsigned int n = toPrint->n; n; n/=10)
        len++;
    for (int i = 0; i < toPrint->n; i++){
        printf("%*d: ",len, toPrint->kode[i]);
        byteList_print(toPrint->text[i], 1);
        printf("\n");
    }
}

int kodeTable_findIndex(kodeTable* whereFind, byteList* find){   
    int left = -1;
    int right = whereFind->n;
    int midle = (left+right)/2;
    while (right - left > 1){
        // printf("(%d; %d; %d)\n", left, midle,right);
        int comp = byteList_comp(find, whereFind->text[midle]);
        if(comp==0)
            return midle;
        if(comp > 0)
            left = midle;
        else
            right = midle;
        midle = (left+right)/2;
    }
    return -1;
}

int kodeTable_addText(kodeTable* gdzieAdd, byteList* coAdd){
    // byteList_print(coAdd, 1);
    // printf("\n");
    int retVal = 0;
    if(gdzieAdd->n == gdzieAdd->mn){
        retVal++;
        gdzieAdd->mn *= 2;
        gdzieAdd->kode = realloc(gdzieAdd->kode, sizeof(unsigned int)*gdzieAdd->mn);
        gdzieAdd->text = realloc(gdzieAdd->text, sizeof(byteList*)*gdzieAdd->mn);
    }
    gdzieAdd->kode[gdzieAdd->n] = gdzieAdd->n;
    gdzieAdd->text[gdzieAdd->n] = coAdd;
    gdzieAdd->n++;

    int left = -1;
    int right = gdzieAdd->n-1;
    int midle = (left+right)/2;
    while (right - left > 1){
        // printf("(%d; %d; %d)\n", left, midle,right);
        int comp = byteList_comp(coAdd, gdzieAdd->text[midle]);
        if(comp >= 0)
            left = midle;
        else
            right = midle;
        midle = (left+right)/2;
    }
    midle++;
    for (int i = gdzieAdd->n-1; i > midle; i--){
        int itemp = gdzieAdd->kode[i];
        gdzieAdd->kode[i] = gdzieAdd->kode[i-1];
        gdzieAdd->kode[i-1] = itemp;

        byteList* ptemp = gdzieAdd->text[i];
        gdzieAdd->text[i] = gdzieAdd->text[i-1];
        gdzieAdd->text[i-1] = ptemp;
    }
    
    return retVal;
}