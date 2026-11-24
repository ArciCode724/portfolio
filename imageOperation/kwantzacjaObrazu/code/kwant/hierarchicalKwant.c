#include"../../header/compresColors.h"
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

typedef struct _cluser1D{
    uint64_t num;
    double sSum; // x^2
    double sum; // x
} cluser1D;

#define swap(x,y){typeof(x) t=x;x=y;y=t;}

static double var1D(cluser1D a, cluser1D b){
    // printf("%6.0f %6.0f %d; ", a.sSum, a.sum, a.num);
    // printf("%6.0f %6.0f %d; ", b.sSum, b.sum, a.num);
    double x2 = (a.sSum + b.sSum)/(a.num+b.num);
    double x = (a.sum + b.sum)/(a.num+b.num);
    x *= x;
    // printf("%6.2f %6.2f %6.2f\n",x2,x, x2-x);
    return x2 - x;
}

static cluser1D mergeCluser1D(cluser1D a, cluser1D b){
    cluser1D retVal;
    retVal.num = a.num + b.num;
    retVal.sSum = a.sSum + b.sSum;
    retVal.sum = a.sum + b.sum;
    return retVal;
}

void hierarchicalKwant1D(uint8_t* data, int dataNum, uint8_t* outPalet, int* paletSize){
    uint32_t compresDataSize = 0;
    wheighedColor1D* compresData = mallocCompresData1D(data, dataNum, &compresDataSize);

    cluser1D* newPalet = malloc(sizeof(cluser1D)*compresDataSize);
    int newPaletNum = compresDataSize;

    for (uint32_t i = 0; i < compresDataSize; i++){
        // printf("%d %d\n", compresData[i].wheight, compresData[i].color);
        newPalet[i].num = compresData[i].wheight;
        newPalet[i].sum = compresData[i].color * compresData[i].wheight;
        newPalet[i].sSum = compresData[i].color * compresData[i].color * compresData[i].wheight;
    }
    free(compresData);

    while (*paletSize < newPaletNum){
        // for (uint32_t i = 0; i < newPaletNum; i++){
        //     printf("%6.3f\n", newPalet[i].sum/newPalet[i].num);
        // }

        int combine[2] = {0,0};
        double bestVar = INFINITY;
        for (int i = 1; i < newPaletNum; i++){
            double newVar = var1D(newPalet[i-1], newPalet[i]);
            // printf("%13.3f ", newVar);
            if(newVar >= bestVar){continue;}
            combine[0] = i-1; combine[1] = i; 
            bestVar = newVar;
        }
        // printf("%d %d\n\n", combine[0],combine[1]);

        newPalet[combine[0]] = mergeCluser1D(newPalet[combine[0]], newPalet[combine[1]]);
        for (int i = combine[1]+1; i < newPaletNum; i++){
            newPalet[i-1] = newPalet[i];
        }
        newPaletNum--;
    }

    // for (uint32_t i = 0; i < newPaletNum; i++){
    //     printf("%6.3f\n", newPalet[i].sum/newPalet[i].num);
    // }
    
    for (uint32_t i = 0; i < newPaletNum; i++){
        outPalet[i] = newPalet[i].sum/newPalet[i].num;
    }

    free(newPalet);
}

// ---

typedef struct _cluser3D{
    uint64_t num;
    double sSum[3]; // x^2
    double sum[3]; // x
} cluser3D;

typedef struct _node3d{
    cluser3D cluter;
    double bestVar;
    int target;
} node3d;

static double varOf(cluser3D a){
    double retVal = 0;
    for (int i = 0; i < 3; i++){
        double x2 = a.sSum[i]/a.num;
        double x = a.sum[i]/a.num;
        x *= x;
        retVal += x2 - x;
    }
    return retVal;
}

static double varDiff3D(cluser3D a, cluser3D b){
    double retVal = 0;
    for (int i = 0; i < 3; i++){
        double x2 = (a.sSum[i] + b.sSum[i])/(a.num+b.num);
        double x = (a.sum[i] + b.sum[i])/(a.num+b.num);
        x *= x;
        retVal += x2 - x;
    }
    return retVal;
}

static cluser3D mergeCluser3D(cluser3D a, cluser3D b){
    cluser3D retVal;
    retVal.num = a.num + b.num;
    for (int i = 0; i < 3; i++){
        retVal.sSum[i] = a.sSum[i] + b.sSum[i];
        retVal.sum[i] = a.sum[i] + b.sum[i];
        // printf("%f + %f = %f\n", a.sum[i], b.sum[i], retVal.sum[i]);
    }
    return retVal;
}

static int closestCluser3D(cluser3D** list, int listNum, int to){
    int retVal;
    double varChange = INFINITY;
    // uint64_t changeNum = INFINITY;
    int range[2][2] = {{0,to},{to+1,listNum}};
    for(int i = 0; i < 2; i++){
        for (int x = range[i][0]; x < range[i][1]; x++){
            double newChange = varDiff3D( *(list[to]), *(list[x]) );
            // uint64_t newChangeNum = list[to]->num + list[x]->num;
            // printf("%d %d %f ; ", to, x, newChange);
            if(newChange > varChange){ continue;}
            // if(newChange == varChange && newChangeNum > changeNum){ continue;}
            retVal = x;
            varChange = newChange;
        }
    }
    return retVal;
}

