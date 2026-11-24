#include<stdlib.h>
#include<stdio.h>
#include"..\header\FFT.h"

//          6.28318530717958647692528676655900576839433879875021164194988918461563281257
#define PI2 6.2831853

typedef struct{
    int start, step;
    int num; // ile elementow nie koniec
} arreyPart;

static void fft_reckur(complex* data, arreyPart whatFrag, complex* out, int plusMinus){
    if(whatFrag.num == 1){
        out[0] = data[whatFrag.start];
        return;
    }

    // wybieramy rozmiar podzialy
    int chosenPrime = whatFrag.num;
    static const int prime[5] = {2,3,5,7,11};
    for(int i=0; i<5; i++){
        if( (chosenPrime % prime[i]) == 0){
            chosenPrime = prime[i];
            break;
        }
    }
    // chosenPrime = whatFrag.num;

    // zerowanie wyjscia
    for(int i = 0; i < whatFrag.num; i++){
        out[i] = 0;
    }
    // static clock_t sum = 0;

    arreyPart subFragment = whatFrag;
    subFragment.num /= chosenPrime;
    subFragment.step *= chosenPrime;
    complex* outFragmanet = malloc(subFragment.num*sizeof(complex));
    for(int iFrag = 0; iFrag < chosenPrime; iFrag++){
        // subFragment.start = iFrag*whatFrag.step + whatFrag.start;
        fft_reckur(data, subFragment, outFragmanet, plusMinus);

        // double of = (double)iFrag/chosenPrime;
        for(int k = 0; k < subFragment.num; k++){
            // double t = (double)iFrag*k/whatFrag.num;
            for(int j = 0; j < chosenPrime; j++){
                // clock_t start = clock();
                complex multiplaer = cexp(I*PI2*( (double)k*iFrag/whatFrag.num + (double)j*iFrag/chosenPrime )*plusMinus);
                out[k + j*subFragment.num] += outFragmanet[k] * multiplaer;
                // sum += clock() - start;
            }
        }
        subFragment.start += whatFrag.step;
    }
    // printf("%f\n", (float)sum/CLOCKS_PER_SEC); // CLOCKS_PER_SEC
    free(outFragmanet);
}

void fft(complex* data, int numData, complex* out){
    // arreyPart startinFrag; startinFrag.start = 0; startinFrag.step = 1; startinFrag.num = numData;
    arreyPart startinFrag = { 0, 1, numData};
    fft_reckur(data, startinFrag, out, -1);
    for(int i = 0; i < numData; i++){
        out[i] /= numData;
    }
}

void ifft(complex* data, int numData, complex* out){
    // arreyPart startinFrag; startinFrag.start = 0; startinFrag.step = 1; startinFrag.num = numData;
    arreyPart startinFrag = { 0, 1, numData};
    fft_reckur(data, startinFrag, out, 1);
    // for(int i = 0; i< numData; i++){
    //     out[i].r /= numData; out[i].i /= numData;
    // }
}

void fft2d(complex* data, int numX, int numY, complex* out){
    complex* bufferList = malloc(sizeof(complex)*numX*numY);
    for(int y = 0; y < numY; y++){
        arreyPart startinFrag = { y*numX, 1, numX};
        fft_reckur(data, startinFrag, bufferList+y*numX, -1);
    }

    // print2d(bufferList, numX, numY);
    // printf("\n");

    complex* bufferX = malloc(sizeof(complex)*numY);
    for(int x = 0; x < numX; x++){
        arreyPart startinFrag = { x, numX, numY};
        fft_reckur(bufferList, startinFrag, bufferX, -1);
        for(int i = 0; i < numY; i++){
            out[i*numX + x] = bufferX[i];
        }
    }

    int num = numX*numY;
    for(int i = 0; i<num; i++){
        out[i] /= num;
    }

    free(bufferX);
    free(bufferList);
}

void ifft2d(complex* data, int numX, int numY, complex* out){
    complex* bufferList = malloc(sizeof(complex)*numX*numY);
    for(int y = 0; y < numY; y++){
        arreyPart startinFrag = { y*numX, 1, numX};
        fft_reckur(data, startinFrag, bufferList+y*numX, 1);
    }

    complex* bufferX = malloc(sizeof(complex)*numY);
    for(int x = 0; x < numX; x++){
        arreyPart startinFrag = { x, numX, numY};
        fft_reckur(bufferList, startinFrag, bufferX, 1);
        for(int i = 0; i < numY; i++){
            out[i*numX + x] = bufferX[i];
        }
    }

    // int num = numX*numY;
    // for(int i = 0; i<num; i++){
    //     out[i].r /= num;
    //     out[i].i /= num;
    // }

    free(bufferX);
    free(bufferList);
}
