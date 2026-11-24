#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"..\header\FFT.h"

int properFileExdend(char* text, char* endWith){
    return strcmp( strrchr(text, '.')+1, endWith ) == 0;
}

void saveTransformChangel(complex* data, int width, int height, char* name){
    uint8_t* saveData = malloc(width*height);
    // int counter[255];
    // for (int i = 0; i < 255; i++){
    //    counter[i] = 0;
    // }
    for(int i = 0; i < width*height; i++){
        double newVal = 2000*cabs(data[i]);
        // double newVal = 255*(carg(data[i]) + 3.14)/(2*3.14);
        // clamp
        if(newVal < 0.0){
            newVal = 0;
        }else if( newVal > 255.0){
            newVal = 0xff;
        }
        saveData[i] = (uint8_t)newVal;

    }
    // printf("%s\n", name);
    // for (int i = 0; i < 255; i++){
    //     printf("%3d: %d\n", i, counter[i]);
    // }
    // printf("\n");
    

    // for(int y = 0; y < height; y++){
    //     for(int x = 0; x < width; x++){
    //         printf("%3u ", saveData[y*width + x]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    int xLeftSize = (width+1)/2;
    int yLeftSize = (height+1)/2;
    int xRightSize = width/2;
    int yRightSize = height/2;
    // swap x
    uint8_t* xBuffer = malloc(xRightSize);
    for(int y = 0; y < height ; y++){
        int row = y*width;
        for(int x = 0; x < xRightSize; x++){
            xBuffer[x] = saveData[row + x + xLeftSize];
        }
        for(int x = xLeftSize-1; x >= 0 ; x--){
            saveData[row + x + xRightSize] = saveData[row + x];
        }
        for(int x = 0; x < xRightSize; x++){
            saveData[row + x] = xBuffer[x];
        }
    }
    free(xBuffer);

    uint8_t* yBuffer = malloc(yRightSize);
    for(int x = 0; x < width ; x++){
        for(int y = 0; y < yRightSize; y++){
            yBuffer[y] = saveData[ (y+yLeftSize)*width + x ];
        }
        for(int y = yLeftSize-1; y >= 0 ; y--){
            saveData[(y+yRightSize)*width + x] = saveData[y*width + x];
        }
        for(int y = 0; y < yRightSize; y++){
            saveData[y*width + x] = yBuffer[y];
        }
    }
    free(yBuffer);

    // for(int y = 0; y < height; y++){
    //     for(int x = 0; x < width; x++){
    //         printf("%3u ", saveData[y*width + x]);
    //     }
    //     printf("\n");
    // }

    struct spng_ihdr ihdr;
    ihdr.bit_depth = 8;
    ihdr.height = height;
    ihdr.width = width;
    ihdr.color_type = 0; // greyscale
    ihdr.compression_method = 0;
    ihdr.filter_method = 0;
    ihdr.interlace_method = 0;

    FILE* output = fopen(name, "wb");
    spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);
    spng_set_png_file(ctxOutput, output);
    spng_set_ihdr(ctxOutput, &ihdr);
    spng_encode_image(ctxOutput, saveData, width*height, SPNG_FMT_RAW, 0);

    spng_ctx_free(ctxOutput);
    fclose(output);
    free(saveData);
}

