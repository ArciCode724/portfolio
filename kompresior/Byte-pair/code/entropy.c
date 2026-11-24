#include<math.h>
#include<stdlib.h>
#include"../head/entropy.h"
#include"stdio.h"

// -sum(p_i*log2(p_i))
// -sum(x_i/n*log2(x_i/n))
// -sum(x_i/n*(log2(x_i) - log(n)))
// 1/n*sum( x_i*(log(n)-log2(x_i)) )
// 1/n*( log(n)*sum(x_i) - sum(x_i*log2(x_i)) )
// 1/n*( log(n)*n - sum(x_i*log2(x_i)) )
// log(n) - sum(x_i*log2(x_i))/n

static const int tokenCombination = 1<<(8*sizeof(token)); // 2^(8*sizeof(token)

static void printCount(int *counter){
    for(int i = 0; i < tokenCombination; i++){
        if(counter[i] == 0) continue;
        printf("%3d: %d\n", i, counter[i]);
    }
}

double entropyOfText(token* text, int textSize){
    int* counter = calloc(tokenCombination, sizeof(int));
    // printf("%p\n", counter);

    for(int i = 0; i < textSize; i++){
        counter[text[i]]++;
    }
    // printCount(counter);

    double retVal = 0;
    for(int i = 0; i < tokenCombination; i++){
        if(counter[i] == 0) continue;
        retVal += counter[i]*log2(counter[i]);
    }
    retVal = log2(textSize) - retVal/textSize;
    free(counter);
    return retVal;
}

void init_entropyCtx(entropy_ctx* ctx, token* text, int textSize){
    ctx->n = textSize;
    for(int i = 0; i < tokenCombination; i++) ctx->count[i] = 0;
    for(int i = 0; i < textSize; i++) ctx->count[text[i]]++;

    double entropy = 0;
    for(int i = 0; i < tokenCombination; i++){
        uint32_t c = ctx->count[i];
        if(c == 0) continue;
        // printf("%d %f\n", i, c*log2(c));
        entropy += c*log2(c);
    }
    ctx->logSum = entropy;
    ctx->entropy = log2(textSize) - entropy/textSize;
}

double predictEntropy(entropy_ctx* ctx, pair newToken, uint32_t tokenNum){
    uint32_t xl = ctx->count[newToken.l];
    uint32_t xr = ctx->count[newToken.r];

    double entropy = ctx->logSum;
    if(newToken.l == newToken.r){
        entropy -= xl*log2(xl);
        xl -= 2*tokenNum;
        if(xl != 0) entropy += xl*log2(xl);
    }else{
        entropy -= xl*log2(xl);
        entropy -= xr*log2(xr);
    
        xl -= tokenNum;
        xr -= tokenNum;
        if(xl != 0) entropy += xl*log2(xl);
        if(xr != 0) entropy += xr*log2(xr);
    }
    entropy += tokenNum*log2(tokenNum);
    
    uint32_t newN = ctx->n - tokenNum;
    return log2(newN) - entropy/newN;
}

void updateEntropyCtx(entropy_ctx* ctx, pair newToken, uint32_t tokenNum, token tokenId){
    // printf("%d %d\n", newToken.l, newToken.r);
    uint32_t xl = ctx->count[newToken.l];
    uint32_t xr = ctx->count[newToken.r];

    double entropy = ctx->logSum;
    if(newToken.l == newToken.r){
        entropy -= xl*log2(xl);
        xl -= 2*tokenNum;
        if(xl != 0) entropy += xl*log2(xl);
    }else{
        entropy -= xl*log2(xl);
        entropy -= xr*log2(xr);
    
        xl -= tokenNum;
        xr -= tokenNum;
        if(xl != 0) entropy += xl*log2(xl);
        if(xr != 0) entropy += xr*log2(xr);
    }
    entropy += tokenNum*log2(tokenNum);

    ctx->logSum = entropy;
    uint32_t newN = ctx->n - tokenNum;
    ctx->n = newN;
    ctx->entropy = log2(newN) - entropy/newN;

    ctx->count[newToken.l] -= tokenNum;
    ctx->count[newToken.r] -= tokenNum;
    ctx->count[tokenId] = tokenNum;
    // printCount(ctx->count);
}