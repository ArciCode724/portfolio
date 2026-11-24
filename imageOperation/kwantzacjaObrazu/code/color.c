#include"../header/color.h"
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

int findClosect(color8_3* palte, int plateNum, color8_3 from){
    int chosen = 0;
    long long dis = colorDis(palte[0], from);
    for (int i = 1; i < plateNum; i++){
        long long newDis = colorDis(palte[i], from);
        if(newDis < dis){
            chosen = i;
            dis = newDis;
        }
    }
    
    return chosen;
}

static double colorDis5D(colorLF_5 a, colorLF_5 b){
    double retVal = 0;
    for (int i = 0; i < 5; i++){
        double d = a.d[i] - b.d[i];
        retVal += d*d;
    }
    return retVal;
}

int findClosect5D(colorLF_5* palte, int plateNum, colorLF_5 find){
    int chosen = 0;
    double dis = colorDis5D(palte[0], find);
    for (int i = 1; i < plateNum; i++){
        double newDis = colorDis5D(palte[i], find);
        if(newDis < dis){
            chosen = i;
            dis = newDis;
        }
    }
    
    return chosen;
}