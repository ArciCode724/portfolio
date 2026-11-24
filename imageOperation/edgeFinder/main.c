#include<spng.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#define swap(a, b) {typeof(a) t = a; a = b; b = t;}

typedef struct _color8_3{
    uint8_t c[3];
} color8_3;

typedef struct _image{
    color8_3* imgData;
    uint32_t width;
    uint32_t height;
} image;

color8_3 getPixelAt(image img, int x, int y){
    if(x < 0){ x = 0;}
    else if(x >= img.width){ x = img.width-1;}
    if(y < 0){ y = 0;}
    else if(y >= img.height){ y = img.height-1;}
    return img.imgData[y*img.width+x];
}

// matix ma rozmiar matixSize*matixSize
void applyMatrix(image img, int x, int y, int* matrix, int matrixSize, int out[3]){
    for (int i = 0; i < 3; i++){
        out[i] = 0;
    }
    
    int center = matrixSize/2;

    int mi = 0;
    int imgY = y - center;
    int imgX;
    for (int fy = 0; fy < matrixSize; fy++){
        imgX = x - center;
        for (int fx = 0; fx < matrixSize; fx++){
            color8_3 loaded = getPixelAt(img, imgX, imgY);
            int multi = matrix[mi++];
            for (int i = 0; i < 3; i++){
                out[i] += loaded.c[i] * multi;
            }
            imgX++;
        }
        imgY++;
    }
}

color8_3 sobelFilter(image img, int x, int y){
    int gx[9] = {
        -1,0,1,
        -2,0,2,
        -1,0,1
    };
    int gy[9] = {
        -1,-2,-1,
        0,0,0,
        1,2,1
    };

    int out[2][3];
    applyMatrix(img, x, y, gx, 3, out[0]);
    applyMatrix(img, x, y, gy, 3, out[1]);

    int sumAbs[2] = {0,0};

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 3; j++){
            sumAbs[i] += abs(out[i][j]);
        }
    }

    int len = 0;
    // (2*(255*3)^2)^0.5 = 255*3*2^0.5
    // 1/2^0.5 <<8 = 181
    // printf("%d ", sumAbs[0]);
    for (int i = 0; i < 2; i++){
        len += sumAbs[i]*sumAbs[i];
    }

    len = sqrt(len);
    // normalizacja
    len = 255 * len / 3060;
    // len /= 3*4;
    // len = (len*181)>>8;
    // printf("%d ", len);
    // len = (len > 40) ? 255 : 0;
    return (color8_3){len,len,len};
}

color8_3 gaussianFilter(image img, int x, int y){
    int matrix[25] = {
        1,4,6,4,1,
        4,16,24,16,4,
        6,24,36,24,6,
        4,16,24,16,4,
        1,4,6,4,1
    };
    int out[3];
    applyMatrix(img, x, y, matrix, 5, out);

    color8_3 retVal;
    for (int i = 0; i < 3; i++){
        retVal.c[i] = out[i]/256;
    }
    return retVal;
}

color8_3 test(image img, int x, int y){
    int matrix[4] = {
        1,0,0,0
    };
    int out[3];
    applyMatrix(img, x, y, matrix, 2, out);

    return (color8_3){out[0],out[1],out[2]};
}

color8_3 qwantColor(image img, int x, int y){
    color8_3 retVal = getPixelAt(img, x, y);
    int grupSize = 256/6;
    for (int i = 0; i < 3; i++){
        retVal.c[i] = retVal.c[i]/grupSize * grupSize + grupSize/2;
    }
    return retVal;
}

#define MAX(c,a,b) (c[a]>c[b])?a:b
#define MIN(c,a,b) (c[a]<c[b])?a:b

double hue(double t){
    while(t < -0.5){ t+=1;}
    while(t > 0.5){ t-=1;}
    double r = -fabs(6*t)+2;
    if(r > 1){r=1;}
    else if(r<0){r=0;}
    return r;
}

color8_3 hslChange(image img, int x, int y){
    color8_3 pixel = getPixelAt(img, x,y);

    double rgb[3];
    for (int i = 0; i < 3; i++){ rgb[i] = (double)pixel.c[i]/255;}
    double hsl[3];
    uint8_t max = MAX(rgb, 0, MAX(rgb, 1,2));
    uint8_t min = MIN(rgb, 0, MIN(rgb, 1,2));
    double c = rgb[max] - rgb[min];
    if(c!=0){
        if(max == 0){ hsl[0] = (rgb[1] - rgb[2])/c;}
        else if(max == 1){ hsl[0] = (rgb[2] - rgb[0])/c + 2;}
        else if(max == 2){ hsl[0] = (rgb[0] - rgb[1])/c + 4;}
        if( hsl[0] < 0){ hsl[0] += 6;}
        hsl[0] /= 6;
    }else{
        hsl[0] = 0;
    }
    hsl[2] = 0.3*rgb[0] + 0.6*rgb[1] + 0.1*rgb[2];
    // hsl[2] = rgb[max];
    hsl[1] = (hsl[2] != 0) ? c/hsl[2] : 0;
    // ---

    // printf("%f ", hsl[0]);
    // hsl[0] = 0.33;

    // ---
    c = hsl[2]*hsl[1];
    double hl = 0.3*hue(hsl[0]) + 0.6*hue(hsl[0] - (double)1/3) + 0.1*hue(hsl[0] - (double)2/3);
    for (int i = 0; i < 3; i++){
        pixel.c[i] = 255*(c*hue(hsl[0] - (double)i/3) + (hsl[2] - c*hl) );
    }

    // printf("\n");
    return pixel;
}

void apply_filter(image img, color8_3 (*filter)(image img, int x, int y)){
    int dataLen = img.height*img.width;
    color8_3* imigDataCopy = malloc(dataLen*sizeof(color8_3));
    for (int i = 0; i < dataLen; i++){
        imigDataCopy[i] = img.imgData[i];
    }
    swap(img.imgData, imigDataCopy);

    int i = 0;
    for (int y = 0; y < img.height; y++){
        for (int x = 0; x < img.width; x++){
            // img.imgData[i] = filter(sorce, x, y);
            imigDataCopy[i] = filter(img, x, y);
            i++;
        }
    }

    swap(img.imgData, imigDataCopy);
    free(imigDataCopy);
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


    // apply_filter((image){imigData, ihdr.width, ihdr.height}, gaussianFilter);
    // apply_filter((image){imigData, ihdr.width, ihdr.height}, sobelFilter);
    // apply_filter((image){imigData, ihdr.width, ihdr.height}, qwantColor);
    apply_filter((image){imigData, ihdr.width, ihdr.height}, hslChange);

    // int number[256];
    // for (int i = 0; i < 256; i++){
    //     number[i] = 0;
    // }
    // for (int i = 0; i < imigSize/3; i++){
    //     number[imigData[i].c[0]]++;
    // }
    // for (int i = 0; i < 256; i++){
    //     printf("%3d %d\n", i, number[i]);
    // }
    

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