#include<spng.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>

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

int main(int argc, char** argv){
    // sprawdzanie porawnosci
    if(argc < 2){
        fprintf(stderr, "brakuje pliku");
        return 1;
    }
    if(!properEnd(argv[1], ".png")){
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
    const int fmt = SPNG_FMT_RAW;
    spng_decode_image(ctxInput, 0, 0, fmt, SPNG_DECODE_PROGRESSIVE);

    struct spng_ihdr ihdr;
    spng_get_ihdr(ctxInput, &ihdr);
    if(ihdr.bit_depth != 8){
        fprintf(stderr, "nie wsperamy bit_depth %u\n", ihdr.bit_depth);
        return 1;
    }
    if(ihdr.color_type != 2 && ihdr.color_type != 6){
        fprintf(stderr, "nie wsperamy color_type %u\n", ihdr.bit_depth);
        return 1;
    }
    int pixel_size = (ihdr.color_type == 2) ? 3 : 4;
    // printf("%u\n", pixel_size);

    // inicjalizaja wyjcia
    const char* outNames[] = {"r.png", "g.png", "b.png"};
    const int changelNumber = 3;
    FILE* changelsFiles[changelNumber];
    spng_ctx* changelsCtx[changelNumber];
    for(int i = 0; i<changelNumber; i++){
        changelsFiles[i] = fopen(outNames[i], "wb");
        changelsCtx[i] = spng_ctx_new(SPNG_CTX_ENCODER);
        spng_set_png_file(changelsCtx[i], changelsFiles[i]);
        spng_set_ihdr(changelsCtx[i], &ihdr);
        int err = spng_encode_image(changelsCtx[i], 0, 0, fmt, SPNG_ENCODE_PROGRESSIVE);
        // printf("%d ", err);
    }

    // rozdzielenie kolorow na pliki
    size_t image_size;
    spng_decoded_image_size(ctxInput, fmt, &image_size);
    size_t image_width = image_size / ihdr.height;
    // printf("image_width %d\n", image_width);

    uint8_t* readRow = malloc(image_width);
    uint8_t* writeRow = malloc(image_width);
    for(int i = 0; i < image_width; i++){
        writeRow[i] = 0;
    }

    for(int rowIter = 0; rowIter < ihdr.height; rowIter++){
        spng_decode_row(ctxInput, readRow, image_width);
        // for(int i = 0; i<image_width; i++){
        //     printf("%3u ", readRow[i]);
        // }
        // printf("\n");
        if( ihdr.color_type & 4){
            for(int i = 3; i < image_width; i+=pixel_size){
                writeRow[i] = readRow[i];
            }
        }

        for(int chanel = 0; chanel < changelNumber; chanel++){
            // wydziel kolor
            for (int i = chanel; i < image_width; i+=pixel_size){
                writeRow[i] = readRow[i];
            }
            spng_encode_row(changelsCtx[chanel], writeRow, image_width);
            // wyszysz
            for (int i = chanel; i < image_width; i+=pixel_size){
                writeRow[i] = 0;
            }
        }
    }

    free(readRow);
    free(writeRow);

    // printf("%d",image_width);


    fclose(input);
    spng_ctx_free(ctxInput);
    for(int i = 0; i<changelNumber; i++){
        fclose(changelsFiles[i]);
        spng_ctx_free(changelsCtx[i]);
    }
    return 0;
}