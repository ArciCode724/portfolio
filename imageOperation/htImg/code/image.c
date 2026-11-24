#include"../head/image.h"

// outSize to dlugosc boku kwadratu out
void loadBlock(image img, uint32_t x, uint32_t y, color8_3* out, uint32_t outSize){
    // zadbanie by nie wyjść po za opszar
    uint32_t xMax = x + outSize;
    if(xMax > img.width){ xMax = img.width;}
    uint32_t yMax = y + outSize;
    if(yMax > img.height){ yMax = img.height;}

    // kopiowanie
    for(int iy = y; iy < yMax; iy++){
        for(int ix = x; ix < xMax; ix++){
            uint32_t outY = iy - y;
            uint32_t outX = ix - x;

            out[outSize*outY + outX] = img.pixels[img.width*iy + ix];
        }
    }

    // uzupelniane x
    uint32_t actulSizeX = xMax - x;
    for(int ix = actulSizeX; ix < outSize; ix++){
        for(int iy = 0; iy < outSize; iy++){
            out[outSize*iy + ix] = out[outSize*iy + ix - actulSizeX];
        }
    }

    // uzupelnianie y
    uint32_t actulSizeY = yMax - y;
    for(int iy = actulSizeY; iy < outSize; iy++){
        for(int ix = 0; ix < outSize; ix++){
            out[outSize*iy + ix] = out[outSize*(iy-actulSizeY) + ix];
        }
    }
}

void saveBlock(image img, uint32_t x, uint32_t y, color8_3* in, uint32_t inSize){
    // zadbanie by nie wyjść po za opszar
    uint32_t xMax = x + inSize;
    if(xMax > img.width){ xMax = img.width;}
    uint32_t yMax = y + inSize;
    if(yMax > img.height){ yMax = img.height;}

    // kopiowanie
    for(int iy = y; iy < yMax; iy++){
        for(int ix = x; ix < xMax; ix++){
            uint32_t outY = iy - y;
            uint32_t outX = ix - x;

            img.pixels[img.width*iy + ix] = in[inSize*outY + outX];
        }
    }
}
