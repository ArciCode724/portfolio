#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include"color.h"
#include"graf.h"

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

void printDigit(int* number, int num, int d){
    for (int i = 0; i < num; i++){
        int temp = number[i];
        for(int j=0; j<d; j++){ temp /= 10;}
        printf("%d ", temp%10);
    }
    
    printf("\n");
}

void drawHist(color8_3* img, struct spng_ihdr ihdr){
    int histSize = 32;
    char* graf;

    int steps[histSize+1];
    for(int i=0; i<histSize+1;i++){
        steps[i] = i*((256+histSize-1)/histSize);
        printf("%d ", steps[i]);
    }
    printf("\n");
    for (int c = 0; c < 3; c++){
        graf = makeHist(img, ihdr.height*ihdr.width, c, histSize);
        printf("%d\n", c);
        for (int y = histSize-1; y >= 0; y--){
            printf(" ");
            for (int x = 0; x < histSize; x++){
                printf("%c ", graf[histSize*y + x]);
            }
            printf("\n");
        }
        for (int i = 2; i >= 0; i--){ printDigit(steps, histSize+1, i);}

        printf("\n");
        free(graf);
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

    // ---

    drawHist(imigData, ihdr);

    // ---

    int err;
    // char* outName = "out.png";
    // FILE* output = fopen(outName, "wb");
    // spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);
    // ihdr.color_type = 2;
    // ihdr.bit_depth = 8;
    // err = spng_set_ihdr(ctxOutput, &ihdr);
    // err = spng_set_png_file(ctxOutput, output);
    // err = spng_encode_image(ctxOutput, imigData, imigSize, SPNG_FMT_PNG, 0);
    // fclose(output);
    // spng_ctx_free(ctxOutput);
    fclose(input);
    printf("end\n");
    return 0;
}