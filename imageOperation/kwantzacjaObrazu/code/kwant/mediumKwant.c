#include"../../header/kwant.h"
#include<stdio.h>
#include"../../header/compresColors.h"
#include<stdlib.h>

typedef struct{
    uint8_t rangeRGB[3][2];
    wheighedColor* list;
    uint32_t len; 
} colorblock;

// skraca jesli 
static colorblock makeRange(colorblock block){
    for (int c = 0; c < 3; c++){
        block.rangeRGB[c][0] = 255;
        block.rangeRGB[c][1] = 0;
    }
    for (int i = 0; i < block.len; i++){
        for (int c = 0; c < 3; c++){
            uint8_t v = block.list[i].color.c[c];
            if(block.rangeRGB[c][0] > v){
                block.rangeRGB[c][0] = v;
            }
            if(block.rangeRGB[c][1] < v){
                block.rangeRGB[c][1] = v;
            }
        }
    }
    return block;
}

static void choseBlock(colorblock* grups, int numGrups, uint32_t* outIndex, uint8_t* outChanel){
    uint32_t bestBlock = 0;
    uint8_t withChangel = 0;
    uint8_t biggestRange = 0;

    for (int i = 0; i < numGrups; i++){
        for (int c = 0; c < 3; c++){
            uint8_t newRange = grups[i].rangeRGB[c][1] - grups[i].rangeRGB[c][0];
            if(newRange > biggestRange){
                bestBlock = i;
                withChangel = c;
                biggestRange = newRange;
            }
        }
    }

    *outIndex = bestBlock;
    *outChanel = withChangel;
}

// zwraca poczatkowa wartosc drugier czecsci
static uint8_t findMedian(colorblock block, uint8_t chanel){
    uint32_t counter[256];
    for (uint32_t i = 0; i < 256; i++){
        counter[i] = 0;
    }
    int32_t half = 0;
    for (uint32_t i = 0; i < block.len; i++){
        counter[ block.list[i].color.c[chanel] ] += block.list[i].wheight;
        half += block.list[i].wheight;
    }
    
    uint8_t median = 0;
    half /= 2;
    for(;half > 0; median++){
        if(half < counter[median]/2){
            break;
        }
        half -= counter[median];
    }
    
    return median;
}

static uint32_t splitData(wheighedColor* list, uint32_t listLen, uint8_t chanel, uint8_t sortValue){
    uint32_t len = 0;
    for (int i = 0; i < listLen; i++){
        if(list[i].color.c[chanel] < sortValue){
            wheighedColor temp = list[len];
            list[len] = list[i];
            list[i] = temp;
            len++;
        }
    }
    return len;
}

static color8_3 blockToColor(colorblock block){
    color8_3 retVal;
    for (int i = 0; i < 3; i++){
        retVal.c[i] = (block.rangeRGB[i][1] + block.rangeRGB[i][0])/2;
    }
    
    return retVal;
}

void mediumKwant3D(color8_3* data, int dataNum, color8_3* outPalet, int* pSizePalet){
    uint32_t compresDataSize = 0;
    wheighedColor* compresData = mallocCompresData(data, dataNum, &compresDataSize);
    if( compresDataSize < *pSizePalet){
        *pSizePalet = compresDataSize;
    }
    int paletSize = *pSizePalet;

    colorblock grups[paletSize];
    grups[0].len = compresDataSize;
    grups[0].list = compresData;
    grups[0] = makeRange(grups[0]);

    // for (int c = 0; c < 3; c++){
    //     printf("%d %d\n", grups[0].rangeRGB[c][0], grups[0].rangeRGB[c][1]);
    // }
    // printf("\n");
    // for (int i = 0; i < compresDataSize; i++){
    //     printf("%d %d %d\n", compresData[i].color.c[0], compresData[i].color.c[1], compresData[i].color.c[2]);
    // }
    // printf("\n");
    
    int groupNum = 1;
    for (; groupNum < paletSize; groupNum++){
        uint32_t blockIndex;
        uint8_t chanel;
        choseBlock(grups, groupNum, &blockIndex, &chanel);
        // printf("%d %d\n", blockIndex, chanel);

        uint8_t median = findMedian(grups[blockIndex], chanel);
        // printf("%d\n", median);

        uint32_t partlen = splitData(grups[blockIndex].list, grups[blockIndex].len, chanel, median);

        // printf("%d\n", partlen);
        // for (int i = 0; i < compresDataSize; i++){
        //     printf("%d %d %d\n", compresData[i].color.c[0], compresData[i].color.c[1], compresData[i].color.c[2]);
        // }
        // printf("\n");
        
        grups[groupNum].list = grups[blockIndex].list + partlen;
        grups[groupNum].len = grups[blockIndex].len - partlen;
        grups[blockIndex].len = partlen;

        grups[groupNum] = makeRange(grups[groupNum]);
        grups[blockIndex] = makeRange(grups[blockIndex]);
    }

    for (int i = 0; i < groupNum; i++){
        // printf("%d-%d %d-%d %d-%d\n",
        //     grups[i].rangeRGB[0][0], grups[i].rangeRGB[0][1], grups[i].rangeRGB[1][0], grups[i].rangeRGB[1][1], grups[i].rangeRGB[2][0], grups[i].rangeRGB[2][1]
        // );
        outPalet[i] = blockToColor(grups[i]);
        // printf("%d %d %d\n", outPalet[i].c[0], outPalet[i].c[1], outPalet[i].c[2]);
    }
    
    free(compresData);
}

void mediumKwant1D(uint8_t* data, int dataNum, uint8_t* outPalet, int* pSizePalet){
    uint32_t compresDataSize = 0;
    wheighedColor1D* compresData = mallocCompresData1D(data, dataNum, &compresDataSize);
    if( compresDataSize < *pSizePalet){
        *pSizePalet = compresDataSize;
    }
    int paletSize = *pSizePalet;

    uint32_t sum[compresDataSize];
    for (int i = 0; i < compresDataSize; i++){
        sum[i] = compresData[i].wheight;
    }
    for (int i = 1; i < compresDataSize; i++){
        sum[i] += sum[i-1];
    }
}