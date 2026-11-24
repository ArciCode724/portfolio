#include<stdio.h>
#include<stdint.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>

int modAdd(int a, int b, int mod){
    int retVal = a + b;
    while (retVal >= mod){
        retVal -= mod;
    }
    return retVal;
}

int modMinus(int a, int b, int mod){
    int retVal = a - b;
    while (retVal >= mod){
        retVal += mod;
    }
    return retVal;
}

int modMulti(int a, int b, int mod){
    int retVal = a * b;
    while (retVal >= mod){
        retVal -= mod;
    }
    return retVal;
}

int modDivade(int a, int b, int mod){
    int reverse = 1;
    for (int i = 1; i < mod; i++){
        int newChek = i*mod + 1;
        if( newChek % b == 0 ){
            return modMulti(a, newChek/b, mod);
        }
    }
    return 0;
} 

void addPolynom(int* a, int na, int* b, int nb, int* o, int mod){
    int small = (na < nb) ? na : nb;
    int big = (na > nb) ? na : nb;

    int i = 0;
    for (; i < small; i++){
        o[i] = modAdd(a[i], b[i], mod);
        // o[i] = a[i] + b[i];
    }
    for (; i < big; i++){
        o[i] = (na > nb) ? a[i] : b[i];
        // o[i] = b[i];
    }
}

void multiPolynom(int* a, int na, int* b, int nb, int* o, int mod){
    for(int i = 0; i < nb + na - 1; i++){
        o[i] = 0;
    }
    for(int i = 0; i < nb; i++){
        for (int j = 0; j < na; j++){
            o[i+j] = modAdd(o[i+j], modMulti(a[j], b[i], mod), mod);
            // o[i+j] += a[j] * b[i];
        }
    }
}

int polyValuAt(int* p, int np, int x, int mod){
    int retVal = 0;
    for (int i = np-1; i>=0; i--){
        retVal = modMulti(retVal, x, mod);
        retVal = modAdd(retVal, p[i], mod);
        // retVal *= x;
        // retVal += p[i];
    }
    return retVal;
}

int main(){
    // printf("%d\n", modDivade(1,2,4));
    srand(time(NULL));
    int mod = 7;

    int dataNum = 3;
    int saveData = 2;
    int data[dataNum + saveData];
    for (int i = 0; i < dataNum; i++){
        data[i] = rand() % mod;
        // data[i] = i+3;
    }

    // 1, x, x^2, ... x^n
    int mainPolynom[dataNum];
    for (int i = 0; i < dataNum; i++){
        mainPolynom[i] = 0;
    }
    
    for (int i = 0; i < dataNum; i++){
        int newPol[dataNum];
        for(int j=0; j < dataNum; j++){
            newPol[j] = 0;
        }
        newPol[0] = 1;

        int outPol[dataNum];
        int partOfPol[2] = {0,1};
        for(int j=0; j < dataNum; j++){
            if(i==j){ continue;}
            partOfPol[0] = mod - j;
            // printf("(%d %d) ", partOfPol[0], partOfPol[1]);
            multiPolynom(newPol, dataNum, partOfPol, 2, outPol, mod);
            for (int z = 0; z < dataNum; z++){
                newPol[z] = outPol[z];
                // printf("%d ", newPol[z]);
            }
            // printf("\n");
        }
        // printf("\n");
        int divBy = polyValuAt(newPol, dataNum, i, mod);
        // printf("%d\n", divBy);
        for (int z = 0; z < dataNum; z++){
            // printf("%d - > ", newPol[z]);
            newPol[z] = modDivade(newPol[z], divBy, mod);
            newPol[z] = modMulti(newPol[z], data[i], mod);
            // printf("%d; ", newPol[z]);
        }
        // printf("\n");

        addPolynom(mainPolynom, dataNum, newPol, dataNum, outPol, mod);
        for (int z = 0; z < dataNum; z++){
            mainPolynom[z] = outPol[z];
        }
    }

    for (int i = dataNum; i < dataNum + saveData; i++){
        data[i] = polyValuAt(mainPolynom, dataNum, i, mod);
    }
    for (int i = 0; i < dataNum; i++){
        printf("%d ", mainPolynom[i]);
    }
    printf("\n");
    for (int i = 0; i < dataNum + saveData; i++){
        printf("(%d %d) ", data[i], polyValuAt(mainPolynom, dataNum, i, mod) );
        // printf("%d ", data[i]);
    }
    printf("\n");
    
}