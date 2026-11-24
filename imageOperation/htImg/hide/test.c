#include<stdio.h>
#include"../code/fht.c"

void print1d(int* data, int size){
    for(int x=0; x<size; x++){
        printf("%5d ", data[x]);
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

void test1d(){
    const int size = 8;
    int data[] = {20,60,60,20,60,61,0,0};
    print1d(data, size);

    fht(data, 3);
    print1d(data, size);

    // const int d = 1<<10;
    // for(int i = 0; i < size; i++){
    //     data[i] += d/2 * ((data[i] > 0) ? 1 : -1);
    //     data[i] /= d;
    // }
    // print1d(data, size);
    // for(int i =0; i < size; i++){ data[i] *= d;}
    // print1d(data, size);

    ifht(data, 3);
    print1d(data, size);
}

void test2d(){
    const int size = 4;
    int data[] = {
        10,10,10,10,
        10,10,10,10,
        10,10,10,10,
        10,10,10,10
    };
    print2d(data, size);

    fht2d(data, 2);
    print2d(data, size);

    ifht2d(data, 2);
    print2d(data, size);
}

int main(){
    test2d();
}