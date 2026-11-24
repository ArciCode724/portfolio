#include<stdio.h>
#include"fht.c"
#include"printTable.c"
#include"walsh.c"
#include<math.h>

int ht_v2(int i, int k){
    int a = i & k;
    for(int half = sizeof(int)/2*8; half > 0; half /= 2) a ^= a>>half;
    return -2*(a&1) + 1;
}

int ht(int i, int k){
    int retVal = 0;
    for(int x = i&k; x; x>>=1){ retVal ^= x&1;}
    return -2*retVal + 1;
}

void htt2d(int* data, int size){
    const int mSize = size*size;
    int retVal[mSize];
    for(int y=0; y < size; y++){
        for(int i=0; i<size; i++){
            retVal[size*y + i] = 0;
            for(int k = 0; k < size; k++){
                retVal[size*y + i] += data[size*y + k]*ht(i,k);
            }
        }
        // for(int i = 0; i < size; i++){
        //     retVal[size*y + i] /= size;
        // }
    }

    for(int x=0; x < size; x++){
        for(int i=0; i<size; i++){
            data[size*i + x] = 0;
            for(int k = 0; k < size; k++){
                data[size*i + x] += retVal[size*k + x]*ht(i,k);
            }
        }
        for(int i = 0; i < size; i++){
            data[size*i + x] /= size;
        }
    }
}

void htt(int* data, int size){
    int retVal[size];
    for(int i=0; i<size; i++){
        retVal[i] = 0;
        for(int k = 0; k < size; k++){
            retVal[i] += data[k]*ht(i,k);
        }
    }
    for(int i = 0; i < size; i++){
        data[i] = 2*retVal[i]/size;
    }
}

void ihtt(int* data, int size){
    int retVal[size];
    for(int i=0; i<size; i++){
        retVal[i] = 0;
        for(int k = 0; k < size; k++){
            retVal[i] += data[k]*ht(i,k);
        }
    }
    for(int i = 0; i < size; i++){
        data[i] = retVal[i]/2;
    }
}

void httf(double* data, int size){
    double retVal[size];
    for(int i=0; i<size; i++){
        retVal[i] = 0;
        for(int k = 0; k < size; k++){
            retVal[i] += data[k]*ht(i,k);
        }
    }
    double div = pow(2.0, log2(size)/2.0);
    for(int i = 0; i < size; i++){
        data[i] = retVal[i]/div;
    }
}

void test1d(){
    const int size = 8;
    const int max = 50;
    int data[] = {20,40,20,10,50,20,10,0};
    print1d(data, size);

    htt(data, size);
    print1d(data, size);

    const int d = 20;
    for(int i =0; i < size; i++){
        data[i] += d/2 * ((data[i] > 0) ? 1 : -1);
        data[i] /= d;
    }
    print1d(data, size);
    for(int i =0; i < size; i++){ 
        data[i] *= d;
        // data[i] += d/2 * ((data[i] > 0) ? 1 : -1);

    }
    print1d(data, size);

    ihtt(data, size);
    print1d(data, size);
}

void test1df(){
    const int size = 8;
    double data[] = {20,40,20,10,50,20,10,0};
    print1df(data, size);

    httf(data, size);
    print1df(data, size);

    const double d = 8;
    for(int i =0; i < size; i++){ data[i] = round(data[i]/d);}
    print1df(data, size);
    for(int i =0; i < size; i++){ data[i] *= d;}
    print1df(data, size);

    httf(data, size);
    print1df(data, size);
}

void test2d(){
        int data[4*4] = {
        0,0,64,0,
        0,64,64,0,
        0,0,64,0,
        0,64,64,64
    };
    print2d(data , 4);
    printf("\n");

    htt2d(data, 4);
    for(int i=0; i<4*4; i++){
        const int a = 16;
        if(data[i] <= a && data[i] >= -a){
            data[i] = 0;
        }
    }
    print2d(data , 4);
    printf("\n");


    htt2d(data, 4);
    print2d(data , 4);
}

void testf(){
    int data[4] = {10,10,10,12};
    print1d(data, 4);
    fht(data, 2);
    print1d(data, 4);
    fht(data, 2);
    print1d(data, 4);
}

int main(){


    return 0;
}