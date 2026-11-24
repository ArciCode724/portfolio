#include<stdint.h>
#include<stdio.h>
#include"../head/fht.h"

// https://en.wikipedia.org/wiki/Fast_Walsh%E2%80%93Hadamard_transform

#define POINTBITS 1<<7

static void fhtRec(int* list, int level){
    if(level == 0){ return; }
    int size = 1<<level;
    // printf("l: %d\n", level);
    // for(int x=0; x<size; x++){
        // printf("%3d ", list[x]);
    // }
    // printf("\n");
    fhtRec(list, level-1);
    fhtRec(list+(level>>1)+1, level-1);
    int half = size >> 1;
    for(int i = 0; i < half; i++){
        // printf("%d %d", i, i+half);
        int x = list[i];
        int y = list[i+half];
        list[i]      = x + y;
        list[i+half] = x - y;
    }
    // printf("\n");
}

void walshOrder(int* out, int level){
    int listSize = 1<<level;
    for(int k = 0; k < listSize; k++){
        int lastState = 0;
        int f = 0;
        // out[k] = 0;
        for(int i = 1; i < listSize; i++){
            // int state = i & k;
            // for(int half = sizeof(int)/2*8; half > 0; half /= 2) state ^= state>>half;
            int state = 0;
            for(int x = i&k; x; x>>=1){ state ^= x;}
            state &= 1;

            if(state != lastState) f++;
            lastState = state;
        }
        // out[f] = k;
        out[k] = f;
    }
}

static void fhtItr(int* list, int level, int* pos){
    int size = 1<<level;    
    for(int half = 1; half < size; half<<=1){
        for(int i = 0; i < size; i+=half<<1){
            for(int j=i; j < i + half; j++){
                int ix = pos[j];
                int iy = pos[j + half];

                int x = list[ix];
                int y = list[iy];
                list[ix] = (x + y)/2;
                list[iy] = (x - y)/2;
            }
        }
    }
}

static void ifhtItr(int* list, int level, int* pos){
    int size = 1<<level;
    for(int half = 1; half < size; half<<=1){
        for(int i = 0; i < size; i+=half<<1){
            for(int j=i; j < i + half; j++){
                int ix = pos[j];
                int iy = pos[j + half];

                int x = list[ix];
                int y = list[iy];
                list[ix] = x + y;
                list[iy] = x - y;
            }
        }
    }
}

// Fast Walsh–Hadamard transform
void fht(int* list, int level){
    int size = 1<<level;
    for(int i = 0; i < size; i++){
        list[i] *= POINTBITS;
    }
    int pos[size];
    for(int i = 0; i < size; i++){ pos[i] = i;}
    fhtItr(list, level, pos);
    // for(int i = 0; i < size; i++){
    //     list[i] += POINTBITS/2;
    //     list[i] /= POINTBITS;
    // }
}

void ifht(int* list, int level){
    int size = 1<<level;
    int pos[size];
    for(int i = 0; i < size; i++){ pos[i] = i;}
    ifhtItr(list, level, pos);
    for(int i = 0; i < size; i++){
        // list[i] += POINTBITS/2;
        list[i] /= POINTBITS;
    }
}

static void fht2dBody(int* list, int level, void (*metod)(int*, int, int*)){
    int size = 1<<level; // rozmiar boku
    int pos[size];

    // x
    for(int i = 0; i < size; i++){ pos[i] = i;}
    for(int y = 0; y < size; y++){
        // fhtItr(list, level, pos);
        metod(list, level, pos);
        for(int i = 0; i < size; i++){ pos[i] += size;}
    }

    // y
    for(int i = 0; i < size; i++){ pos[i] = i*size;}
    for(int x = 0; x < size; x++){
        // fhtItr(list, level, pos);
        metod(list, level, pos);
        for(int i = 0; i < size; i++){ pos[i] += 1;}
    }
}

void fht2d(int* list, int level){
    int size = 1<<level; // rozmiar boku
    for(int i = 0; i < size*size; i++){
        list[i] *= POINTBITS;
    }
    fht2dBody(list, level, fhtItr);
    // for(int i = 0; i < size*size; i++){
    //     list[i] += POINTBITS/2;
    //     list[i] /= POINTBITS;
    // }
}

void ifht2d(int* list, int level){
    fht2dBody(list, level, ifhtItr);
    int size = 1<<level; // rozmiar boku
    for(int i = 0; i < size*size; i++){
        list[i] /= POINTBITS;
    }
}