#include<spng.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include"pixelGrupGraf.h"

void grafPrintCon(pixelGrupGraf* graf){
    for (uint64_t i = 0; i < graf->hashNum; i++){
        if(graf->conHashMap[i].key[0] == NULL){continue;}
        printf("%3d:", i);
        printf("%3d ", graf->conHashMap[i].key[0]-graf->grups);
        printf("%3d\n", graf->conHashMap[i].key[1]-graf->grups);
    }
}

int compColor(color8_3 a, color8_3 b){
    for (int i = 0; i < 3; i++){
        if(a.c[i] != b.c[i]){ return 0;}
    }
    return 1;
}

void mergePixels(color8_3* data, uint32_t width, uint32_t height, uint64_t limit){
    pixelGrupGraf* graf = create_pixelGrupGraf(data, width, height);
    
    // grafPrintCon(graf);
    // printf("\n");

    // for (uint64_t i = 0; i < width*height; i++){
    //     printf("%d: %d\n", i, graf->grups[i].neighborsNum);
    // }
    

    while (1){
        // grafPrintCon(graf);
        // printf("\n");
        connection minCon = minDistance(graf);
        if(minCon.distance > limit){ break;}
        printf("%10lld\r",graf->heapNum);
        pixelGrupGraf_mergeConnection(graf, minCon);
        // break;
    }

    color8_3* unicOne = malloc(sizeof(color8_3)*width*height);
    uint64_t unicNum = 0;
    for (uint64_t i = 0; i < width*height; i++){
        if(graf->grups[i].pixelsNum == 0){ continue;}
        // printf("%d ",i);
        int exist = 0;
        for (uint64_t j = 0; j < unicNum; j++){
            if(compColor(unicOne[j], graf->grups[i].color)){ exist = 1; break;}
        }
        if(exist == 0){
            unicOne[unicNum++] = graf->grups[i].color;
        }
    }
    printf("\ncolor num:%d\n" ,unicNum);
    free(unicOne);
    
    pixelGrupGrafToimage(graf, data);
    
    free_pixelGrupGraf(graf);
}

int main(int argc, char** argv){
    int err;

    FILE* input = fopen(argv[1], "rb");

    spng_ctx* ctxInput = spng_ctx_new(0);
    err = spng_set_png_file(ctxInput, input);
    // printf("%s\n", spng_strerror(err));
    int fmt = SPNG_FMT_RGB8;
    // spng_decode_image(ctxInput, 0, 0, fmt, 0);

    struct spng_ihdr ihdr;
    err = spng_get_ihdr(ctxInput, &ihdr);
    // printf("%s\n", spng_strerror(err));

    size_t imigSize;
    spng_decoded_image_size(ctxInput, fmt, &imigSize);
    color8_3* imigData = malloc(imigSize);
    spng_decode_image(ctxInput, imigData, imigSize, fmt, 0);
    spng_ctx_free(ctxInput);

    uint64_t limit = (argc > 1) ? atoi(argv[2]) : 512;

    // printf("%d %d\n", ihdr.width, ihdr.height);
    mergePixels(imigData, ihdr.width, ihdr.height, limit);

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

    printf("end");
}