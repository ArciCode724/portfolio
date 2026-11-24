#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"../header/kwant.h"
#include"../header/paletApply.h"
#include"../header/kernel.h"

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

void doOnLumenChanal(color8_3* imigData, struct spng_ihdr ihdr, int paletSize){
    int dataNum = ihdr.width*ihdr.height;
    uint8_t* greyChanel = malloc(dataNum);
    for (int i = 0; i < dataNum; i++){
        // luminece
        // 0.2126 R + 0.7152 G + 0.0722 B
        greyChanel[i] = 0.2126*imigData[i].c[0]
                      + 0.7152*imigData[i].c[1]
                      + 0.0722*imigData[i].c[2];
        // greyChanel[i] = imigData[i].c[0];
    }

    // uint8_t palet[5] = {0, 255, 128, 64, 192};
    // paletSize = 3;
    uint8_t palet[paletSize];
    
    hierarchicalKwant1D(greyChanel, dataNum, palet, &paletSize);
    
    // printf("palet: ");
    // for (int i = 0; i < paletSize; i++){
    //     printf("%d ", palet[i]);
    // }
    // printf("\n");
    simpleApply1D(greyChanel, dataNum, palet, paletSize);
    // ditherApply1D(greyChanel, ihdr.width, ihdr.height, palet, paletSize);
    // orderApply1D(greyChanel, ihdr.width, ihdr.height, palet, paletSize);
    
    for (int i = 0; i < dataNum; i++){
        for (int j = 0; j < 3; j++){
            // printf("%d %d",  imigData[i].c[j] , greyChanel[i]);
            imigData[i].c[j] = greyChanel[i];
        }
    }

    
    free(greyChanel);
}

void doOn3ChanalSepred(color8_3* imigData, struct spng_ihdr ihdr, int paletSize){
    uint64_t dataSize = ihdr.width*ihdr.height;
    uint8_t* chanel = malloc(sizeof(uint8_t)*dataSize);

    // paletSize = 4;
    // uint8_t platests[3][4] = {
    //     {100, 255, 56, 0},
    //     {128,72, 255, 0},
    //     {0,156, 56,255}
    // };

    for (int c = 0; c < 3; c++){
        // copy
        for (uint64_t i = 0; i < dataSize; i++){
            chanel[i] = imigData[i].c[c];
        }

        paletSize = 2;
        uint8_t palet[2] = {0,255};
        // paletSize = 4;
        // uint8_t palet[4] = {0,85,170,255};
        // uint8_t* palet = platests[c];
        
        // uint8_t palet[paletSize];
        // hierarchicalKwant1D(chanel, dataSize, palet, &paletSize);

        // simpleApply1D(chanel, dataSize, palet, paletSize);
        orderApply1D(chanel, ihdr.width, ihdr.height, palet, paletSize);


        // save
        for (uint64_t i = 0; i < dataSize; i++){
            imigData[i].c[c] = chanel[i];
        }
    }
}

void doOn3Chanal(color8_3* imigData, struct spng_ihdr ihdr, int paletSize){
    color8_3 palet[paletSize];
    kmeanKwant(imigData, ihdr.width*ihdr.height, palet, &paletSize);
    // mediumKwant3D(imigData, ihdr.width*ihdr.height, palet, &paletSize);
    // hierarchicalKwant3D(imigData, ihdr.width*ihdr.height, palet, &paletSize);

    // paletSize = 2;
    // color8_3 palet[2] = {
    //     {0,0,0},{255,255,255}
    // };

    simpleApply(imigData, ihdr.width*ihdr.height, palet, paletSize);
    // ditherApply(imigData, ihdr.width, ihdr.height,  palet, paletSize);
    // orderApply3D(imigData, ihdr.width, ihdr.height,  palet, paletSize);
}

void doOn5Chanal(color8_3* imigData, struct spng_ihdr ihdr, int paletSize){
    colorLF_5 palet[paletSize];
    kmeanKwant5D(imigData, ihdr.width, ihdr.height, palet, &paletSize);
    for (int i = 0; i < paletSize; i++){
        // printf(
        //     "%d %d %d %d %d\n",
        //     palet[i].d[0]*255,
        //     palet[i].d[1]*255,
        //     palet[i].d[2]*255,
        //     palet[i].d[3]*ihdr.width,
        //     palet[i].d[4]*ihdr.height
        // );
        printf(
            "%f %f %f %f %f\n",
            palet[i].d[0],
            palet[i].d[1],
            palet[i].d[2],
            palet[i].d[3],
            palet[i].d[4]
        );
    }
    
    simpleApply5D(imigData, ihdr.width, ihdr.height, palet, paletSize);
}

void doBlock(color8_3* imigData, struct spng_ihdr ihdr, int paletSize){
    int blockSize = 32;
    struct spng_ihdr ihdrForPart = ihdr;
    ihdrForPart.height = blockSize; ihdrForPart.width = blockSize;
    printf("size %d\n", ihdr.width*ihdr.height);
    for (int y = 0; y < ihdr.height/blockSize; y++){
        for (int x = 0; x < ihdr.width/blockSize; x++){
            printf("%2d/%2d %2d/%2d\r", y,ihdr.height/blockSize, x,ihdr.width/blockSize);
            color8_3 part[blockSize*blockSize];

            for(int yy = 0; yy < blockSize; yy++){
                for (int xx = 0; xx < blockSize; xx++){
                    // printf("%d\n", ihdr.width*yy*blockSize + xx*blockSize + ihdr.width*y + x);
                    part[blockSize*yy+xx] = imigData[ihdr.width*y*blockSize + x*blockSize + ihdr.width*yy + xx];
                }
            }
            // f(x,y) = ihdr.width*y*blockSize + x*blockSize

            doOn3Chanal(part, ihdrForPart, paletSize);

            for(int yy = 0; yy < blockSize; yy++){
                for (int xx = 0; xx < blockSize; xx++){
                    // part[blockSize*yy+xx] = imigData[ihdr.width*(yy+y*blockSize) + (xx+x*blockSize)];
                    imigData[ihdr.width*y*blockSize + x*blockSize + ihdr.width*yy + xx] = part[blockSize*yy+xx];
                }
            }
        }
    }
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
    int paletSize = 256;
    if(argc > 2){
        paletSize = atoi(argv[2]);
    }
    if(paletSize <= 1){
        paletSize = 2;
    }

    // inicjalizacja informacji o inpucie
    spng_ctx* ctxInput = spng_ctx_new(0);
    spng_set_png_file(ctxInput, input);
    int fmt = SPNG_FMT_RGB8;
    spng_decode_image(ctxInput, 0, 0, fmt, 0);

    struct spng_ihdr ihdr;
    spng_get_ihdr(ctxInput, &ihdr);

    // printf("c");

    size_t imigSize;
    spng_decoded_image_size(ctxInput, fmt, &imigSize);
    // uint8_t* imigData = malloc(imigSize);
    color8_3* imigData = malloc(imigSize);
    spng_decode_image(ctxInput, imigData, imigSize, fmt, 0);
    spng_ctx_free(ctxInput);

    // sharpenImg(imigData, ihdr.width, ihdr.height);
    // doOnLumenChanal(imigData, ihdr, paletSize);
    // doOn3ChanalSepred(imigData, ihdr, paletSize);
    // doOn3Chanal(imigData, ihdr, paletSize);
    // kwantLight(imigData, ihdr.width*ihdr.height, paletSize);
    // doOn5Chanal(imigData, ihdr, paletSize);
    doBlock(imigData, ihdr, paletSize);
    // meanShift(imigData, ihdr.width*ihdr.height, paletSize);

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
    return 0;
}