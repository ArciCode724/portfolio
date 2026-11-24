#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"./color.h"
#include"./main.h"
#include"./area.h"

int properFileExdend(char* text, char* endWith){
    return strcmp( strrchr(text, '.')+1, endWith ) == 0;
}

int properEnd(char* text, char* endWith){
    int lenText;
    for(lenText = 0; text[lenText]; lenText++);
    int lenEnd = 0;
    for(lenEnd = 0; endWith[lenEnd]; lenEnd++);
    while (lenEnd){
        if( text[--lenText] != endWith[--lenEnd] ){
            return 0;
        }
    }
    return 1;
}

void splitImg(img data, int areaTargetNum){
    // areaTargetNum = 2;
    area* areaList = malloc(sizeof(area)*areaTargetNum);
    areaList[0] = areaOfImg(data);
    int areaNum = 1;
    // areaList[0].height = 1;

    while(areaNum < areaTargetNum){
        int bigestVar = 0;
        for (int i = 1; i < areaNum; i++){
            if(areaList[bigestVar].variance < areaList[i].variance){
                bigestVar = i;
            }
        }
        area toSplit = areaList[bigestVar];
        areaList[bigestVar] = areaList[--areaNum];
        spitArea(data, toSplit, areaList+areaNum);
        areaNum += 2;
    }


    for (int i = 0; i < areaNum; i++){
        printArea(areaList[i]);
        avgColorInArea(data, areaList[i]);
    }
    // avgColorInArea(data, areaList[1]);

    free(areaList);
}

int main(int argc, char** argv){
    // sprawdzanie porawnosci
    if(argc < 2){
        fprintf(stderr, "brakuje pliku");
        return 1;
    }
    if(!properFileExdend(argv[1], "png")){
        fprintf(stderr, "zly typ pliku");
        return 1;
    }
    FILE* input = fopen(argv[1], "rb");
    if(!input){
        fprintf(stderr, "plik nie istnieje");
        return 1;
    }
    int grupsNum = 2;
    if(argc > 2){
        grupsNum = atoi(argv[2]);
    }

    // inicjalizacja informacji o inpucie
    spng_ctx* ctxInput = spng_ctx_new(0);
    spng_set_png_file(ctxInput, input);
    int fmt = SPNG_FMT_RGB8;
    spng_decode_image(ctxInput, 0, 0, fmt, 0);

    struct spng_ihdr ihdr;
    spng_get_ihdr(ctxInput, &ihdr);

    size_t imigSize;
    spng_decoded_image_size(ctxInput, fmt, &imigSize);
    // uint8_t* imigData = malloc(imigSize);
    color8_3* imigData = malloc(imigSize);
    spng_decode_image(ctxInput, imigData, imigSize, fmt, 0);
    spng_ctx_free(ctxInput);

    // ---
    img toChange = {.pixels = imigData, .height = ihdr.height, .width = ihdr.width};
    splitImg(toChange, grupsNum);

    // ---

    int err;
    char* outName = "out.png";
    FILE* output = fopen(outName, "wb");
    spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);
    ihdr.color_type = 2;
    ihdr.bit_depth = 8;
    err = spng_set_ihdr(ctxOutput, &ihdr);
    err = spng_set_png_file(ctxOutput, output);
    err = spng_encode_image(ctxOutput, imigData, imigSize, SPNG_FMT_PNG, 0);
    fclose(output);
    spng_ctx_free(ctxOutput);
    fclose(input);
    printf("end\n");
    printf("doubel %d", sizeof(double));
    return 0;
}