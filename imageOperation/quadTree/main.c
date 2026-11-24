#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>

typedef struct { uint8_t c[3]; } color8_3;

typedef struct{
    uint32_t x, y;
    uint32_t level; // 2^x
    color8_3 color;
} quad;

int properFileExdend(char* text, char* endWith){
    return strcmp( strrchr(text, '.')+1, endWith ) == 0;
}

// na prawo przerzuca wszystki quady co maja odpowiedny level
uint64_t seperateLevel(quad* list, uint64_t listSize, uint32_t level){
    uint64_t separator = 0;
    for(uint64_t i = 0; i < listSize; i++){
        if(list[i].level == level){ continue;}
        quad t = list[i];
        list[i] = list[separator];
        list[separator] = t;
        separator++;
    }
    return separator;
}

uint64_t* partentsIdOfQuad(quad* list, uint64_t listSize, uint32_t parentLevel, uint32_t width){
    uint64_t* retVal = malloc(sizeof(uint64_t)*listSize);
    uint64_t parentSize = 1<<parentLevel;

    for(int i = 0; i < listSize; i++){
        uint64_t paretnX = list[i].x/parentSize;
        uint64_t paretnY = list[i].y/parentSize;
        retVal[i] = paretnY*width + paretnX*parentSize;
    }

    return retVal;
}

void sortByPartenId(quad* list, uint64_t* partenIdOfQuad, uint64_t listSize){
    for(uint64_t jump = listSize/2; jump > 0; jump /= 2){
        for(uint64_t i = jump; i < listSize; i++){
            uint64_t jj = i - jump;
            for(uint64_t j = i; j >= jump && partenIdOfQuad[j] < partenIdOfQuad[jj] ; j -= jump){
                quad t = list[j];
                list[j] = list[jj];
                list[jj] = t;

                uint64_t tt = partenIdOfQuad[j];
                partenIdOfQuad[j] = partenIdOfQuad[jj];
                partenIdOfQuad[jj] = tt;

                jj -= jump;
            }
        }
    }
}

uint64_t evluateCombination(quad children[4]){
    uint64_t sum[3] = {0,0,0};
    for (int i = 0; i < 4; i++){
        for (int c = 0; c < 3; c++){
            sum[c] += children[i].color.c[c];
        }
    }
    for(int c = 0; c < 3; c++){
        sum[c] = (sum[c]+2)/4;
    }

    uint64_t retVal = 0;
    for (int i = 0; i < 4; i++){
        uint64_t distance = 0;
        for (int c = 0; c < 3; c++){
            int64_t x = children[i].color.c[c] - sum[c];
            distance += x*x;
        }
        // if(retVal < distance){ retVal = distance;}
        retVal += distance;
    }

    return retVal/4;
}

// niesprawdza czy faktyczne dzieci sa poprawne
quad combineQuads(quad children[4]){
    quad retVal;
    retVal.level = children[0].level + 1;
    uint64_t parentSize = 1<<retVal.level;
    retVal.x = children[0].x/parentSize * parentSize;
    retVal.y = children[0].y/parentSize * parentSize;
    uint32_t sums[3] = {0,0,0};
    for (int i = 0; i < 4; i++){
        for (int c = 0; c < 3; c++){
            sums[c] += children[i].color.c[c];
        }
    }
    for (int c = 0; c < 3; c++){
        retVal.color.c[c] = (sums[c]+2)/4;
    }

    return retVal;
}

void aplayQuad(color8_3* img, uint32_t w, quad q){
    uint64_t quadSize = 1<<q.level;
    for(uint64_t y = 0; y < quadSize; y++){
        for(uint64_t x = 0; x < quadSize; x++){
            uint64_t i = (q.y+y)*w + (q.x+x);
            img[i] = q.color;
        }
    }

    // color8_3 frame = {255,255,255};
    // for(uint64_t i = 0; i < quadSize; i++){
    //     // x min
    //     img[(q.y+i)*w + q.x] = frame;
    //     // x max
    //     img[(q.y+i)*w + q.x + quadSize-1] = frame;
    //     // y min
    //     img[q.y*w + q.x + i] = frame;
    //     // y max
    //     img[(q.y + quadSize-1)*w + q.x + i] = frame;
    // }
}

void compresColor(color8_3* img, uint32_t h, uint32_t w){
    uint64_t treeSize = h*w;
    quad* tree = malloc(sizeof(quad)*treeSize);
    for (uint32_t y = 0; y < h; y++){
        for (uint32_t x = 0; x < w; x++){
            uint64_t i = y*w + x;
            tree[i].x = x;
            tree[i].y = y;
            tree[i].level = 0;
            tree[i].color = img[i];
        }
    }
    
    uint64_t workingListSize = treeSize;
    quad* workingList = tree;
    // level < 4
    for(uint64_t level = 0; workingListSize > 0; level++){
        // for(uint64_t i = 0; i < treeSize; i++){
        //     printf("x:%3d y:%3d l:%d\n", tree[i].x, tree[i].y, tree[i].level);
        // }
        // printf("\n");

        uint64_t split = seperateLevel(workingList, workingListSize, level);
        workingList += split;
        workingListSize -= split;

        uint64_t* partenIds = partentsIdOfQuad(workingList, workingListSize, level+1, w);
        sortByPartenId(workingList, partenIds, workingListSize);

        // for(uint64_t i = 0; i < treeSize; i++){
        //     printf("x:%3d y:%3d l:%d p:%3d\n", tree[i].x, tree[i].y, tree[i].level, partenIds[i]);
        // }
        // printf("\n");

        uint64_t saveAt = 0;
        for(uint64_t i = 0; i < workingListSize;){
            // printf("%d ", i);
            if(i+3 < workingListSize && partenIds[i] == partenIds[i+3] && evluateCombination(workingList+i) < 2000){
                // printf("%llu\n", parentVariance(workingList+i));
                workingList[saveAt++] = combineQuads(workingList+i);

                treeSize -= 3;
                i += 4;
            }else{
                workingList[saveAt++] = workingList[i];
                i++;
            }
        }
        workingListSize = saveAt;
        // for(uint64_t i = 0; i < treeSize; i++){
        //     printf("x:%3d y:%3d l:%d\n", tree[i].x, tree[i].y, tree[i].level);
        // }
        // printf("\n");

        free(partenIds);
    }

    for(uint64_t i = 0; i < treeSize; i++){
        // printf("x:%3d y:%3d l:%d\n", tree[i].x, tree[i].y, tree[i].level);
        aplayQuad(img, w, tree[i]);
    }

    free(tree);
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

    compresColor(imigData, ihdr.height, ihdr.width);

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