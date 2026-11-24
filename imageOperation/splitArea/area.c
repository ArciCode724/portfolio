#include"./area.h"
#include<stdio.h>
#include<stdlib.h>

typedef struct{
    uint64_t sSum[3];
    uint64_t sum[3];
    uint64_t n;
} varinacePart;

void printArea(area toPrint){
    printf("sw:%d sh:%d w:%d h:%d\n",
        toPrint.start_width, toPrint.start_height,
        toPrint.width, toPrint.height
    );
}

static varinacePart varPartAddColor(varinacePart a, color8_3 b){
    for (int c = 0; c < 3; c++){
        a.sum[c] += b.c[c];
        a.sSum[c] += b.c[c]*b.c[c];
    }
    a.n++;
    return a;
}

static varinacePart varPartAdd(varinacePart a, varinacePart b){
    for (int c = 0; c < 3; c++){
        a.sum[c] += b.sum[c];
        a.sSum[c] += b.sSum[c];
    }
    a.n += b.n;
    return a;
}

static varinacePart varPartMinus(varinacePart a, varinacePart b){
    for (int c = 0; c < 3; c++){
        a.sum[c] -= b.sum[c];
        a.sSum[c] -= b.sSum[c];
    }
    a.n -= b.n;
    return a;
}

static uint64_t calculateVariance(varinacePart from){
    if(from.n == 0){ return 0;}
    int64_t retVal = 0;
    // printf("n:%d ", from.n);
    for (int c = 0; c < 3; c++){
        // printf("ss:%d s:%d; ", from.sSum[c], from.sum[c]);
        retVal += from.sSum[c]/from.n - (from.sum[c]*from.sum[c])/(from.n*from.n); 
    }
    // printf(" -> %d\n",retVal);
    return retVal;
    /*
        printf("ss:%d s:%d; ", from.sSum[c], from.sum[c]);
        double x = from.sSum[c];
        printf("%d %d -> ", from.sSum[c], (uint64_t)x);
        x /= from.n;
        retVal += x;
        printf("%.2f; ", x);
        x = from.sum[c];
        printf("%.2f -> ", x);
        x /= from.n;
        x *= x;
        printf("%.2f ", x);
        retVal -= x;
    */
}

area areaOfImg(img imgData){
    varinacePart sums = {.sSum = {0,0,0}, .sum = {0,0,0}, .n = 0};
    // varinacePart sums = {.sSum = {0,0,0}, .sum = {0,0,0}, .n = imgData.height*imgData.width};
    for (uint64_t i = 0; i < imgData.height*imgData.width; i++){
        sums = varPartAddColor(sums, imgData.pixels[i]);
        // for (int c = 0; c < 3; c++){
        //     sums.sum[c] += imgData.pixels[i].c[c];
        //     sums.sSum[c] += imgData.pixels[i].c[c]*imgData.pixels[i].c[c];
        // }
    }
    area retVal = {
        .height = imgData.height,
        .width = imgData.width,
        .start_height = 0,
        .start_width = 0,
        .variance = calculateVariance(sums)
    };
    return retVal;
}

static color8_3 avgColor(img imgData, area areaOfIntrest){
    uint64_t sum[3] = {0,0,0};
    uint64_t limitY = areaOfIntrest.start_height + areaOfIntrest.height;
    uint64_t limitX = areaOfIntrest.start_width + areaOfIntrest.width;
    for (uint64_t y = areaOfIntrest.start_height; y < limitY; y++){
        for (uint64_t x = areaOfIntrest.start_width; x < limitX; x++){
            color8_3 pix = imgData.pixels[imgData.width*y + x];
            for (int c = 0; c < 3; c++){
                sum[c] += pix.c[c];
            }
        }
    }

    color8_3 retVal;
    for (int c = 0; c < 3; c++){
        retVal.c[c] = sum[c]/(areaOfIntrest.width*areaOfIntrest.height);
    }
    return retVal;
}

void avgColorInArea(img imgData, area areaOfIntrest){
    // printf("b");
    if( (areaOfIntrest.width*areaOfIntrest.height) == 0){
        return;
    }
    color8_3 newColor = avgColor(imgData, areaOfIntrest);
    printf("color:%d %d %d\n",newColor.c[0],newColor.c[1],newColor.c[2]);

    uint64_t limitY = areaOfIntrest.start_height + areaOfIntrest.height;
    uint64_t limitX = areaOfIntrest.start_width + areaOfIntrest.width;
    for (uint64_t y = areaOfIntrest.start_height; y < limitY; y++){
        for (uint64_t x = areaOfIntrest.start_width; x < limitX; x++){
            // printf("%d %d", y, x);
            imgData.pixels[imgData.width*y + x] = newColor;
        }
    }
}

