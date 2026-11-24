#include"../../header/paletApply.h"
// #include<stdlib.h>
// #include<stdint.h>
// #include<stdio.h>
#include<math.h>


void simpleApply1D(uint8_t* data, int dataNum, uint8_t* palet, int paletNum){
    for (int i = 0; i < dataNum; i++){
        int bestOpion = 0;
        for (int o = 0; o < paletNum; o++){
            if(abs(data[i] - palet[bestOpion]) > abs(data[i] - palet[o])){
                bestOpion = o;
            }
        }
        data[i] = palet[bestOpion];
    }    
}

void simpleApply(color8_3* data, int dataNum, color8_3* palet, int paletNum){
    for (int i = 0; i < dataNum; i++){
        data[i] = palet[ findClosect(palet, paletNum, data[i]) ];
    }    
}

void simpleApply5D(color8_3* data, int dataWitdh, int dataHeight, colorLF_5* palet, int paletNum){
    for(int y = 0; y < dataHeight; y++){
        for (int x = 0; x < dataWitdh; x++){
            colorLF_5 find = {
                0,0,0,
                (double)x / dataWitdh,
                (double)y / dataHeight
            };
            for (int i = 0; i < 3; i++){
                find.d[i] = data[y*dataWitdh+x].c[i]/255.0;
            }
            int chosenColorI = findClosect5D(palet, paletNum, find);
            for (int i = 0; i < 3; i++){
                data[y*dataWitdh+x].c[i] = palet[chosenColorI].d[i] * 255;
            }
        }
    }
}
