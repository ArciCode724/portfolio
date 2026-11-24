#include"../../header/paletApply.h"
// #include<stdlib.h>
// #include<stdint.h>
#include<stdio.h>
#include<math.h>

void orderApply1D(uint8_t* data, int dataWitdh, int dataHeight, uint8_t* palet, int paletNum){
    for (int i = 1; i < paletNum; i++){
        for (int j = i; j > 0; j--){
            if(palet[j-1] <= palet[j]){break;}
            palet[j] ^= palet[j-1];
            palet[j-1] ^= palet[j];
            palet[j] ^= palet[j-1];
        }
    }
    
    // int matixSize = 2;
    // uint8_t orderMatrix[2][2] = {
    //     {0,2},
    //     {3,1}
    // };
    int matixSize = 4;
    uint8_t orderMatrix[4][4] = {
        { 0, 8, 2,10},
        {12, 4,14, 6},
        { 3,11, 1, 9},
        {15, 7,13, 5}
    };
    for (int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            int pixelIdx = y*dataWitdh+x;
            if(data[pixelIdx] < palet[0]){
                data[pixelIdx] = palet[0];
                continue;
            }
            if(palet[paletNum-1] <= data[pixelIdx]){
                data[pixelIdx] = palet[paletNum-1];
                continue;
            }

            int i = 0;
            for(; !(palet[i] <= data[pixelIdx] && data[pixelIdx] < palet[i+1]); i++);

            int range = palet[i+1] - palet[i];
            int bias = orderMatrix[y%matixSize][x%matixSize];
            int test = palet[i] + range*(2*bias + 1)/(matixSize*matixSize)/2;
            data[pixelIdx] = (data[pixelIdx] <= test) ? palet[i] : palet[i+1];
        }
    }
}

// ---

typedef struct _colorMix{
    color8_3 a,b;
    double mix;
} colorMix;

#define CASHSIZE 32
typedef struct _colorMixCash{
    struct {
        color8_3 key;
        colorMix val;
        uint8_t saveBit;
    } mem[CASHSIZE];
    int curentPoint;
} colorMixCash;

static void cashInit(colorMixCash* cash, color8_3 key, colorMix val){
    cash->curentPoint = 0;
    for (int i = 0; i < CASHSIZE; i++){
        cash->mem[i].key = key;
        cash->mem[i].val = val;
        cash->mem[i].saveBit = 1;
    }
}

static void cashAdd(colorMixCash* cash, color8_3 key, colorMix val){
    while (cash->mem[ cash->curentPoint ].saveBit){
        cash->mem[ cash->curentPoint ].saveBit = 0;
        cash->curentPoint = (cash->curentPoint + 1)%CASHSIZE;
    }
    cash->mem[cash->curentPoint].key = key;
    cash->mem[cash->curentPoint].val = val;
    cash->mem[cash->curentPoint].saveBit = 1;
    cash->curentPoint = (cash->curentPoint + 1)%CASHSIZE;
}

static int cashFind(colorMixCash* cash, color8_3 key){
    for (int i = 0; i < CASHSIZE; i++){
        if( colorEqual(cash->mem[i].key, key) ){
            cash->mem[i].saveBit = 1;
            return i;
        }
    }
    return -1;
}

static double mixColor(color8_3 a, color8_3 b, color8_3 target){
// (t-1)*a + t*b
// S[i=0,n]((v_1_i*(1-t)+v_2_i*t - v_3_i)^2)
// dt
// S[i=0,n](2*(v_1_i*(1-t)+v_2_i*t - v_3_i)*(v_2_i-v_1_i)) = 0
// S[i=0,n](v_1_i*(v_2_i-v_1_i) + t*(v_2_i-v_1_i)^2 - v_3_i*(v_2_i-v_1_i)) = 0
// t = S[i=0,n]((v_2_i - v_1_i)(v_3_i - v_1_i))/S[i=0,n]((v_2_i-v_1_i)^2)
    double top = 0;
    double down = 0;
    for (int i = 0; i < 3; i++){
        top += (b.c[i] - a.c[i])*(target.c[i] - a.c[i]);
        down += (b.c[i] - a.c[i])*(b.c[i] - a.c[i]);
    }
    if(down == 0){return 0;}
    return top/down;
}

