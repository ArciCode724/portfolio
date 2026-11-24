#include<stdint.h>
#include<stdio.h>

void moveToFront(uint8_t * list, uint32_t n){
    uint8_t kolejka[256];
    for(uint16_t i=0; i<256; i++)
        kolejka[i] = (uint8_t)i;

    uint8_t apear;
    for(uint32_t i=0; i<n; i++){
        for(apear = 0; kolejka[apear] != list[i];apear++);
        list[i] = (uint8_t)apear;
        for(uint8_t j = apear; j>0; j--){
            uint8_t temp = kolejka[j];
            kolejka[j] = kolejka[j-1];
            kolejka[j-1] = temp;
        }
    }
}

void unMoveToFront(uint8_t* list, uint32_t n){
    uint8_t kolejka[256];
    for(uint16_t i=0; i<256; i++)
        kolejka[i] = (uint8_t)i;

    uint8_t apear;
    for(uint32_t i=0; i<n; i++){
        apear = list[i];
        list[i] = kolejka[apear];
        for(uint8_t j = apear; j>0; j--){
            uint8_t temp = kolejka[j];
            kolejka[j] = kolejka[j-1];
            kolejka[j-1] = temp;
        }
    }
}