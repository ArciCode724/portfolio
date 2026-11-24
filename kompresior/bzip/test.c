#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"bws.h"

int main(){
    uint32_t numLinst = 16;
    uint8_t testLins[numLinst];
    srand(time(NULL));
    for(uint32_t i = 0; i < numLinst; i++){
        testLins[i] = rand()%4;
        // testLins[i] = i%4;
    }
    uint32_t ofsets[numLinst];
    for(uint32_t i = 0; i<numLinst; i++)
        ofsets[i] = i;
    bws_radix(ofsets, testLins, numLinst);
    printf("\n");

    for(uint32_t i = 0; i < numLinst; i++){
        for(uint32_t j = ofsets[i]; j < numLinst; j++){
            printf("%2X " , testLins[j]);
        }
        for(uint32_t j = 0; j < ofsets[i]; j++){
            printf("%2X " , testLins[j]);
        }
        printf("\n");
    }
}