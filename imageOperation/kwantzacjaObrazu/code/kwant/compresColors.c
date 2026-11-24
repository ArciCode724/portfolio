#include"../../header/compresColors.h"
#include<stdlib.h>

#define COLORCOMB 256*256*256

wheighedColor* mallocCompresData(color8_3* data, int dataNum, uint32_t* newSize){
    uint32_t* counter = malloc(sizeof(uint32_t)*COLORCOMB);
    // printf("%p", counter);
    for(uint32_t i = 0; i < dataNum; i++){
        uint32_t index = data[i].c[0];
        index<<=8;
        index |= data[i].c[1];
        index<<=8;
        index |= data[i].c[2];
        counter[index]++;
    }
    int noZero = 0;
    for(uint32_t i = 0; i < COLORCOMB; i++){
        if(counter[i]){
            noZero++;
        }
    }
    // printf("%d", noZero);
    wheighedColor* compresColors = malloc(sizeof(wheighedColor)*noZero);
    noZero = 0;
    for(uint32_t i = 0; i < COLORCOMB; i++){
        if(counter[i] == 0){
            continue;
        }
        color8_3 color;
        color.c[0] = (uint8_t)(i>>16);
        color.c[1] = (uint8_t)(i>>8);
        color.c[2] = (uint8_t)i;
        compresColors[noZero].color = color;
        compresColors[noZero].wheight = counter[i];

        noZero++;
    }
    free(counter);
    *newSize = noZero;
    return compresColors;
}

wheighedColor1D* mallocCompresData1D(uint8_t* data, int dataNum, uint32_t* newSize){
    wheighedColor1D* retVal = malloc(sizeof(wheighedColor1D)*256);
    *newSize = 0;
    for (int i = 0; i < 256; i++){
        retVal[i].color = i;
        retVal[i].wheight = 0;
    }
    for (int i = 0; i < dataNum; i++){
        retVal[data[i]].wheight++;  
    }
    for (int i = 0; i < 256; i++){
        if(retVal[i].wheight == 0){ continue;}
        retVal[(*newSize)++] = retVal[i];
    }
    return retVal;
}