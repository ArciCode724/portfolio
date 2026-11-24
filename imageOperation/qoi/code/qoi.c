#include"../header/qoi.h"
#include<stdio.h>
#include<stdlib.h>

qoiFile* qoiFile_init(){
    qoiFile* retVal = malloc(sizeof(qoiFile));
    retVal->width = 0;
    retVal->height = 0;
    retVal->channels = 0;
    retVal->colorspace = 0;
    retVal->rawData = NULL;
    return retVal;
} 

void qoiFile_free(qoiFile* toFree){
    free(toFree->rawData);
    free(toFree);
}

static uint8_t colorHash(uint8_t* color, uint8_t changel){
    uint8_t a = (changel == 4) ? color[3] : 255;
    return (color[0] * 3 + color[1] * 5 + color[2] * 7 + a * 11) % 64;
}

// ---

static int8_t expant2bit(uint8_t a){
    a &= 3;
    return a-2;
}

static int8_t expant4bit(uint8_t a){
    a &= 15;
    return a-8;
}

static int8_t expant6bit(uint8_t a){
    a &= 63;
    return a-32;
}

static uint32_t readBigEndian32(FILE* in){
    uint8_t readed[4];
    fread(readed, 1, 4, in);
    return readed[0]<<24 | readed[1]<<16 | readed[2]<<8 | readed[3];
}

int qoi_decode(char* fileName, qoiFile* out){
    FILE* in = fopen(fileName, "rb");
    if(!in){ return 1; }

    char magic[4];
    fread(magic, 1, 4, in);
    char const properMagic[4] = {'q','o','i','f'};
    for (int i = 0; i < 4; i++){
        if( magic[i] != properMagic[i] ){ return 2; }
    }
    
    out->width = readBigEndian32(in);
    out->height = readBigEndian32(in);
    out->channels = fgetc(in);
    out->colorspace = fgetc(in);
    uint64_t rawDataNum = out->width*out->height*out->channels;
    out->rawData = malloc(rawDataNum);
    // for (int i = 0; i < rawDataNum; i++){
    //     out->rawData[i] = 0;
    // }
    
    // printf("%d %d %d %d\n", out->width, out->height, out->channels, out->colorspace);

    uint8_t colorHashMap[64][4];
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 4; j++){
            colorHashMap[i][j] = 0;
        }
    }
    
    uint64_t saveI = 0;
    uint8_t iInHash;
    uint8_t previusPixel[4] = {0,0,0,255};
    while (saveI < rawDataNum){
        uint8_t byte;
        byte = fgetc(in);
        // fread(&byte, 1, 1, in);

        // QOI_OP_RGBA
        if(byte == 255){
            for(int i = 0; i < 4; i++){
                byte = fgetc(in);
                // fread(&byte, 1, 1, in);
                out->rawData[saveI++] = byte;
            }
        }
        // QOI_OP_RGB
        else if(byte == 254){
            for(int i = 0; i < 3; i++){
                byte = fgetc(in);
                // fread(&byte, 1, 1, in);
                out->rawData[saveI++] = byte;
            }
            if(out->channels == 4){
                out->rawData[saveI++] = previusPixel[3];
            }
        }
        // QOI_OP_RUN
        else if( (byte&192) == 192){
            for (int i = (byte&63) + 1; i > 0; i--){
                for (int c = 0; c < out->channels; c++){
                    out->rawData[saveI++] = previusPixel[c];
                }
            }
        }
        // QOI_OP_INDEX
        else if( (byte&192) == 0){
            iInHash = byte&63;
            for (int c = 0; c < out->channels; c++){
                out->rawData[saveI++] = colorHashMap[iInHash][c];
            }
        }
        // QOI_OP_DIFF
        else if( (byte&192) == 64){
            for (int i = 0; i < 3; i++){
                out->rawData[saveI++] = previusPixel[i] + expant2bit( byte>>(4-2*i) );
            }
            if(out->channels == 4){
                out->rawData[saveI++] = previusPixel[3];
            }
        }
        // QOI_OP_LUMA
        else if( (byte&192) == 128){
            int8_t dg = expant6bit(byte);
            byte = fgetc(in);
            // fread(&byte, 1, 1, in);

            out->rawData[saveI++] = previusPixel[0] + expant4bit(byte>>4) + dg;
            out->rawData[saveI++] = previusPixel[1] + dg;
            out->rawData[saveI++] = previusPixel[2] + expant4bit(byte) + dg;

            if(out->channels == 4){
                out->rawData[saveI++] = previusPixel[3];
            }
        }
        else{
            return 3;
        }

        // save proces pixel
        for (int i = 0; i < out->channels; i++){
            previusPixel[i] = out->rawData[saveI - out->channels + i];
        }
        iInHash = colorHash(previusPixel, 4);
        for (int i = 0; i < 4; i++){
            colorHashMap[iInHash][i] = previusPixel[i];
        }
    }

    fclose(in);
    return 0;
}

