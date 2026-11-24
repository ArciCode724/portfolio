#include<stdlib.h>
#include<stdio.h>
#include"../../header/kwant.h"
#include"../../header/compresColors.h"
// #include<math.h>

typedef struct _partition3D{
    wheighedColor** partData;
    int* partNum;
    int dLen;
    int partLen;
} partition3D;

static int hashVector(int* vec, int d, int dLen){
    int retVal = 0;
    for(int i=d-1; i>=0; i--){
        retVal *= dLen;
        retVal += vec[i];
    }
    return retVal;
}

static void partitionOfColor8_3(color8_3 color, int partLen, int* outlist){
    for (int i = 0; i < 3; i++){
        outlist[i] = color.c[i]/partLen;
    }
}

static int hashColor8_3(color8_3 color, int dLen, int partLen){
    int list[3];
    partitionOfColor8_3(color, partLen, list);
    return hashVector(list, 3, dLen);
}

static partition3D mallocPartition3D(wheighedColor* data, int numData, int partLen){
    partition3D retVal;
    retVal.partLen = partLen;

    int dLen = 255/partLen + 1;
    retVal.dLen = dLen;
    int listLen = dLen*dLen*dLen;

    retVal.partNum = malloc(sizeof(int)*listLen);
    for (int i = 0; i < listLen; i++){
        retVal.partNum[i] = 0;
    }
    // printf("%d %d\n",dLen, listLen);
    
    retVal.partData = malloc(sizeof(wheighedColor*)*listLen);
    int realocBy = 8;
    for (int i = 0; i < numData; i++){
        int hash = hashColor8_3(data[i].color, dLen, partLen);
        // printf("%d ", hash);
        if(retVal.partNum[hash] == 0){
            retVal.partData[hash] = malloc(sizeof(wheighedColor)*realocBy);
            // printf("%d", retVal.partData[i] != NULL);
        }else if(retVal.partNum[hash]%realocBy == 0){
            retVal.partData[hash] = realloc(retVal.partData[hash], sizeof(wheighedColor)*(retVal.partNum[hash] + realocBy) );
            // printf("%p ", retVal.partData[hash]);
        }
        // printf("%d ", retVal.partNum[hash]);
        // printf("a");
        retVal.partData[hash][retVal.partNum[hash]++] = data[i];
        // printf("b\n");
    }
    
    return retVal;
}

static void freePartition3D(partition3D toFree){
    int listLen = toFree.dLen*toFree.dLen*toFree.dLen;
    for (int i = 0; i < listLen; i++){
        if(toFree.partNum[i] == 0){
            continue;
        }
        free(toFree.partData[i]);
    }
    free(toFree.partData);
    free(toFree.partNum);
}

static color8_3 avgColorWithPartition(partition3D part3D, color8_3 center, int range){
    int rangePow = range * range;

    int partitionOfcener[3];
    partitionOfColor8_3(center, range, partitionOfcener);

    uint64_t sumRGB[3] = {0,0,0};
    uint64_t num = 0;
    for (int i = 0; i < 3*3*3; i++){
        int checkPartition[3];
        int isOutside = 0;
        int partitionI = i;
        for (int j = 0; j < 3; j++){
            checkPartition[j] = partitionOfcener[j] + partitionI%3 - 1;
            partitionI /= 3;
            if(checkPartition[j] < 0 || checkPartition[j] >= part3D.dLen){
                isOutside = 1;
            }
        }
        if(isOutside){
            continue;
        }
        
        int hash = hashVector(checkPartition, 3, part3D.dLen);

        for (int j = 0; j < part3D.partNum[hash]; j++){
            if( colorDis(center, part3D.partData[hash][j].color) > rangePow ){
                continue;
            }
            
            for (int z = 0; z < 3; z++){
                sumRGB[z] += part3D.partData[hash][j].color.c[z] * part3D.partData[hash][j].wheight;
            }
            // printf("%d %d %d\n", data[i].color.c[0], data[i].color.c[1], data[i].color.c[2]);
            num += part3D.partData[hash][j].wheight;
        }
    }

    color8_3 retVal;
    for (int i = 0; i < 3; i++){
        retVal.c[i] = sumRGB[i]/num;
    }
    // // printf("%d %d %d\n\n", retVal.c[0], retVal.c[1], retVal.c[2]);
    
    return retVal;
}

static color8_3 avgColor(wheighedColor* data, int dataNum, color8_3 center, int range){
    range *= range;

    uint64_t sumRGB[3] = {0,0,0};
    uint64_t num = 0;
    for (int i = 0; i < dataNum; i++){
        if( colorDis(center, data[i].color) > range ){
            continue;
        }
        // printf("%d ", colorDis(center, data[i].color));
        for (int j = 0; j < 3; j++){
            sumRGB[j] += data[i].color.c[j] * data[i].wheight;
        }
        // printf("%d %d %d\n", data[i].color.c[0], data[i].color.c[1], data[i].color.c[2]);
        num += data[i].wheight;
    }

    color8_3 retVal;
    for (int i = 0; i < 3; i++){
        retVal.c[i] = sumRGB[i]/num;
    }
    // printf("%d %d %d\n\n", retVal.c[0], retVal.c[1], retVal.c[2]);
    
    return retVal;
}

void meanShift(color8_3* data, int dataNum, int range){
    int compresDataSize = 0;
    wheighedColor* compresData = mallocCompresData(data, dataNum, &compresDataSize);

    color8_3* replaceColor = malloc(sizeof(color8_3)*compresDataSize);
    for (int i = 0; i < compresDataSize; i++){
        replaceColor[i] = compresData[i].color;
        // printf("%d %d %d\n", replaceColor[i].c[0], replaceColor[i].c[1], replaceColor[i].c[2]);
    }

    partition3D part3D = mallocPartition3D(compresData, compresDataSize, range);
    // for (int i = 0; i < part3D.dLen*part3D.dLen*part3D.dLen; i++){
    //     if(part3D.partNum[i] == 0){
    //         continue;
    //     }
    //     for (int j = 0; j < part3D.partNum[i]; j++){
    //         for (int z = 0; z < 3; z++){
    //             printf("%d ", part3D.partData[i][j].color.c[z]);
    //         }
    //         printf(", ");   
    //     }
    //     printf("\n");
    // }
    
    
    for(int i = 0; i < compresDataSize;){
        // break;
        // color8_3 newMean = avgColor(compresData, compresDataSize, replaceColor[i], range);
        color8_3 newMean = avgColorWithPartition(part3D, replaceColor[i], range);
        // printf("%d %d %d -> %d %d %d\n", replaceColor[i].c[0], replaceColor[i].c[1], replaceColor[i].c[2], newMean.c[0], newMean.c[1], newMean.c[2]);
        if( colorEqual(replaceColor[i], newMean) ){
            printf("\r%d ", compresDataSize-i);
            i++;
            // printf("\n");
        }else{
            replaceColor[i] = newMean;
        }
        // printf("%d ", i);
    }

    // for (int i = 0; i < compresDataSize; i++){
    //     printf("%d %d %d\n", replaceColor[i].c[0], replaceColor[i].c[1], replaceColor[i].c[2]);
    // }

    // replace to new color
    for (int i = 0; i < dataNum; i++){
        int newColorI = 0;
        for(; !colorEqual(compresData[newColorI].color, data[i]); newColorI++);
        data[i] = replaceColor[newColorI];
    }
    
    freePartition3D(part3D);
    free(replaceColor);
    free(compresData);
}