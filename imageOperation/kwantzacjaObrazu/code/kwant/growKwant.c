#include<stdio.h>
#include<stdint.h>
#include<math.h>
#include"../../header/kwant.h"

static void kwant1dGrow_split(int counter[256], int sunOfCounter, int* splitOut, int unicNum){
    int readeValue = 0;
    double dataToPartition = (double)sunOfCounter;
    for(int part = 0; part < unicNum-1; part++){ // poterbujemy podzielic unicNum-1
        double partSize = dataToPartition/(unicNum-part);
    //     // printf("%f\n", partSize);

        for(;readeValue < 256 && counter[readeValue] == 0; readeValue++);

        double coutOfValue = (double)counter[ readeValue++ ];
        dataToPartition -= coutOfValue;
        partSize -= coutOfValue;

        while (readeValue < 256 && partSize > 0.0){
            double coutOfValue = (double)counter[ readeValue ];
            if(partSize < coutOfValue/2){
                break;
            }
            readeValue++;
            dataToPartition -= coutOfValue;
            partSize -= coutOfValue;
            for(;readeValue < 256 && counter[readeValue] == 0; readeValue++);
        }
        splitOut[part] = readeValue;
    }
    splitOut[unicNum-1] = 256;
}

static void kwant1dGrow_avg(int counter[256], int* split, int* avgOut, int unicNum){
    int readeValue = 0;
    for(int i = 0; i < unicNum; i++){
        double suma = 0;
        int numInPart = 0;
        for (; readeValue < split[i]; readeValue++){
            int j = counter[readeValue];
            if(j == 0){
                continue;
            }
            // wagowe sumowanie
            // numInPart += j;
            // suma += (double)j * readeValue;
            // nie wagowe sumowanie
            numInPart++;
            suma += (double)readeValue;
            // printf("%d\n", j);
        }
        // printf("%d %f\n", numInPart, suma);
        avgOut[i] = (int)round(suma/numInPart);
    }
}

void kwant1dGrow(uint8_t* data, int numData, uint8_t* outPalet, int* paletSize){
    // zliczanie
    int counter[256];
    for (int i = 0; i < 256; i++){
        counter[i] = 0;
    }
    for (int i = 0; i < numData; i++){
        counter[data[i]]++;
    }
    
    int numExisting = 0;
    for (int i = 0; i < 256; i++){
        // printf("%d: %d\n", i, counter[i]);
        numExisting += (counter[i] > 0) ? 1 : 0;
    }
    *paletSize = (numExisting < *paletSize) ? numExisting : *paletSize;
    int unicNum = *paletSize;

    // partycjonowanie
    int split[unicNum];
    kwant1dGrow_split(counter, numData, split, unicNum);
    
    for(int i=0; i<unicNum; i++){
        printf("%d ", split[i]);
    }
    printf("\n");

    // wyliczanie srednich
    int avgryg[unicNum];
    kwant1dGrow_avg(counter, split, avgryg, unicNum);
    // avgryg[0] = 0; avgryg[unicNum-1] = 255;
    // for(int i=0; i<unicNum; i++){
    //     printf("%d ", avgryg[i]);
    // }
    // printf("\n");

  
}

static void kwnat1dMerge(uint8_t* data, int numData, int unicNum){
    // 256 bo 1 byte
    int counter[256];
    for (int i = 0; i < 256; i++){
        counter[i] = 0;
    }

    for (int i = 0; i < numData; i++){
        counter[data[i]]++;
    }
    
    int ranges[257]; // przedzialy zbiorwor
    int numRanges = 0;
    int sum[256]; 
    for(int i = 0; i < 256; i++){
        if(counter[i] == 0){
            continue;
        }
        sum[numRanges] = counter[i];
        ranges[numRanges] = i;
        numRanges++;
    }
    int sumAdding[256]; // jest to suma dwuch sum
    for(int i = 0; i < numRanges-1; i++){
        sumAdding[i] = sum[i] + sum[i+1];
    }
    sumAdding[numRanges-1] = 0;

    // for(int i = 0; i < numRanges; i++){
    //     printf("%d %d %d\n", ranges[i], sum[i], sumAdding[i]);
    // }

    while (numRanges > unicNum){
        // for(int i = 0; i < numRanges; i++){
        //     printf("%d %d %d\n", ranges[i], sum[i], sumAdding[i]);
        // }
        // printf("\n");

        int intexOfsmalesSum = 0;
        for (int i = 1; i < numRanges-1; i++){
            if(sumAdding[intexOfsmalesSum] > sumAdding[i]){
                intexOfsmalesSum = i;
            }
        }
        if(intexOfsmalesSum > 0){
            sumAdding[intexOfsmalesSum-1] += sum[intexOfsmalesSum+1];
        }
        if(intexOfsmalesSum+1 < numRanges-1){
            sumAdding[intexOfsmalesSum+1] += sum[intexOfsmalesSum];
        }
        sum[intexOfsmalesSum] = sumAdding[intexOfsmalesSum];
        for (int  i = intexOfsmalesSum+1; i < numRanges-1; i++){
            ranges[i] = ranges[i+1];
            sum[i] = sum[i+1];
            sumAdding[i-1] = sumAdding[i];
        }
        numRanges--;
    }

    // for(int i = 0; i < numRanges; i++){
    //     printf("%d %d %d\n", ranges[i], sum[i], sumAdding[i]);
    // }
    // printf("\n");
    
}


