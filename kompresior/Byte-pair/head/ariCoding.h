#ifndef ARICODING_H
#define ARICODING_H
//Arithmetic coding

#include"tokens.h"
#include<stdio.h>

#define END_OF_CODE ((1<<sizeof(token)*8)-1)

// context dla in i out
typedef struct{
    FILE* out;
    uint32_t base;
    uint32_t range;

    uint16_t head;
    uint32_t cery;

    // prob by zdekodowac/zakodowad token
    uint16_t probs[1<<sizeof(token)*8];
} ariCtxEncode;

typedef struct{
    FILE* in;
    // uint32_t base;
    uint32_t val;
    uint32_t range;

    // prob by zdekodowac/zakodowad token
    uint16_t probs[1<<sizeof(token)*8];
} ariCtxDecode;

ariCtxEncode* ari_encodingContex(FILE* out);

ariCtxDecode* ari_decodingContex(FILE* in);

void ari_encodeToken(ariCtxEncode* ctx, token tok);

void ari_flushEncode(ariCtxEncode* ctx);

token ari_decodeToken(ariCtxDecode* ctx);

#endif