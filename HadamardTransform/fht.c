#include<stdint.h>
#include<stdio.h>

// https://en.wikipedia.org/wiki/Fast_Walsh%E2%80%93Hadamard_transform

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

static void fhtItr(int* list, int level){
    int size = 1<<level;
    for(int half = 1; half < size; half<<=1){
        for(int i = 0; i < size; i+=half<<1){
            for(int j=i; j < i + half; j++){
                int x = list[j];
                int y = list[j+half];
                list[j]      = x + y;
                list[j+half] = x - y;
            }
        }
    }
}

// Fast Walsh–Hadamard transform
void fht(int* list, int level){
    // fhtRec(list, level);
    fhtItr(list, level);
    int size = 1<<level;
    int div = size>>1;
    for(int i = 0; i < size; i++){
        list[i] /= div;
    }
}