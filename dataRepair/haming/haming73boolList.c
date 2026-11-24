#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<stdlib.h>

void printBoolList(uint8_t* l, int n){
    for (int i = 0; i < n; i++){
        printf("%d", l[i]==1);
    }
    // for (int i = 0; i < n; i++){
    //     if(l[i]){
    //         printf("\\overline{%c}",'a'+i);
    //     }else{
    //         printf("%c", 'a'+i );
    //     }
    // }
}

void addOne(uint8_t* l, int n){
    int i = n-1;
    while (i >= 0 && l[i]){
        l[i] = 0;
        i--;
        if(i < 0){
            return;
        }
    }
    l[i] = 1;
}

void addHamingBytes(uint8_t l[7]){
    // (1 1 0 1)(x_0)   (x_4)
    // (1 0 1 1)(x_1) = (x_5)
    // (0 1 1 1)(x_2)   (x_6)
    //          (x_3)
    l[4] = l[0]^l[1]^l[3];
    l[5] = l[0]^l[2]^l[3];
    l[6] = l[1]^l[2]^l[3];
}

void fixHamingFront(uint8_t l[7]){
    // (1 1 0 1)(x_0)   (x_4)
    // (1 0 1 1)(x_1) = (x_5)
    // (0 1 1 1)(x_2)   (x_6)
    //          (x_3)
    uint8_t t[3];
    t[0] = l[0]^l[1]^l[3]^l[4];
    t[1] = l[0]^l[2]^l[3]^l[5];
    t[2] = l[1]^l[2]^l[3]^l[6];

    l[0] ^=  t[0] &  t[1] & ~t[2];
    l[1] ^=  t[0] & ~t[1] &  t[2];
    l[2] ^= ~t[0] &  t[1] &  t[2];
    l[3] ^=  t[0] &  t[1] &  t[2];
}

int main(){
    srand(time(NULL));
    uint8_t list[7] = {0,0,0,0,0,0,0};

    for (int i = 0; i < 4; i++){
        list[i] = rand() % 256;
        // list[i] = 1;
    }

    addHamingBytes(list);
    for (int i = 0; i < 7; i++){
        printf("%02x ", list[i]);
    }
    printf("\n");

    list[0] = 0;

    fixHamingFront(list);
    for (int i = 0; i < 7; i++){
        printf("%02x ", list[i]);
    }
    printf("\n");

    return 0;
}