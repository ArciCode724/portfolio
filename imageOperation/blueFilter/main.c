#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<time.h>
#include<spng.h>

void print2d(uint32_t* weights, uint32_t wh){
    for (uint32_t h = 0; h < wh; h++){
        for(uint32_t w = 0; w < wh; w++){
            printf("%3d ", weights[wh*h + w]);
        }
        printf("\n");
    }
}

void findSmalest(uint32_t* pos, uint32_t size, uint32_t* weights){
    uint32_t saveAt = 1;
    for(uint32_t i = 1; i < size; i++){
        if(weights[pos[0]] < weights[pos[i]]) continue;
        if(weights[pos[0]] != weights[pos[i]]) saveAt = 0;

        uint32_t t = pos[i];
        pos[i] = pos[saveAt];
        pos[saveAt] = t;
        saveAt++;
    }

    uint32_t toSwap = rand()%saveAt;
    uint32_t t = pos[toSwap];
    pos[toSwap] = pos[0];
    pos[0] = t;
}

void sampleFar(uint32_t* pos, uint32_t size, uint32_t* weights){
    const uint32_t sampleNum = (size >= 10) ? 10 : size;

    for(uint32_t i = 0; i < sampleNum; i++){
        uint32_t toSwap = rand()%size;
        uint32_t t = pos[toSwap];
        pos[toSwap] = pos[i];
        pos[i] = t;
    }

    for(uint32_t i = 1; i < sampleNum; i++){
        if(weights[pos[0]] < weights[pos[i]]) continue;
        uint32_t t = pos[i];
        pos[i] = pos[0];
        pos[0] = t;
    }
}

void find(uint32_t* pos, uint32_t find){
    for(uint32_t i = 0; 1; i++){
        if(pos[i] != find) continue;

        uint32_t t = pos[i];
        pos[i] = pos[0];
        pos[0] = t;
        return;
    }

}

#define abs(x) ((x < 0) ? -x : x)

void updateWeights(uint32_t* weights, uint32_t wh, uint32_t pos){
    const int32_t range = (wh-1)/2;
    // printf("r %d\n", range);
    uint32_t centerX = pos % wh;
    uint32_t centerY = pos / wh;
    for(int32_t y = -range; y <= range; y++){
        for(int32_t x = -range; x <= range; x++){
            uint32_t curentPos = (wh + centerY + y)%wh*wh + (wh + centerX + x)%wh;  

            uint32_t newWeight;
            if(abs(y) > abs(x)) newWeight = abs(y);
            else                newWeight = abs(x);
            weights[curentPos] += range - newWeight + 1;

            // uint32_t newWeight = abs(y) + abs(x);
            // if( newWeight <= range){
            //     weights[curentPos] += range - newWeight + 1;
            // }

            // uint32_t newWeight = y*y + x*x;
            // if( newWeight <= range * range){
            //     weights[curentPos] += range * range - newWeight + 1;
            // }
        }
    }
}

void createBlueMatrix(uint32_t wh, uint32_t* out){
    const uint32_t size = wh*wh;
    for (uint32_t w = 0; w < size; w++) out[w] = 0;

    uint32_t weights[size];
    for (uint32_t w = 0; w < size; w++) weights[w] = 0;

    uint32_t pos[size];
    for (uint32_t w = 0; w < size; w++) pos[w] = w;
    uint32_t posLeft = size;

    // uint32_t startUp[] = {0, wh/2, wh/2*wh, wh/2*wh + wh/2};
    // for(uint32_t i = 0; i < 4; i++){
    //     find(pos, startUp[i]);
    //     out[ pos[0] ] = --posLeft;
    //     updateWeights(weights, wh, pos[0]);
    //     pos[0] = pos[posLeft];
    // }

    for(uint32_t i = 0; posLeft > 0; i++){
        // findSmalest(pos, posLeft, weights);
        sampleFar(pos, posLeft, weights);
        // printf("%d %d\n", pos[0] % wh, pos[0] / wh);
        out[ pos[0] ] = --posLeft;
        updateWeights(weights, wh, pos[0]);
        pos[0] = pos[posLeft];
        // print2d(weights, wh);
        // printf("\n");
    }
}

int main(){
    srand(time(NULL));
    uint32_t wh = 1<<4;
    uint32_t list[wh*wh];
    createBlueMatrix(wh, list);
    print2d(list, wh);

    uint8_t imigData[wh*wh];
    for(uint32_t i=0; i < wh*wh; i++) imigData[i] = list[i];

    int err;
    char* outName = "out.png";
    FILE* output = fopen(outName, "wb");
    spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);
    struct spng_ihdr ihdr = {
        .width = wh,
        .height = wh,
        .bit_depth = 8,
        .color_type = SPNG_COLOR_TYPE_GRAYSCALE,
        .compression_method = 0,
        .filter_method = SPNG_FILTER_NONE,
        .interlace_method = SPNG_INTERLACE_NONE
    };
    // ihdr.color_type = 2;
    // ihdr.bit_depth = 8;
    err = spng_set_ihdr(ctxOutput, &ihdr);
    err = spng_set_png_file(ctxOutput, output);
    err = spng_encode_image(ctxOutput, imigData, wh*wh, SPNG_FMT_PNG, 0);
    fclose(output);
    // spng_ctx_free(ctxOutput);
    // printf("end\n");
}