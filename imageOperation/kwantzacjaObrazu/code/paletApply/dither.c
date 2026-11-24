#include"../../header/paletApply.h"
#include<stdlib.h>
// #include<stdint.h>
// #include<stdio.h>
// #include<math.h>

typedef struct _vec3{
    int32_t d[3];
} vec3;

static void addVec3(vec3* to, vec3 from, int mulit, int dive){
    for (int i = 0; i < 3; i++){
        to->d[i] += from.d[i]*mulit/dive;
    }
}

static int32_t vec3Dis(vec3 a, color8_3 b){
    int32_t retVal = 0;
    for (int i = 0; i < 3; i++){
        int32_t d = a.d[i] - b.c[i];
        retVal += d*d;
    }
    return retVal;
}

static int copyifCanDataVec3(color8_3* data, int dataWitdh, int dataHeight, int start, vec3* out){
    // printf("%d %d %d\n", start, dataWitdh, dataHeight);
    if(start/dataWitdh >= dataHeight){
        // printf("a");
        return 0;
    }
    // printf("%d ",start);
    for (int i = 0; i < dataWitdh; i++){
        for (int j = 0; j < 3; j++){
            out[i].d[j] = data[i+start].c[j];
        }
        // printf("(%3d %3d %3d) ", out[i].r, out[i].g, out[i].b);
    }
    // printf("\n");
    return 1;
}

static vec3 clampVec3(vec3 v){
    for (int i = 0; i < 3; i++){
        if(v.d[i]<0){ v.d[i] = 0;}
        else if(v.d[i] > 255){ v.d[i] = 255;}
    }
    return v;
}

void ditherApply(color8_3* data, int dataWitdh, int dataHeight, color8_3* palet, int paletNum){
    // int mutilMatrix[4][2] ={
    //     {7,16},
    //     {1,16},
    //     {5,16},
    //     {3,16}
    // };
    int mutilMatrix[4][2] ={
        {1,8},
        {1,8},
        {1,8},
        {1,8}
    };

    // for (int i = 0; i < dataWitdh*dataHeight; i++){
    //     printf("(%d %d %d) ", data[i].r,data[i].g,data[i].b);
    // }
    // printf("\n\n");
    

    vec3* currentLine = malloc(sizeof(vec3)*dataWitdh); 
    vec3* nextLine = malloc(sizeof(vec3)*dataWitdh);

    copyifCanDataVec3(data, dataWitdh, dataHeight, 0, currentLine);
    copyifCanDataVec3(data, dataWitdh, dataHeight, 0+dataWitdh, nextLine);

    int currentPixel = 0;
    for (int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            // printf("(%d %d %d) ", currentLine[x].r,currentLine[x].g,currentLine[x].b);
            currentLine[x] = clampVec3(currentLine[x]);
            int32_t distance = vec3Dis(currentLine[x], palet[0]);
            color8_3 newColor = palet[0];
            for (int i = 1; i < paletNum; i++){
                int32_t newDis = vec3Dis(currentLine[x], palet[i]);
                
                if(distance > newDis){
                    distance = newDis;
                    newColor = palet[i];
                }
            }
            
            vec3 diff = {
                currentLine[x].d[0] - newColor.c[0],
                currentLine[x].d[1] - newColor.c[1],
                currentLine[x].d[2] - newColor.c[2],
            };
            // printf("(%d %d %d) ", diff.r,diff.g,diff.b);

            if(x>0){
                addVec3(nextLine+x-1, diff, 3, 16);
            }
            if(x+1<dataWitdh){
                addVec3(currentLine+x+1, diff, 7, 16);
                addVec3(nextLine+x+1, diff, 1, 16);
            }
            addVec3(nextLine+x, diff, 5, 16);

            data[currentPixel++] = newColor;
        }

        vec3* temp = currentLine;
        currentLine = nextLine;
        nextLine = temp;

        copyifCanDataVec3(data, dataWitdh, dataHeight, currentPixel+dataWitdh, nextLine);

        // for (int i = 0; i < dataWitdh; i++){
        //     printf("(%3d %3d %3d) ", currentLine[i].r,currentLine[i].g,currentLine[i].b);
        // }
        // printf("\n");
        // for (int i = 0; i < dataWitdh; i++){
        //     printf("(%d %d %d) ", nextLine[i].r,nextLine[i].g,nextLine[i].b);
        // }
        // printf("\n\n");
    }
    
    free(currentLine);
    free(nextLine);
}

static int copyifCanData(uint8_t* data, int dataWitdh, int dataHeight, int start, int32_t* out){
    if(start/dataWitdh >= dataHeight){
        return 0;
    }
    for (int i = 0; i < dataWitdh; i++){
        out[i] = data[i+start];
    }
    return 1;
}

void ditherApply1D(uint8_t* data, int dataWitdh, int dataHeight, uint8_t* palet, int paletNum){
    int32_t* currentLine = malloc(sizeof(int32_t)*dataWitdh); 
    int32_t* nextLine = malloc(sizeof(int32_t)*dataWitdh); 

    copyifCanData(data, dataWitdh, dataHeight, 0, currentLine);
    copyifCanData(data, dataWitdh, dataHeight, 0+dataWitdh, nextLine);

    int currentPixel = 0;
    for (int y = 0; y < dataHeight; y++){
        // for (int x = 0; x < dataWitdh; x++){
        //     printf("%d ",currentLine[x]);
        // }
        // printf("\n");
        for (int x = 0; x < dataWitdh; x++){

            if(currentLine[x] > 255){ currentLine[x] = 255; }
            else if(currentLine[x] < 0){ currentLine[x] = 0; }

            int bestOpion = 0;
            for (int o = 0; o < paletNum; o++){
                if(abs(currentLine[x] - palet[bestOpion]) > abs(currentLine[x] - palet[o])){
                    bestOpion = o;
                }
            }
            
            int32_t diff = currentLine[x] - palet[bestOpion];

            data[currentPixel++] = palet[bestOpion];

            if(x>0){
                nextLine[x-1] += 3*diff/16;
            }
            if(x+1<dataWitdh){
                currentLine[x+1] += 7*diff/16;
                nextLine[x+1] += 1*diff/16;
            }
            nextLine[x] += 5*diff/16;

        }
        // for (int x = 0; x < dataWitdh; x++){
        //     printf("%d ",currentLine[x]);
        // }
        // printf("\n\n");

        int32_t* temp = currentLine;
        currentLine = nextLine;
        nextLine = temp;

        copyifCanData(data, dataWitdh, dataHeight, currentPixel+dataWitdh, nextLine);
    }
    
    free(currentLine);
    free(nextLine);
}