static double costOfMix(colorMix toTest, color8_3 target){
    double retVal = 0;
    retVal += 0.1*colorDis(toTest.a, toTest.b);
    color8_3 mixEffect;
    for (int i = 0; i < 3; i++){
        mixEffect.c[i] = (1-toTest.mix)*toTest.a.c[i] + toTest.mix * toTest.b.c[i];
    }
    retVal += colorDis(mixEffect, target);
    return retVal;
}

static colorMix findBestColorMix(color8_3 target, color8_3* palet, int paletNum){
    double cost = INFINITY;
    colorMix retVal;
    for (int i = 0; i < paletNum; i++){
        for (int j = i; j < paletNum; j++){
            double mix = mixColor(palet[i], palet[j], target);
            if(mix < 0 || mix > 1){ continue;}
            colorMix testMix = {.a = palet[i], .b = palet[j], .mix = mix};
            double newCost = costOfMix(testMix, target);
            if(cost > newCost){
                retVal = testMix;
                cost = newCost;
            }
        }
    }
    return retVal;
}

void orderApply3D(color8_3* data, int dataWitdh, int dataHeight, color8_3* palet, int paletNum){
    // int matixSize = 2;
    // uint8_t orderMatrix[2][2] = {
    //     {0,2},
    //     {3,1}
    // };
    int matixSize = 4;
    uint8_t orderMatrix[4][4] = {
        { 0, 8, 2,10},
        {12, 4,14, 6},
        { 3,11, 1, 9},
        {15, 7,13, 5}
    };
    // int matixSize = 8;
    // uint8_t orderMatrix[8][8] = {
    //     { 0,32, 8,40, 2,34,10,42},
    //     {48,16,56,24,50,18,58,26},
    //     {12,44, 4,36,14,46, 6,38},
    //     {60,28,52,20,62,30,54,22},
    //     { 3,35,11,43, 1,33, 9,41},
    //     {51,19,59,27,49,17,57,25},
    //     {15,47, 7,39,13,45, 5,37},
    //     {63,31,55,23,61,29,53,21}
    // };

    colorMixCash cash;
    cashInit(&cash, data[0], findBestColorMix(data[0], palet, paletNum));

    int new = 0;
    int old = 0;

    for (int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            int pixelIdx = y*dataWitdh+x;
            
            colorMix mixForColor;
            int cashIndex = cashFind(&cash, data[pixelIdx]);
            if(cashIndex == -1){
                mixForColor = findBestColorMix(data[pixelIdx], palet, paletNum);
                cashAdd(&cash, data[pixelIdx], mixForColor);
                new++;
                // printf("a");
            }else{
                mixForColor = cash.mem[cashIndex].val;
                old++;
                // printf("b");
            }

            double check = orderMatrix[y%matixSize][x%matixSize];
            check = (check+1)/(matixSize*matixSize+1);
            if(mixForColor.mix < check){
                data[pixelIdx] = mixForColor.a;
            }else{
                data[pixelIdx] = mixForColor.b;
            }
        }
    }
    // printf("new: %d; old: %d", new, old);
}

/* 
old
    for (int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            int pixelIdx = y*dataWitdh+x;
            colorMix mixForColor = findBestColorMix(data[pixelIdx], palet, paletNum);

            double check = orderMatrix[y%matixSize][x%matixSize];
            check = (check+1)/(matixSize*matixSize+1);
            if(mixForColor.mix < check){
                data[pixelIdx] = mixForColor.a;
                // printf("a");
            }else{
                data[pixelIdx] = mixForColor.b;
                // printf("a");
            }
        }
    }
*/