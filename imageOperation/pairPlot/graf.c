#include"graf.h"
#include<stdlib.h>
#include<stdio.h>

char* makeHist(color8_3* img, uint64_t imgSize, int c, int outSize){
    if(outSize > 256){ outSize = 256;}
    char* retVal = malloc(sizeof(char)*outSize*outSize);
    
    uint64_t count[256];
    for (int i = 0; i < 256; i++){ count[i] = 0;}
    for (int i = 0; i < imgSize; i++){ count[img[i].c[c]]++;}
    // for (int i = 0; i < 256; i++){ printf("%d ", count[i]);}
    // printf("\n");
    
    int rangeOfPixel = (256+outSize-1)/outSize;

    int countOut[outSize];
    for (int i = 0; i < outSize; i++){ countOut[i] = 0;}
    for (int i = 0; i < outSize; i++){
        for (int j = rangeOfPixel*i; j < 256 && j < rangeOfPixel*(i+1); j++){
            countOut[i] += count[j];
        }
    }
    int max = 0;
    for (int i = 0; i < outSize; i++){ 
        if(countOut[i] > max){
            max = countOut[i];
        }
    }
    for (int i = 0; i < outSize; i++){
        countOut[i] = outSize*countOut[i]/max;
    }
    
    for (int y = 0; y < outSize; y++){
        for (int x = 0; x < outSize; x++){
            if(countOut[x] >= y){
                retVal[y*outSize + x] = '#';
            }else{
                retVal[y*outSize + x] = '.';
            }
        }
    }

    return retVal;
}