static void innitNodePoint(node3d* list, int listNum, int nodeId){
    list[nodeId].bestVar = INFINITY;
    for (int i = 0; i < listNum; i++){
        if(nodeId == i){continue;}
        double newVar = varDiff3D(list[nodeId].cluter, list[i].cluter);
        if(newVar < list[nodeId].bestVar){
            list[nodeId].bestVar = newVar;
            list[nodeId].target = i;
        }
    }
}

static void updateNodeCluster(node3d* list, int listNum, int nodeId, cluser3D newCluser){
    list[nodeId].cluter = newCluser;
    list[nodeId].bestVar = INFINITY;
    for (int i = 0; i < listNum; i++){
        if(nodeId == i){continue;}
        double newVar = varDiff3D(list[nodeId].cluter, list[i].cluter);
        if(newVar < list[nodeId].bestVar){
            list[nodeId].bestVar = newVar;
            list[nodeId].target = i;
        }
        if(newVar < list[i].bestVar){
            list[i].bestVar = newVar;
            list[i].target = nodeId;
        }
    }
}

static void removeNode(node3d* list, int listNum, int nodeId){
    // printf("%d %d\n",listNum, nodeId);
    list[nodeId] = list[--listNum];
    // printf("%d\n",listNum);
    for (int i = 0; i < listNum; i++){
        // printf("%d ",list[i].target);
        if(list[i].target == listNum){
            // printf("A");
            // printf("%d ->",list[i].target);
            list[i].target = nodeId;
            // printf("%d; ",list[i].target);
        }
        if(list[i].target == nodeId){
            innitNodePoint(list, listNum, i);
        }
    }
    // printf("\n");
}

void hierarchicalKwant3D(color8_3* data, int dataNum, color8_3* outPalet, int* paletSize){
    // build innit grups
    int compresDataSize = 0;
    wheighedColor* compresData = mallocCompresData(data, dataNum, &compresDataSize);
    if(*paletSize > compresDataSize){ *paletSize = compresDataSize;}
    node3d* newPalet = malloc(sizeof(node3d)*compresDataSize);
    for (int i = 0; i < compresDataSize; i++){
        newPalet[i].cluter.num = compresData->wheight;
        for (int j = 0; j < 3; j++){
            newPalet[i].cluter.sum[j] = compresData[i].color.c[j];
            newPalet[i].cluter.sSum[j] = compresData[i].color.c[j] * compresData[i].color.c[j];

            newPalet[i].cluter.sum[j] *= compresData[i].wheight;
            newPalet[i].cluter.sSum[j] *= compresData[i].wheight;
        }
    }
    int newPaletNum = compresDataSize;
    free(compresData);
    // for (int i = 0; i < newPaletNum; i++){ innitNodePoint(newPalet, newPaletNum, i);}
    // for (int i = 0; i < newPaletNum; i++){
    //     printf("%d %d\n", i, newPalet[i].target);
    // }
    
    // reduse grups num
    while (newPaletNum > *paletSize){
        printf("\r%d", newPaletNum);
        // for (int i = 0; i < newPaletNum; i++){
        //     printf("%3d :",i);
        //     for (int j = 0; j < 3; j++){
        //         printf("%3d ", (uint64_t)newPalet[i]->sum[j] / newPalet[i]->num);
        //     }
        //     printf("\n");
        // }
        // printf("\n");

        int combine[2] = {0,0};
    //     while (1){
    //         int clasest = closestCluser3D(newPalet, newPaletNum, combine[1]);
    //         if(combine[0] == clasest){ break;}
    //         combine[0] = combine[1];
    //         combine[1] = clasest;
    // //         // printf("%d %d\n", combine[0],combine[1]);
    //     }
        double minDiff = INFINITY;
        for (int i = 0; i < newPaletNum; i++){
            for (int j = i+1; j < newPaletNum; j++){
                double newDiff = varDiff3D(newPalet[i].cluter, newPalet[j].cluter);
                if(newDiff >= minDiff){ continue;};
                // printf("%3d %3d : %f -> %f\n", i,j,minDiff,newDiff);
                minDiff = newDiff;
                combine[0] = i;
                combine[1] = j;
            }            
        }
        // for (int i = 0; i < newPaletNum; i++){ innitNodePoint(newPalet, newPaletNum, i);}
        // int chosenNode = 0;
        // for (int i = 1; i < newPaletNum; i++){
        //     if(newPalet[i].bestVar < newPalet[chosenNode].bestVar){
        //         chosenNode = i;
        //     }
        // }

        // cluser3D newCluseter = mergeCluser3D( newPalet[chosenNode].cluter, newPalet[newPalet[chosenNode].target].cluter );
        // removeNode(newPalet, newPaletNum--, newPalet[chosenNode].target);
        // updateNodeCluster(newPalet, newPaletNum, chosenNode, newCluseter);

        newPalet[combine[0]].cluter = mergeCluser3D( newPalet[combine[0]].cluter, newPalet[combine[1]].cluter );
        newPalet[combine[1]] = newPalet[--newPaletNum];
    }

    // ret color
    for (int i = 0; i < newPaletNum; i++){
        for (int j = 0; j < 3; j++){
            outPalet[i].c[j] = newPalet[i].cluter.sum[j] / newPalet[i].cluter.num;
            // printf("%3d ", outPalet[i].c[j]);
        }
        // printf("\n");
    }

    free(newPalet);
}