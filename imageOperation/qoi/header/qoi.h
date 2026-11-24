#ifndef QOI_H
#define QOI_H

#include<stdint.h>

typedef struct _qoiFile{
    // image width in pixels (BE)
    uint32_t width;
    // image height in pixels (BE)
    uint32_t height;
    // 3 = RGB, 4 = RGBA
    uint8_t channels;
    // 0 = sRGB with linear alpha, 1 = all channels linear
    uint8_t colorspace;
    uint8_t* rawData;
} qoiFile;

qoiFile* qoiFile_init(); 

void qoiFile_free(qoiFile* toFree);

// 0 - no error
// 1 - plik nie istnieje
// 2 - magic jest zly
// 3 - nieznany blok
int qoi_decode(char* fileName, qoiFile* out);

void qoi_encode(char* fileName, qoiFile* in);

#endif