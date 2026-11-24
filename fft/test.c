#include<stdio.h>
#include"FFT.h"
#include<time.h>
#include<math.h>
#include<stdlib.h>

//          6.28318530717958647692528676655900576839433879875021164194988918461563281257
#define PI2 6.2831853

void print2d(complex* data, int numX, int numY){
    for(int y=0; y<numY; y++){
        for(int x=0; x<numX; x++){
            printf("%0.2f %0.2f ; ", creal(data[y*numX + x]), cimag(data[y*numX + x]));
        }
        printf("\n");
    }
}

void testFFT(){
    int numData = 32; // 1000000
    // complex data[numData];
    // complex out[numData];
    complex* data = malloc(numData*sizeof(complex));
    complex* out = malloc(numData*sizeof(complex));
    for(int i=0; i<numData; i++){
        double t = (double)i/numData; 
        // data[i].r = 5 + cos(PI2*1*t) + sin(PI2*2*t) - cos(PI2*3*t) + 500*cos(PI2*400*t); 
        data[i] = 5 - sin(PI2*1*t) + cos(PI2*2*t); 
        // data[i].i = 0;
        printf("%f %f\n", creal(data[i]), cimag(data[i]));
    }
    printf("\n");

    // clock_t start = clock();

    fft(data, numData, out);

    // printf("%f\n", (float)(clock() - start)/CLOCKS_PER_SEC); // CLOCKS_PER_SEC

    // return;

    for(int i = 0; i< numData; i++){
        printf("%f %f\n", creal(out[i]), cimag(out[i]));
    }
    printf("\n");

    ifft(out, numData, data);

    for(int i = 0; i< numData; i++){
        printf("%f %f\n", creal(data[i]), cimag(data[i]));
    }
    printf("\n");

    // printCosSinLine(out, numData);

}

void testFFT2d(){
    int numX = 9;
    int numY = 3;
    int num = numX*numY;
    complex* list = malloc(sizeof(complex)*numX*numY);
    for(int y=0; y<numY; y++){
        for(int x=0; x<numX; x++){
            list[y*numX + x] =  cos(PI2*x/numX)*cos(PI2*y/numY) + 20*cos(PI2*2*x/numX)*sin(PI2*y/numY) + 13*sin(PI2*3*x/numX)*cos(PI2*y/numY) + 4*sin(PI2*4*x/numX)*sin(PI2*y/numY);
        }
    }

    
    print2d(list, numX, numY);
    printf("\n");
    
    complex* out = malloc(sizeof(complex)*numX*numY);
    fft2d(list, numX, numY, out);
    print2d(out, numX, numY);
    printf("\n");
    
    // for(int i = 0; i<num; i++){
    //     out[i] = roundf(creal(out[i])) + I*roundf(cimag(out[i]));
    // }
    // print2d(out, numX, numY);
    // printf("\n");

    ifft2d(out, numX, numY, list);
    print2d(list, numX, numY);
    printf("\n");
}

int main(int argc, char const *argv[]){
    testFFT();
    // testFFT2d();
}