// !! uwaga zapisuje jako rgb8
int main(int argc, char** argv){
    // sprawdzanie porawnosci
    if(argc < 2){
        fprintf(stderr, "brakuje pliku\n");
        return 1;
    }
    if(!properFileExdend(argv[1], "png")){
        fprintf(stderr, "zly typ pliku\n");
        return 1;
    }
    FILE* input = fopen(argv[1], "rb");
    if(!input){
        fprintf(stderr, "plik nie istnieje\n");
        return 1;
    }
    double persentSave = 1.0;
    if(argc > 2){
        persentSave = atof(argv[2]);
    }

    // inicjalizacja informacji o inpucie
    spng_ctx* ctxInput = spng_ctx_new(0);
    spng_set_png_file(ctxInput, input);
    
    struct spng_ihdr ihdr;
    spng_get_ihdr(ctxInput, &ihdr);
    printf(
        "width:%u\nheight:%u\nbit_depth:%u\ncolor_type:%u\ncompression_method:%u\nfilter_method:%u\ninterlace_method:%u\n", 
        ihdr.width, ihdr.height, ihdr.bit_depth, ihdr.color_type, ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method
    );
    // return 0;
    int xLimit = (int)roundf( persentSave*ihdr.width/2.0 );
    int yLimit = (int)roundf( persentSave*ihdr.height/2.0 );
    printf("%d %d\n", xLimit, yLimit);

    size_t imigSize;
    spng_decoded_image_size(ctxInput, SPNG_FMT_RGB8, &imigSize);
    uint8_t* imigData = malloc(imigSize);
    spng_decode_image(ctxInput, imigData, imigSize, SPNG_FMT_RGB8, 0);
    spng_ctx_free(ctxInput);

    // for(int i = 0; i < imigSize; i++){
    //     printf("%3u ", imigData[i]);
    // }
    // printf("\n");

    char* changelFilesNames[3] = {"r.png", "g.png", "b.png"};
    // uint8_t* chanelData = malloc(imigSize/3);
    complex* cChanel = malloc(imigSize/3 *sizeof(complex));
    complex* postTransform = malloc(imigSize/3 *sizeof(complex));
    // c < 3 poniewasz mamy tylko 3 kanalu w SPNG_FMT_RGB8
    for(int c = 0; c < 3; c++){
        // break;
        for(int i = 0; i < imigSize/3; i++){
            // chanelData[i] = imigData[3*i + c];
            cChanel[i] = imigData[3*i + c];
            // printf("%3u ", imigData[3*i + c]);
        }
        // printf("\n");

        fft2d(cChanel, ihdr.width, ihdr.height, postTransform);

        // for(int i = 0; i < imigSize/3; i++){
        //     printf("%f %f; ", creal(postTransform[i]), cimag(postTransform[i]));
        // }
        // printf("\n");

        saveTransformChangel(postTransform, ihdr.width, ihdr.height, changelFilesNames[c]);

        // for(int i = 0; i < imigSize/3; i++){
        //     postTransform[i] = roundf(creal(postTransform[i])) + I*roundf(cimag(postTransform[i]));
        // }

        // for(int i = 0; i < imigSize/3; i++){
        //     printf("%f %f; ", creal(cChanel[i]), cimag(cChanel[i]));
        // }
        // printf("\n\n");
        // for(int y = 0; y < ihdr.height; y++){
        //     for (int x = 0; x < ihdr.width; x++){
        //         printf("%.0f ", cabs(postTransform[y*ihdr.width+x]));
        //     }
        //     printf("\n");
        // }
        // printf("\n");

        // usuwanie poza kwadratem
        for(int y = 0; y < ihdr.height; y++){
            int startPos = y*ihdr.width + 1 + xLimit;
            for (int x = (ihdr.width-1) - 2*xLimit; x > 0; x--){
                postTransform[startPos++] = 0;
            }
        }

        for(int x = 0; x < ihdr.width; x++){
            int startPos = x + (1 + yLimit)*ihdr.width;
            for (int y = (ihdr.height-1) - 2*yLimit; y > 0; y--){
                postTransform[startPos] = 0;
                startPos += ihdr.width;
            }
        }

        // for(int y = 0; y < ihdr.height; y++){
        //     for (int x = 0; x < ihdr.width; x++){
        //         printf("%.0f ", cabs(postTransform[y*ihdr.width+x]));
        //     }
        //     printf("\n");
        // }
        // printf("\n");


        ifft2d(postTransform, ihdr.width, ihdr.height, cChanel);

        for(int i = 0; i < imigSize/3; i++){
            // chanelData[i] = imigData[3*i + c];
            if( 0.0 > creal(cChanel[i]) ){
                imigData[3*i + c] = 0;
            }else if( 255.0 < creal(cChanel[i]) ){
                imigData[3*i + c] = 255;
            }else{
                imigData[3*i + c] =  (uint8_t)creal(cChanel[i]);
            }
        }
    }
    // for(int i = 0; i < imigSize; i++){
    //     printf("%3u ", imigData[i]);
    // }
    // printf("\n");

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

    free(imigData);
    free(cChanel);

    printf("end\n");
    return 0;
}