#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include"bws.h"

void printCircle(uint8_t* base, uint32_t n, uint32_t ofset){
    for(int i=ofset; i < n; i++)
        printf("%c", base[i]);
    for(int i=0; i < ofset; i++)
        printf("%c", base[i]);
    printf("\n");
}

short compOfset(uint8_t* base, uint32_t n, uint32_t ofsetA, uint32_t ofsetB){
    for(int i=0; i<n; i++){
        if(ofsetA >= n)
            ofsetA -= n;
        if(ofsetB >= n)
            ofsetB -= n;

        if(base[ofsetA] > base[ofsetB])
            return 1;
        else if(base[ofsetA] < base[ofsetB])
            return -1;

        ofsetA++;
        ofsetB++;
    }
    return 0;
}

uint32_t bws_qSortSplit(uint32_t* list, uint8_t* base, uint32_t n, uint32_t start, uint32_t end){
    int temp;
    int pivit = list[start];
    int left = start+1;
    for(int i = start+1; i<end; i++){
        if(compOfset(base, n, pivit, list[i]) == 1){
            temp = list[left];
            list[left] = list[i];
            list[i] = temp;
            left++;
        }
    }
    list[start] = list[left-1];
    list[left-1] = pivit;
    return left;
}

void bws_qSort(uint32_t* list, uint8_t* base, uint32_t n, uint32_t start, uint32_t end){
    if(end - start <= 1)
        return;
    uint32_t split = bws_qSortSplit(list, base, n, start, end);
    bws_qSort(list, base, n, start, split);
    bws_qSort(list, base, n, split, end);
}

void bws_shellSort(uint32_t* list, uint8_t* base, uint32_t n){
    for(uint32_t jump = n/2; jump > 0; jump/=2){
        for(uint32_t i=jump; i<n; i++){
            for(uint32_t j=i; j>=jump && compOfset(base, n, list[j-jump], list[j]) == 1; j-=jump){
                uint32_t temp = list[j];
                list[j] = list[j-jump];
                list[j-jump] = temp;
            }
        }
    }
}

uint8_t getValue(uint32_t ofset, uint8_t* base, uint32_t numBase, uint32_t takeFromIndex){
    if(takeFromIndex >= numBase){
        takeFromIndex = numBase - 1;
    }
    takeFromIndex += ofset;
    if(takeFromIndex >= numBase){
        takeFromIndex -= numBase;
    }
    return base[takeFromIndex];
}

void bws_radix(uint32_t* list, uint8_t* base, uint32_t n){
    // sorting from fron to back
    uint32_t* listFrom = list;
    uint32_t* listTo = malloc(sizeof(uint32_t)*n);

    uint32_t counder[257]; // ostatni to suma 
    uint8_t baseZnak;
    // struct {
    //     uint32_t range[n+1];
    //     uint32_t deph[n];
    //     uint32_t numStack;
    // } rangeStack;
    struct {
        uint32_t* range;
        uint32_t* deph;
        uint32_t numStack;
    } rangeStack;
    rangeStack.range = malloc(sizeof(uint32_t)*(n+1));
    rangeStack.deph = malloc(sizeof(uint32_t)*n);

    rangeStack.range[0] = 0;
    rangeStack.range[1] = n;
    rangeStack.deph[0] = 0;
    rangeStack.numStack = 1;
    // for(int i=2; i>0;i--){
    while (rangeStack.numStack > 0){
        // for(int i = 0; i<=rangeStack.numStack;i++){
        //     printf("%d <%d> ", rangeStack.range[i], rangeStack.deph[i]);
        // }
        // printf("\n");
        uint32_t start = rangeStack.range[rangeStack.numStack-1];
        uint32_t end = rangeStack.range[rangeStack.numStack];
        uint32_t deph = rangeStack.deph[rangeStack.numStack-1];
        // printf("\r%3d-%d", deph, end);
        rangeStack.numStack--;
        if(end - start <= 1 || deph>=n){
            continue;
        }

        // printf("\r%lld ", deph);
        // //zerowanie couter
        for(uint32_t i=0; i< 257; i++){
            counder[i] = 0;
        }
        // // zliczanie znakow
        for(uint32_t i=start; i < end; i++){
            counder[getValue(listFrom[i], base, n, deph)]++;
        }
        // // sumowanie
        counder[0] += start;
        for(uint32_t i=1; i< 257; i++){
            counder[i] += counder[i-1];
        }

        // // sort
        for(int64_t i=end-1; i >= start; i--){
            baseZnak = getValue(listFrom[i], base, n, deph);
            counder[baseZnak]--;
            listTo[counder[baseZnak]] = listFrom[i];
        }

        // kopiowanie
        for(uint32_t i=start; i < end; i++){
            listFrom[i] = listTo[i];
        }

        // dodawanae do staka
        deph++;
        for(uint32_t i=1; i< 257; i++){
            if(counder[i-1] == counder[i]){
                continue;
            }
            rangeStack.range[rangeStack.numStack+1] = counder[i];
            rangeStack.deph[rangeStack.numStack] = deph;
            rangeStack.numStack++;
        }
        // break;
    }
    
    free( rangeStack.range );
    free( rangeStack.deph );
    free(listTo);

    for(uint32_t i=0; i< n; i++){
        list[i] = listFrom[i];
    }
}

uint32_t make_bws(uint8_t* text, uint32_t n){
    uint32_t* ofsets = malloc(sizeof(uint32_t)*n);
    for(uint32_t i = 0; i<n; i++)
        ofsets[i] = i;
    // bws_qSort(ofsets, text, n, 0, n);
    // bws_shellSort(ofsets, text, n);
    bws_radix(ofsets, text, n);

    // retVal->n = n;
    uint32_t origin=0;
    for(;ofsets[origin];origin++);
    uint8_t* newText = malloc(n);
    for(uint32_t i = 0; i<n; i++){
        if(ofsets[i]==0){
            newText[i] = text[n-1];
        }else{
            newText[i] = text[ofsets[i]-1];
        }
    }
    free(ofsets);
    for(uint32_t i = 0; i<n; i++)
        text[i] = newText[i];
    free(newText);
    return origin;
}

void unMake_bws(uint8_t* text, uint32_t n, uint32_t startRow){
    // uint8_t row0[n];
    // uint32_t originPos[n];
    uint8_t* row0 = malloc(sizeof(uint8_t)*n);
    uint32_t* originPos = malloc(sizeof(uint32_t)*n);
    for(uint32_t i=0; i<n; i++){
        row0[i] = text[i];
        originPos[i] = i;
    }

    // shell sortuje po row0 a nastepnie originPos laczac te listy
    for(uint32_t jump = n/2; jump > 0; jump/=2){
        for(uint32_t i=jump; i<n; i++){
            for(uint32_t j=i; j>=jump; j-=jump){
                if(row0[j-jump] < row0[j]){
                    break;
                }
                if(row0[j-jump] == row0[j] && originPos[j-jump] < originPos[j]){
                    break;
                }

                uint32_t temp = row0[j-jump];
                row0[j-jump] = row0[j];
                row0[j] = temp;

                temp = originPos[j-jump];
                originPos[j-jump] = originPos[j];
                originPos[j] = temp;
            }
        }
    }

    for(uint32_t i=0; i<n; i++){
        text[i] = row0[startRow];
        startRow = originPos[startRow];
    }

    free(row0);
    free(originPos);
}