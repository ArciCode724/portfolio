#include<stdlib.h>
#include<stdio.h>
#include"../../header/kwant.h"
#include"../../header/compresColors.h"
#include<math.h>
#include<limits.h>

#include"../../header/kdtree.h"

typedef struct _pointHolder{
    void* pToData;
    int sorce;
} pointHolder;

static void printColor(const void* a){
    color8_3* pa = ((pointHolder*)a)->pToData;
    printf("%d: ", ((pointHolder*)a)->sorce);
    for (int i = 0; i < 3; i++){
        printf("%d ", pa->c[i]);
    }
}

static int compColor8_3(const void* a, const void* b, int d){
    color8_3* pa = ((pointHolder*)a)->pToData;
    color8_3* pb = ((pointHolder*)b)->pToData;
    return pa->c[d] - pb->c[d];
}

static double distColor8_3(const void* a, const void* b, int d){
    color8_3* pa = ((pointHolder*)a)->pToData;
    color8_3* pb = ((pointHolder*)b)->pToData;
    // printColor(*pa);
    // printColor(*pb);

    double sum = 0;
    double part[3];
    for (int i = 0; i < 3; i++){
        part[i] = (double)(pa->c[i]) - (double)(pb->c[i]);
        part[i] *= part[i];
        sum += part[i];
    }
    if(d==-1){
        return sum;
    }
    return part[d];
}

void kmeanKwant1D(uint8_t* data, int dataNum, uint8_t* outPalet, int* paletSize){
    // uint32_t compresDataSize = 0;
    // wheighedColor1D* compresData = mallocCompresData1D(data, dataNum, &compresDataSize);
    // if( compresDataSize < paletSize){
    //     *paletSize = compresDataSize;
    //     paletSize = compresDataSize;
    // }
    // uint8_t centriod[*paletSize];
    // free(compresData);
}

static void ppStartPalte(color8_3* out, int outNum, wheighedColor* data, int dataNum){
    int curentOutSize = 1;
    out[0] = data[0].color;
    for (; curentOutSize < outNum; curentOutSize++){
        double distance = 0;
        color8_3 newColor = data[0].color;

        for (int i = 1; i < dataNum; i++){
            double myDystance = INFINITY;

            for (int j = 0; j < curentOutSize; j++){
                double dystaneBetwen = colorDis(data[i].color, out[j]);
                dystaneBetwen = dystaneBetwen*dystaneBetwen * data[i].wheight;
                if(dystaneBetwen < myDystance){ myDystance = dystaneBetwen; }
            }
            
            if(distance < myDystance){
                distance = myDystance;
                newColor = data[i].color;
            }
        }
        
        out[curentOutSize] = newColor;
    }
    
}

