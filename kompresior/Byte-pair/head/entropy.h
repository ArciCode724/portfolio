#ifndef ENTROPY_H
#define ENTROPY_H

#include"tokens.h"

// entropy = log(n) - sum(x_i*log2(x_i))/n

typedef struct{
    uint32_t count[1<<(8*sizeof(token))];
    uint32_t n;
    double logSum; // sum(x_i*log2(x_i))
    double entropy;
} entropy_ctx;

double entropyOfText(token* text, int textSize);

void init_entropyCtx(entropy_ctx* ctx, token* text, int textSize);

double predictEntropy(entropy_ctx* ctx, pair newToken, uint32_t tokenNum);

void updateEntropyCtx(entropy_ctx* ctx, pair newToken, uint32_t tokenNum, token tokenId);

#endif