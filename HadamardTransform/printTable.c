#include<stdio.h>

void print1df(double* data, int size){
    for(int x=0; x<size; x++){
        printf("%7.1f ", data[x]);
    }
    printf("\n");
}

void print1d(int* data, int size){
    for(int x=0; x<size; x++){
        printf("%3d ", data[x]);
    }
    printf("\n");
}

void print2d(int* data, int size){
    for(int y=0; y<size; y++){
        for(int x=0; x<size; x++){
            printf("%3d ", data[size*y+x]);
        }
        printf("\n");
    }    
}