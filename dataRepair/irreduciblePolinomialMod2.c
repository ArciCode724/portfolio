#include<stdio.h>

void printInBin(int number, int len){
    int printPos = 1<<(len-1);
    // printf("%d %d\n", numLen, printPos);
    for (int i = 0; i < len; i++){
        printf("%d", (number&printPos) != 0);
        printPos>>=1;
    }
    printf("\n");
}

int intPow(int a, int b){
    int retVal = 1;
    for(;b; b-=1){
        retVal *= a;
    }
    return retVal;
}

int xorMulti(int a, int b){
    int retVal = 0;
    while (b){
        if( b & 1 == 1){
            retVal ^= a;
        }
        a <<= 1;
        b >>= 1;
    }
    return retVal;
}

int main(){
    // printInBin( xorMulti(0b1100, 0b0011) , 16 );
    // printf("%d", intPow(3,3));
    int numBits = 9;
    int numOfComb = intPow(2,numBits);
    // inicjalizacja
    int isPrime[ numOfComb ];
    for (int i = 0; i < numOfComb; i++){
        isPrime[i] = 1;
    }

    // znalaznie nie redukcyjnych
    for (int i = 2; i < numOfComb; i++){
        if( isPrime[i] == 0 ){
            continue;
        }
        // multiplay
        for( int m = i; m < numOfComb; m++){
            int notPrime = xorMulti(i, m);
            if( notPrime > numOfComb ){
                break;
            }
            isPrime[ notPrime ] = 0;
        }
    }
    
    // wyswietl
    for (int i = 0; i < numOfComb; i++){
        if(isPrime[i] == 0){
            continue;
        }
        printf("%d ", isPrime[i]);
        printInBin(i, numBits);
    }

    return 0;
}