static uint64_t bestSplit(varinacePart* list, uint64_t listNum, uint64_t outVar[2]){
    varinacePart a = {.sSum = {0,0,0}, .sum = {0,0,0}, .n = 0};
    varinacePart b = {.sSum = {0,0,0}, .sum = {0,0,0}, .n = 0};
    for(uint64_t i = 0; i<listNum; i++){
        b = varPartAdd(b, list[i]);
        // b = varPartMinus(b, list[i]);
        // printf("%d %d %d\n", i, list[i].sum[0], list[i].sSum[0]);
        // printf("%d %d\n", b.sum[0], b.sSum[0]);
    }

    uint64_t splitPaint = 0;
    uint64_t bestVarianceSum = INT64_MAX;

    uint64_t newVarianceA;
    uint64_t newVarianceB;
    printf("\n");
    for (int i = 0; i < listNum; i++){
        newVarianceA = calculateVariance(a);
        newVarianceB = calculateVariance(b);
        printf("%d: %u %u %u\n", i, newVarianceA, newVarianceB, (newVarianceA + newVarianceB));
        if( (newVarianceA + newVarianceB) < bestVarianceSum){
            splitPaint = i;
            bestVarianceSum = newVarianceA + newVarianceB;
            outVar[0] = newVarianceA;
            outVar[1] = newVarianceB;
        }
        
        a = varPartAdd(a, list[i]);
        printf("%u %u ->", b.sum[0], b.sSum[0]);
        b = varPartMinus(b, list[i]);
        printf("%u %u %u\n", b.sum[0], b.sSum[0], b.n);
    }
    newVarianceA = calculateVariance(a);
    newVarianceB = calculateVariance(b);
    if( (newVarianceA + newVarianceB) < bestVarianceSum){
        splitPaint = listNum-1;
        bestVarianceSum = newVarianceA + newVarianceB;
        outVar[0] = newVarianceA;
        outVar[1] = newVarianceB;
    }
    // printf("\n");
    return splitPaint;
}

void spitArea(img imgData, area toSplit, area* out){
    varinacePart* listToSpilt;
    uint64_t limitY = toSplit.start_height + toSplit.height;
    uint64_t limitX = toSplit.start_width + toSplit.width;
    // rows
    listToSpilt = calloc(toSplit.height, sizeof(varinacePart));
    for (uint64_t y = toSplit.start_height; y < limitY; y++){
        uint64_t listI = y - toSplit.start_height;
        // listToSpilt[listI].n = toSplit.width;
        for (uint64_t x = toSplit.start_width; x < limitX; x++){
            uint64_t i = imgData.width*y + x;
            listToSpilt[listI] = varPartAddColor(listToSpilt[listI], imgData.pixels[i]);
            // for (int c = 0; c < 3; c++){
            //     listToSpilt[listI].sum[c] += imgData.pixels[i].c[c];
            //     listToSpilt[listI].sSum[c] += imgData.pixels[i].c[c]*imgData.pixels[i].c[c];
            // }
        }
    }
    uint64_t anserRows[3];
    anserRows[0] = bestSplit(listToSpilt, toSplit.height, anserRows+1);
    free(listToSpilt);
    // printf("\n");
    // columns
    listToSpilt = calloc(toSplit.width, sizeof(varinacePart));
    for (uint64_t x = toSplit.start_width; x < limitX; x++){
        uint64_t listI = x - toSplit.start_width;
        // listToSpilt[listI].n = toSplit.height;
        for (uint64_t y = toSplit.start_height; y < limitY; y++){
            uint64_t i = imgData.width*y + x;
            listToSpilt[listI] = varPartAddColor(listToSpilt[listI], imgData.pixels[i]);
            // for (int c = 0; c < 3; c++){
            //     listToSpilt[listI].sum[c] += imgData.pixels[i].c[c];
            //     listToSpilt[listI].sSum[c] += imgData.pixels[i].c[c]*imgData.pixels[i].c[c];
            // }
        }
    }
    uint64_t anserColumns[3];
    anserColumns[0] = bestSplit(listToSpilt, toSplit.width, anserColumns+1);
    free(listToSpilt);

    out[0] = toSplit; out[1] = toSplit;
    if(anserRows[1] + anserRows[2] < anserColumns[1] + anserColumns[2]){
        // printf("%d %d %d\n", anserRows[0], anserRows[1], anserRows[2]);
        out[0].variance = anserRows[1];
        out[0].height = anserRows[0];

        out[1].variance = anserRows[2];
        out[1].start_height += anserRows[0];
        out[1].height = toSplit.height - anserRows[0];
    }else{
        // printf("%d %d %d\n", anserRows[0], anserRows[1], anserRows[2]);
        out[0].variance = anserColumns[1];
        out[0].width = anserColumns[0];

        out[1].variance = anserColumns[2];
        out[1].start_width += anserColumns[0];
        out[1].width = toSplit.width - anserColumns[0];
    }
}
