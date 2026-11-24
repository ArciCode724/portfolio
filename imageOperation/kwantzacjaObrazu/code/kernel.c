#include"../header/kernel.h"
#include<stdlib.h>

typedef struct{int16_t c[3];} vec3;

static color8_3 vec3ToColor8_3(vec3 vec){
    color8_3 retVal;
    for (int i = 0; i < 3; i++){
        if(vec.c[i] < 0){ retVal.c[i] = 0; }
        else if(vec.c[i] > 255){ retVal.c[i] = 255; }
        else { retVal.c[i] = vec.c[i]; }
    }
    return retVal;
}

static vec3 addColor8_3ToVec3(color8_3 color, vec3 vec, int muli){
    for (int i = 0; i < 3; i++){
        vec.c[i] += muli*color.c[i];
    }
    return vec;
}

void sharpenImg(color8_3* data, int dataWitdh, int dataHeight){
    vec3* previusLine = malloc(sizeof(vec3)*dataWitdh);
    vec3* curentLine = malloc(sizeof(vec3)*dataWitdh);
    
    for (int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            for (int i = 0; i < 3; i++){
                curentLine[x].c[i] = 0;
            }

            curentLine[x] = addColor8_3ToVec3(data[y*dataWitdh+x],curentLine[x],5);
            if(y > 0){
                curentLine[x] = addColor8_3ToVec3(data[(y-1)*dataWitdh+x],curentLine[x], -1);
            }
            if(y+1 < dataHeight){
                curentLine[x] = addColor8_3ToVec3(data[(y+1)*dataWitdh+x],curentLine[x], -1);
            }
            if(x>0){
                curentLine[x] = addColor8_3ToVec3(data[y*dataWitdh+x-1],curentLine[x], -1);
            }
            if(x+1<dataWitdh){
                curentLine[x] = addColor8_3ToVec3(data[y*dataWitdh+x+1],curentLine[x], -1);
            }
        }

        if(y>0){
            for (int x = 0; x < dataWitdh; x++){
                data[(y-1)*dataWitdh+x] = vec3ToColor8_3( previusLine[x] );
            }
        }


        vec3* temp = previusLine;
        previusLine = curentLine;
        curentLine = temp;
    }
    for (int x = 0; x < dataWitdh; x++){
        data[(dataHeight-1)*dataWitdh+x] = vec3ToColor8_3( previusLine[x] );
    }

    free(previusLine);
    free(curentLine);
}