// ---

// picel to fragment tablicy o dlugosi channels
static int isPixelsEqual(uint8_t* a, uint8_t* b, uint8_t channels){
    for(int i=0; i<channels; i++){
        if( a[i] != b[i] ){
            return 0;
        }
    }
    return 1;
}

static void writeBigEndian32(FILE* out, uint32_t numer){
    uint8_t outBytes[4] = {
        numer>>24,
        numer>>16,
        numer>>8,
        numer
    };
    fwrite(outBytes, 1, 4, out);
}

// zapisze pixel zakladaja ze A jest takie same jak poprednie
static void writeRGBpixle(uint8_t* curent, uint8_t* previus, FILE* out){
    int8_t rd = curent[0] - previus[0];
    int8_t gd = curent[1] - previus[1];
    int8_t bd = curent[2] - previus[2];
    // printf("%d %d %d\n", rd, gd, bd);
    
    // QOI_OP_DIFF
    if( -2 <= rd && rd <= 1 && -2 <= gd && gd <= 1 && -2 <= bd && bd <= 1){
        // printf("A");
        fputc(64 | (rd+2)<<4 | (gd+2)<<2 | (bd+2), out);
        return;
    }

    // QOI_OP_LUMA 
    rd-=gd;
    bd-=gd;
    if( -8 <= rd && rd <= 7 && -32 <= gd && gd <= 31 && -8 <= bd && bd <= 7 ){
        fputc( 128 | (gd+32), out);
        fputc( (rd+8)<<4 | (bd+8), out);
        return;
    }

    // QOI_OP_RGB
    fputc(254, out);
    for (int c = 0; c < 3; c++){
        fputc(curent[c], out);
    }
}

void qoi_encode(char* fileName, qoiFile* in){
    FILE* out = fopen(fileName, "wb");

    char magic[4] = {'q', 'o', 'i', 'f'};
    fwrite(magic, 1, 4, out);
    writeBigEndian32(out, in->width);
    writeBigEndian32(out, in->height);
    fputc(in->channels, out);
    fputc(in->colorspace, out);

    uint64_t rawDataNum = in->width*in->height*in->channels;

    uint8_t colorHashMap[64][4];
    for (int i = 0; i < 64; i++){
        for (int j = 0; j < 4; j++){
            colorHashMap[i][j] = 0;
        }
    }
    uint8_t iInHash;
    uint8_t previusPixel[4] = {0,0,0,255};

    for (int64_t i = 0; i < rawDataNum; i+=in->channels){
        // QOI_OP_RUN
        uint8_t runLen = 0;
        while(isPixelsEqual(previusPixel, in->rawData+i, in->channels)){
            runLen++;
            i+=in->channels;
            if(runLen == 62){
                break;
            }
            if(i >= rawDataNum){
                break;
            }
        }
        if(runLen){
            runLen--;
            fputc( 192 | runLen , out);
            i-=in->channels;
            continue;
        }

        // iInHash = colorHash(in->rawData + i, in->channels);
        // uint8_t aplay_index = isPixelsEqual(in->rawData + i, colorHashMap[iInHash], in->channels);

        // QOI_OP_INDEX
        if(isPixelsEqual(in->rawData + i, colorHashMap[iInHash], in->channels)){
            fputc( 0 | iInHash , out);
        }
        else if(in->channels == 3 || ( in->channels == 4 && previusPixel[3] == in->rawData[i+3])){
            writeRGBpixle(in->rawData + i, previusPixel, out);
        }
        // QOI_OP_RGBA
        else{
            fputc(255, out);
            for (int c = 0; c < 4; c++){
                fputc(in->rawData[i+c], out);
            }
        }

        // save proces pixel
        for (int c = 0; c < in->channels; c++){
            previusPixel[c] = in->rawData[i + c];
        }
        iInHash = colorHash(previusPixel, 4);
        for (int c = 0; c < 4; c++){
            colorHashMap[iInHash][c] = previusPixel[c];
        }
    }

    fclose(out);
}