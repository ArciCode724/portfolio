#include<stdint.h>
#include<stdlib.h>
#include<stdio.h>
#include"rle.h"

rleText* rleText_init(uint32_t size){
    rleText* retVal = malloc(sizeof(rleText));
    retVal->text = malloc(sizeof(uint16_t)*size);
    retVal->n = 0;
    return retVal;
}

void rleZeros(rleText* save, uint8_t* list, uint32_t n){
    save->n = 0;

    for(uint32_t i = 0; i<n; i++){
        uint32_t zeroCount;
        for(zeroCount = 0; i<n && list[i] == 0; i++)
            zeroCount++;
        if(zeroCount){
            zeroCount++;
            // printf("%d ", zeroCount);
            for(; zeroCount; zeroCount>>=1){
                save->text[save->n] = (zeroCount & 1) + R0;
                save->n++;
            }
            save->n--;

            i--;
            continue;
        }
        save->text[save->n] = (uint16_t)list[i] + 1;
        save->n++;
    }
}

uint32_t unRleZeros(uint8_t* save, rleText* list){

    uint32_t retN = 0;
    for(uint32_t i = 0; i<list->n; i++){
        if(list->text[i] > R1){
            // printf("%d ", list->text[i]);
            save[retN] = (uint8_t)(list->text[i]-1);
            retN++;
            continue;
        }
        uint32_t zeroNum = 0;
        uint32_t bitRead = 1;
        for(; i<list->n && list->text[i] <= R1; i++){
            // printf("%d ", list->text[i]);
            zeroNum |= list->text[i] * bitRead;
            bitRead<<=1;
        }
        i--;
        zeroNum |= bitRead;
        // printf("1 -> %d; ", zeroNum);
        // printf("%d ", zeroNum);
        zeroNum--;
        for(; zeroNum > 0; zeroNum--){
            save[retN] = 0;
            retN++;
        }
    }
    return retN;
}

int getRleFore(FILE* in){
    static int8_t apear = 0;
    static int16_t lastChar = -1;
    
    // printf("<%d>", apear);
    if(apear == 3){
        apear = 0;
        
        uint8_t aditiona;
        int16_t buf = fgetc(in);
        for(aditiona = 0; buf == lastChar && aditiona < 255; aditiona++){
            buf = fgetc(in);
        }
        if(buf != EOF){
            fseek(in, -1, SEEK_CUR);
        }
        lastChar = -1;

        return aditiona;
    }
    
    int loaded = fgetc(in);
    if(loaded == EOF){
        return EOF;
    }
    if(lastChar == loaded){
        apear++;
    }else{
        apear = 0;
        lastChar = loaded;
    }
    
    return loaded;
};

void putRleFore(FILE* out, uint8_t outChar){
    static int8_t apear = 0;
    static int16_t lastChar = -1;
    
    if(apear == 3){
        apear = 0;

        for(; outChar > 0; outChar--){
            fprintf(out,"%c", lastChar);
        }
        lastChar = -1;
        return;
    }

    if(lastChar == outChar){
        apear++;
    }else{
        apear = 0;
        lastChar = outChar;
    }

    fprintf(out, "%c", outChar);
}
