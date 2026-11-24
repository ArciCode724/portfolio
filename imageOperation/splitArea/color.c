#include"./color.h"
#include<math.h>
#include<stdio.h>

int colorEqual(color8_3 colorA, color8_3 colorB){
    // printf("%d %d %d -> %d %d %d\n", colorA.c[0], colorA.c[1], colorA.c[2], colorB.c[0], colorB.c[1], colorB.c[2]);
    for (int i = 0; i < 3; i++){
        if( colorA.c[i] != colorB.c[i]){
            return 0;
        }
    }
    return 1;
}

long long colorDis(color8_3 colorA, color8_3 colorB){
    long long retVal = 0;
    for (int i = 0; i < 3; i++){
        long long d = colorA.c[i] - colorB.c[i];
        retVal += d*d;
    }
    return retVal;
}

static double colorDis5D(colorLF_5 a, colorLF_5 b){
    double retVal = 0;
    for (int i = 0; i < 5; i++){
        double d = a.d[i] - b.d[i];
        retVal += d*d;
    }
    return retVal;
}