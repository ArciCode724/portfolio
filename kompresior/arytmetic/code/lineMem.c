// #include<stdint.h>
#include<stdlib.h>
#include"..\header\lineMem.h"

// typedef struct{
//     uint8_t* list;
//     uint32_t n, mn, pos;
// } lineMem;

lineMem* lineMem_init(uint32_t size){
    lineMem* retVal = malloc(sizeof(lineMem));
    retVal->list = malloc(size*sizeof(uint8_t));
    retVal->mn = size;
    retVal->n = 0;
    retVal->pos = 0;
    return retVal;
}

int16_t lineMem_add(lineMem* list, uint8_t addC){
    int16_t retVar;
    if(list->mn == list->n){
        if(list->mn == list->pos){
            list->pos = 0;
        }
        retVar = list->list[list->pos];
    }else{
        retVar = -1;
        list->n++;
    }
    list->list[list->pos] = addC;
    list->pos++;
    return retVar;
}

// 0 nie ustalo sie; 1 udalo sie
int lineMem_getSoonRemoveKontekst(lineMem* list, uint8_t* out, int size){
    if(list->n != list->mn){
        return 0;
    }
    uint32_t move = list->pos;
    for(int i=0; i<size; i++){
        if(move == list->mn){
            move = 0;
        }
        out[i] = list->list[move];
        move++;
    }

    return 1;
}
