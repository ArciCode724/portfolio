#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"window.h"

window* window_init(){
    window* retVal = malloc(sizeof(window));
    // mozna traktowac jako dlugosc lewj i prawej strony
    retVal->center = pow(256,sizeof(uint))/2;
    retVal->left = 0;
    retVal->right = 0;
    retVal->tasma = malloc(pow(256,sizeof(uint))*sizeof(BYTE));

    retVal->quickFinder.ofset = retVal->center;
    retVal->quickFinder.n = pow(256,3);
    retVal->quickFinder.kombinacje = malloc(retVal->quickFinder.n*sizeof(uintList));
    for(int i=0; i<pow(256,3);i++){
        // printf("%d ",i);
        retVal->quickFinder.kombinacje[i].list = malloc(8*sizeof(uint));
        retVal->quickFinder.kombinacje[i].mn=8;
        retVal->quickFinder.kombinacje[i].n=0;
    }
    // printf("\n%p\n", retVal->quickFinder.kombinacje);

    return retVal;
}

int makeFinderIndex(window* base, uint index){
    if(index < base->center-base->left || index+2 >= base->center+base->right)
        return -1;
    int retVal = 0;
    for(int i=0; i<3; i++)
        retVal = retVal<<8 | base->tasma[index+i];
    return retVal;
}

void window_moveBy(window* where, uint by){
    if(by >= 2*where->center)
        return;
    
    // printf("%d", by);
    if(where->quickFinder.ofset < by){
        // printf("!\n");
        for(int i=0; i< where->quickFinder.n; i++)
            for (int j=0; j < where->quickFinder.kombinacje[i].n; j++)
                where->quickFinder.kombinacje[i].list[j] -= where->quickFinder.ofset - where->center;
        where->quickFinder.ofset += where->center;
    }
    where->quickFinder.ofset -= by;
    // printf("%d ", where->quickFinder.ofset); 

    // usuwanie
    for(uint i=where->center-where->left; i<by; i++){
        int index = makeFinderIndex(where, i);
        if(where->quickFinder.kombinacje[index].n == 0) 
            continue;
        for(int j=1; j< where->quickFinder.kombinacje[index].n; j++)
            where->quickFinder.kombinacje[index].list[j-1] = where->quickFinder.kombinacje[index].list[j];
        where->quickFinder.kombinacje[index].n--;
    }

    for(int i=0; i<2*where->center-by; i++){
        where->tasma[i] = where->tasma[i+by];
    }
    where->right = (where->right <= by) ? 0 : where->right-by;
    where->left = (where->center-where->left < by) ? where->center : where->left+by; 

    // dodawanie
    for(int i=1; i<=by; i++){
        int index = makeFinderIndex(where, where->center-i);
        // printf("| %X ", index);
        if(index == -1)
            continue;
        if(where->quickFinder.kombinacje[index].mn == where->quickFinder.kombinacje[index].n){
            where->quickFinder.kombinacje[index].mn+=8;
            where->quickFinder.kombinacje[index].list = realloc(where->quickFinder.kombinacje[index].list, where->quickFinder.kombinacje[index].mn*sizeof(uintList));
        }
        where->quickFinder.kombinacje[index].list[where->quickFinder.kombinacje[index].n] = i-1+where->quickFinder.ofset;
        where->quickFinder.kombinacje[index].n++;
    }

    // printf("\n");
    // for(int i=0; i< where->quickFinder.n; i++){
    //     if(where->quickFinder.kombinacje[i].n == 0)
    //     continue;
    //     printf("%x: ", i);
    //     for (int j=0; j < where->quickFinder.kombinacje[i].n; j++)
    //     printf("%d ", where->quickFinder.kombinacje[i].list[j]+1-where->quickFinder.ofset); // +1-okno->quickFinder.ofset
    //     printf("\n");
    // }
}

void window_find(window* where, uint* retIndex, uint* retLen){
    if(where->right<MIN_ROZ){
        *retIndex = 0;
        *retLen = 0;
        return;
    }
    int pierwszeTrzy = makeFinderIndex(where, where->center);
    if(pierwszeTrzy == -1){
        *retIndex = 0;
        *retLen = 0;
        return;
    }
    // printf("%X ", pierwszeTrzy);

    int limit = 16;
    for(int i=where->quickFinder.kombinacje[pierwszeTrzy].n-1; i>=0; i--){
        limit--;
        int pos = where->quickFinder.kombinacje[pierwszeTrzy].list[i]+1-where->quickFinder.ofset;
        // printf("%d ", where->quickFinder.kombinacje[pierwszeTrzy].n);
        uint len = 0;
        while (len < where->right && where->tasma[where->center+len] == where->tasma[where->center-pos+len]){
            // printf("%d\n", i);
            len++;
        }
        // if(len > 0){
        if(len >= MIN_ROZ){
            *retIndex = pos;
            *retLen = len;
            return;
        }
        
        if(limit == 0){
            break;
        }
    }
    *retIndex = 0;
    *retLen = 0;

}

void window_printTasme(window* where){
    for(uint i = where->center-where->left; i<where->center; i++){
        printf("%c", where->tasma[i]);
    }
    printf("|");
    for(uint i = where->center; i<where->center + where->right; i++){
        printf("%c", where->tasma[i]);
    }
    printf("\n");
}

int window_fillQue(window* where, bitI* from){
    while(where->right < where->center){
        int b = bitI_getMulkiBits(from, 8*sizeof(BYTE));
        if(b == EOF)
            return EOF;
        where->tasma[where->center+where->right] = b;
        where->right++;
    }
    return 0;
}