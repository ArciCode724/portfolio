#include<spng.h>
#include"../header/spngio.h"

void spng_decode(char* fileName, qoiFile* out){
    spng_ctx* ctxInput = spng_ctx_new(0);
    FILE* in = fopen(fileName, "rb");
    spng_set_png_file(ctxInput, in);

    struct spng_ihdr ihdr;
    spng_get_ihdr(ctxInput, &ihdr);
    out->width = ihdr.width;
    out->height = ihdr.height;
    out->channels = (ihdr.color_type&4) ? 4 : 3;

    int fmt = (ihdr.color_type&4) ? SPNG_FMT_RGBA8 : SPNG_FMT_RGB8;

    size_t imigSize;
    spng_decoded_image_size(ctxInput, fmt, &imigSize);

    free(out->rawData);
    out->rawData = malloc(imigSize);
    spng_decode_image(ctxInput, out->rawData, imigSize, fmt, 0);

    spng_ctx_free(ctxInput);
    fclose(in);
}

void spng_encode(char* fileName, qoiFile* in){
    int err;
    char* outName = "out.png";
    FILE* output = fopen(outName, "wb");
    spng_ctx* ctxOutput = spng_ctx_new(SPNG_CTX_ENCODER);

    struct spng_ihdr ihdr;
    ihdr.width = in->width;
    ihdr.height = in->height;
    ihdr.color_type = (in->channels == 3) ? 2 : 6;
    ihdr.bit_depth = 8;
    ihdr.filter_method = 0;
    ihdr.interlace_method = 0;
    ihdr.compression_method = 0;
    err = spng_set_ihdr(ctxOutput, &ihdr);
    
    err = spng_set_png_file(ctxOutput, output);
    uint64_t imigSize = in->height*in->width*in->channels;
    err = spng_encode_image(ctxOutput, in->rawData, imigSize, SPNG_FMT_PNG, 0);
    fclose(output);
    spng_ctx_free(ctxOutput);
}