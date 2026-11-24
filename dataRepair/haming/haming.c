#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>

void printInBin(int number, int len){
    int printPos = 1<<(len-1);
    // printf("%d %d\n", numLen, printPos);
    for (int i = 0; i < len; i++){
        printf("%d", (number&printPos) != 0);
        printPos>>=1;
    }
    printf("\n");
}

void printList(uint8_t* data, int dataNum){
    for (int i = 0; i < dataNum; i++){
        printf("%d ", data[i]);
    }
    printf("\n");
}

int xorOnes(uint8_t* data, int dataNum){
    int retVal = 0;
    for (int i = 0; i < dataNum; i++){
        if(data[i]){
            continue;
        }
        retVal ^= i+1;
    }
    return retVal;
}

void guardData(uint8_t* data, int dataNum){
    int repir = xorOnes(data, dataNum);
    printInBin(repir, ceil(log2(dataNum+0.5)) );
    for(int i=1; i < dataNum; i <<= 1){
        if( (repir&i) == 0){
            continue;
        }
        data[i-1] = !data[i-1];
    }
}

void fixData(uint8_t* data, int dataNum){
    int repir = xorOnes(data, dataNum);
    printInBin(repir, ceil(log2(dataNum+0.5)) );
    if(repir == 0){
        return;
    }
    data[repir-1] = !data[repir-1];
}

int main(){
    srand(time(NULL));

    int dataNum = 7;
    uint8_t data[dataNum];
    for (int i = 0; i < dataNum; i++){
        data[i] = rand() % 2;
    }

    printList(data, dataNum);
    guardData(data, dataNum);
    printList(data, dataNum);

    int randChange = rand() % dataNum;
    data[randChange] = !data[randChange];
    printList(data, dataNum);

    fixData(data, dataNum);
    printList(data, dataNum);

    return 0;
}