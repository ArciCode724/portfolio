#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

// 255 255   0   0   0 255
//   0 255 255 255   0   0
//   0   0   0 255 255 255

int main(int argc, char** argv){

    struct spng_ihdr ihdr;
    ihdr.color_type = 2;
    ihdr.bit_depth = 8;
    ihdr.width = 255*6;
    ihdr.height = 128;
    ihdr.filter_method = 0;
    ihdr.interlace_method = 0;
    ihdr.compression_method = 0;
    int imigSize = 3*ihdr.width*ihdr.height;
    uint8_t* imigData = malloc(imigSize);

    
    int colorIn = 0;
    int wichCanel[6] = {1, 0,2, 1,0, 2};
    int changeBy[6]  = {1,-1,1,-1,1,-1};
    uint8_t color[3] = {255,0,0};
    for (int part = 0; part < 6; part++){
        for (int i = 0; i < 255; i++){
            // printf("%d %d %d %d\n", colorIn, color[0], color[1], color[2]);
            // printf("A");
            imigData[colorIn++] = color[0];
            imigData[colorIn++] = color[1];
            imigData[colorIn++] = color[2];

            color[ wichCanel[part] ] += changeBy[part];
        }
        
    }

    // printf("b");
    
    
    // copy y
    for (int y = 1; y < ihdr.height; y++){
        uint8_t* from = imigData + (y-1)*ihdr.width*3;
        uint8_t* to = imigData + y*ihdr.width*3;
        // printf("%d %d\n", (y-1)*ihdr.width, y*ihdr.width);
        for (int x = 0; x < 3*ihdr.width; x++){
            *(to++) = *(from++);
        }
    }
    // while (colorIn < imigSize){
    //     imigData[colorIn++] = 0;
    // }
    
    
    // for (int i = 0; i < imigSize; i++){
    //     printf("%d ", imigData[i]);
    // }
    

    int err;
    char* outName = "testImig.png";
    FILE* output = fopen(outName, "wb");
    spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);
    err = spng_set_ihdr(ctxOutput, &ihdr);
    // printf("%s\n", spng_strerror(err));
    err = spng_set_png_file(ctxOutput, output);
    // printf("%s\n", spng_strerror(err));
    err = spng_encode_image(ctxOutput, imigData, imigSize, SPNG_FMT_PNG, 0);
    // printf("%s\n", spng_strerror(err));
    fclose(output);
    spng_ctx_free(ctxOutput);
    printf("end\n");
    return 0;
}