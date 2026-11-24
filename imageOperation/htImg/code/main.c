#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include"../head/image.h"
#include"../head/fht.h"

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

void removeSmalest(int* list, int listNum, int remove){
    for(int r = 0; r < remove; r++){
        int toRemove = 0;
        for(int i = 1; i < listNum; i++){
            if(list[i] == 0){ continue;}
            if(list[toRemove] > list[i]){
                toRemove = i;
            }
        }
        list[toRemove] = 0;
    }
}

void removeHighF(int* list, int* fOrder, int blockSize, int remove){
    // printf("%d\n", remove);
    for(int k = 0; k < remove; k++){
        int x = 2*(blockSize-1) - k;
        int y = 0;

        if(x >= blockSize){
            y += x - blockSize + 1;
            x -= x - blockSize + 1;
        }
        // while(x >= blockSize){ x--; y++; }
        // while( y < blockSize && x >= 0){
        //     list[ blockSize*fOrder[y] + fOrder[x] ] = 0;
        //     // list[ blockSize*fOrder[x] + fOrder[y] ] = 0;
        //     // printf("%d %d -> %d %d,", x,y, fOrder[x],fOrder[y]);
        //     x--;
        //     y++;
        // }

        while(x >= y){
            list[ blockSize*fOrder[y] + fOrder[x] ] = 0;
            list[ blockSize*fOrder[x] + fOrder[y] ] = 0;
            x--;
            y++;
        }
    }
}

void reduseBlock(color8_3* block, uint32_t blockLevel){
    int blockSize = 1<<blockLevel;
    int blockLen = blockSize*blockSize;
    int* oneColor = malloc(blockLen*sizeof(int));

    int fOrder[blockSize];
    walshOrder(fOrder, blockLevel);
    // for(int i = 0; i < blockSize; i++) printf("%d ", fOrder[i]);
    // printf("\n");

    for (int c = 0; c < 3; c++){
        for (int i = 0; i < blockLen; i++){
            oneColor[i] = block[i].c[c];
        }
        
        fht2d(oneColor, blockLevel);

        // removeHighF(oneColor, fOrder, blockSize, 16);
        for(int i = 0; i < blockLen; i++){
            int d = 80*(fOrder[i/blockSize]+fOrder[i%blockSize]) + 17;

            oneColor[i] += d/2 * ((oneColor[i] > 0) ? 1 : -1);
            oneColor[i] /= d;

            oneColor[i] *= d;
        }
        ifht2d(oneColor, blockLevel);
        
        for (int i = 0; i < blockLen; i++){
            if(oneColor[i] < 0){ oneColor[i] = 0;}
            if(oneColor[i] > 255){ oneColor[i] = 255;}
            block[i].c[c] = oneColor[i];
        }
    }

    free(oneColor);    
}

void reduseResolution(image img, uint32_t blockLevel){
    int blockSize = 1<<blockLevel;
    color8_3* workingBlock = malloc(blockSize*blockSize*sizeof(color8_3));

    for (int y = 0; y < img.height; y+=blockSize){
        for(int x = 0; x < img.width; x+=blockSize){
            loadBlock(img, x, y, workingBlock, blockSize);

            reduseBlock(workingBlock, blockLevel);

            saveBlock(img, x, y, workingBlock, blockSize);
        }
    }
    

    free(workingBlock);
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
    int quality = 256;
    if(argc > 2){
        quality = atoi(argv[2]);
    }
    if(quality <= 1){
        quality = 1;
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

    image img = {.pixels = imigData, .width = ihdr.width, .height = ihdr.height};
// ----
// do on imgage
    // color8_3 out[16];
    // loadBlock(img, 3, 0, out, 4);
    // for(int i = 0; i < 16; i++){
    //     if(i%4 == 0){ printf("\n");}
    //     printf("%3d ", out[i].c[0]);
    // }
    // out[0].c[0] = 0;
    // out[0].c[1] = 0;
    // saveBlock(img, 3, 0, out, 4);

    reduseResolution(img, 4);

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
    return 0;
}