void kmeanKwant(color8_3* data, int dataNum, color8_3* outPalet, int* paletSizePointer){
    int compresDataSize = 0;
    wheighedColor* compresData = mallocCompresData(data, dataNum, &compresDataSize);
    int paletSize = *paletSizePointer;
    if( compresDataSize < paletSize){
        *paletSizePointer = compresDataSize;
        paletSize = compresDataSize;
    }

    color8_3 curentPalet[paletSize];
    color8_3 newPalet[paletSize];
    struct {
        double sumRGB[3]; 
        uint32_t num;
    } avgPoint[paletSize];
    
    for (int i = 0; i < paletSize; i++){
        color8_3 a = compresData[ (i+1)*compresDataSize/(paletSize+1) ].color;
        curentPalet[i] = a;
    }
    // ppStartPalte(curentPalet, paletSize, compresData, compresDataSize);

    int changeChapen = 1000;
    for (int try = 0; changeChapen > 0; try++){
        // printf("%d ", changeChapen);
        for (int i = 0; i < paletSize; i++){
            for (int j = 0; j < 3; j++){
                avgPoint[i].sumRGB[j] = 0.0;
            }
            avgPoint[i].num = 0;
        }

        pointHolder dataForKD[paletSize];
        for (int i = 0; i < paletSize; i++){
            dataForKD[i].pToData = curentPalet+i;
            dataForKD[i].sorce = i;
        }
        kdtree* findTree = createKdtree(dataForKD, paletSize, sizeof(pointHolder), 3, compColor8_3);
        // kdtreePrint(findTree, printColor);

        for (int i = 0; i < compresDataSize; i++){
            pointHolder out;
            pointHolder findTo = {&(compresData[i].color), 0};
            kdtreeFindClosest(&out, &findTo, findTree, distColor8_3);
            int whichCluster = out.sorce;
            // printColor(&out);
            // printColor(&findTo);
            // printf(" %lf", distColor8_3(&out, &findTo, -1));
            // printf("\n");
            // int whichCluster = findClosect(curentPalet, paletSize, compresData[i].color);
            for (int j = 0; j < 3; j++){
                avgPoint[whichCluster].sumRGB[j] += (double)compresData[i].color.c[j] * compresData[i].wheight;
            }
            avgPoint[whichCluster].num += compresData[i].wheight;
        }
        freeKdtree(findTree);
        // printf("\n");
    
        for (int i = 0; i < paletSize; i++){
            for (int j = 0; j < 3; j++){
                newPalet[i].c[j] = (uint8_t)(avgPoint[i].sumRGB[j]/avgPoint[i].num);
            }
        }
        
        changeChapen = 0;
        for (int i = 0; i < paletSize; i++){
            // if(
            //     !changeChapen && (
            //         curentPalet[i].r != newPalet[i].r
            //         || curentPalet[i].g != newPalet[i].g
            //         || curentPalet[i].b != newPalet[i].b
            //     )
            // ){ changeChapen = 1; }
            for (int j = 0; j < 3; j++){
                changeChapen += abs(curentPalet[i].c[j] - newPalet[i].c[j]);
            }
            curentPalet[i] = newPalet[i];
        }
    }
    
    for (int i = 0; i < paletSize; i++){
        outPalet[i] = curentPalet[i];
    }
    
    free(compresData);
}

void kmeanKwant5D(color8_3* data, int dataWitdh, int dataHeight, colorLF_5* outPalet, int* pointPaletSize){
    colorLF_5* floatData = malloc(dataWitdh*dataHeight*sizeof(colorLF_5));
    int dataNum = dataWitdh*dataHeight;
    int paletSize = *pointPaletSize;

    for(int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            colorLF_5 new = {
                0,0,0,
                (double)x / dataWitdh,
                (double)y / dataHeight
            };
            for (int i = 0; i < 3; i++){
                new.d[i] = data[y*dataWitdh+x].c[i]/255.0;
            }
            floatData[y*dataWitdh+x] = new;
        }
    }

    colorLF_5 curentPalet[paletSize];
    colorLF_5 newPalet[paletSize];
    struct {
        colorLF_5 color; 
        uint32_t num;
    } avgPoint[paletSize];

    for (int i = 0; i < paletSize; i++){
        colorLF_5 a = floatData[ (i+1)*dataNum/(paletSize+1) ];
        curentPalet[i] = a;
    }

    double changeChapen = 1000;
    for (int try = 0; changeChapen > 0; try++){
        for (int i = 0; i < paletSize; i++){
            for (int j = 0; j < 5; j++){
                avgPoint[i].color.d[j] = 0.0;
            }
            avgPoint[i].num = 0;
        }

        for (int i = 0; i < dataNum; i++){
            int whichCluster = findClosect5D(curentPalet, paletSize, floatData[i]);
            for (int j = 0; j < 5; j++){
                avgPoint[whichCluster].color.d[j] += floatData[i].d[j] ;
            }
            avgPoint[whichCluster].num++;
        }
    
        for (int i = 0; i < paletSize; i++){
            for (int j = 0; j < 5; j++){
                newPalet[i].d[j] = avgPoint[i].color.d[j]/avgPoint[i].num;
            }
        }
        
        changeChapen = 0;
        for (int i = 0; i < paletSize; i++){
            for (int j = 0; j < 5; j++){
                changeChapen += fabs(curentPalet[i].d[j] - newPalet[i].d[j]);
            }
            curentPalet[i] = newPalet[i];
        }
    }

    for (int i = 0; i < paletSize; i++){
        outPalet[i] = curentPalet[i];
    }
    
    free(floatData);
}