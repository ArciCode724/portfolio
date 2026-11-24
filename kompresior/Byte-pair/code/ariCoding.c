#include"../head/ariCoding.h"
#include<stdlib.h>

#define TOKENBITSIZE sizeof(token)*8

#define probBit 11
#define probRate 5

static inline void initProp(uint16_t probs[sizeof(token)*8]){
    for(uint32_t i = 0; i< 1<<TOKENBITSIZE; i++) probs[i] = 1<<(probBit-1);
}

ariCtxEncode* ari_encodingContex(FILE* out){
    ariCtxEncode* retVal = malloc(sizeof(ariCtxEncode));
    retVal->out = out;
    retVal->base = 0;
    retVal->range = _UI32_MAX;
    retVal->head = 0;
    retVal->cery = 0;
    initProp(retVal->probs);
    return retVal;
}

ariCtxDecode* ari_decodingContex(FILE* in){
    ariCtxDecode* retVal = malloc(sizeof(ariCtxDecode));
    retVal->in = in;
    // retVal->base = 0;
    getc(in);
    retVal->val = 0;
    for(int i = 0; i < 4; i++) retVal->val = retVal->val<<8 | getc(in);
    retVal->range = _UI32_MAX;
    initProp(retVal->probs);
    return retVal;
}

static void normalizeEncode(ariCtxEncode* ctx){
    if(ctx->range > 0x00ffffffu) return;
    uint8_t newHead = ctx->base >> (sizeof(uint32_t) - 1)*8;

    ctx->range <<= 8;
    ctx->base  <<= 8;

    if(newHead == 0xff){
        ctx->cery++;
    }else{
        putc(ctx->head, ctx->out);
        for(uint32_t i = 0; i < ctx->cery; i++) putc(0xff, ctx->out);

        ctx->cery = 0;
        ctx->head = newHead;
    }
} 

static void increasBase(ariCtxEncode* ctx, uint32_t baseChange){
    // printf("%08x < %08x\n", ctx->base, _UI32_MAX - baseChange);
    if(ctx->base > _UI32_MAX - baseChange){
        if(ctx->cery == 0){
            // printf("a\n");
            ctx->head += 1;
        }else{
            putc(ctx->head + 1, ctx->out);
            ctx->head = 0;
            for(uint32_t i = 1; i < ctx->cery; i++) putc(0, ctx->out);
            ctx->cery = 0;
        }
        // ctx->base -= _UI32_MAX - baseChange + 1;
        ctx->base += baseChange;
    }else{
        ctx->base += baseChange;
    }
}

void ari_encodeToken(ariCtxEncode* ctx, token tok){
    token state = 1;
    // printf("   %08x %08x\n", ctx->base, ctx->range);
    for(int i = 0; i < TOKENBITSIZE; i++){
        // printf("%x\n", i, state);
        uint8_t bit = tok>>TOKENBITSIZE-1;
        tok<<=1;
        // printf("%d\n", bit);
        
        uint16_t ctxProb = ctx->probs[state];
        uint32_t bound = (ctx->range>>probBit)*ctxProb;
        // printf("%06x * %x -> %06x\n", ctx->range, ctxProb, bound);
        // printf("%08d %08x\n", ctxProb, bound);
        if(bit == 0){
            ctx->range = bound;

            ctxProb += (1<<probBit) - ctxProb >> probRate;
        }else{ // bit == 1
            ctx->range -= bound;
            // ctx->base += bound;
            increasBase(ctx, bound);

            ctxProb -= ctxProb >> probRate;
        }
        ctx->probs[state] = ctxProb;

        state = state<<1 | bit;

        // printf("%2d %08x %08x\n", i, ctx->base, ctx->range);
        normalizeEncode(ctx);
        // printf("c\n");
        // printf("%2d %08x %08x\n", i, ctx->base, ctx->range);
    }
}

void ari_flushEncode(ariCtxEncode* ctx){
    FILE* out = ctx->out;
    putc(ctx->head, out);
    for(uint32_t i = 0; i < ctx->cery; i++) putc(0xff, out);

    uint32_t base = ctx->base;
    for(int i = 0; i < sizeof(uint32_t); i++){
        putc(base>>8*3, out);
        base<<=8;
    }
    // putc(0x80, out);
}

token ari_decodeToken(ariCtxDecode* ctx){
    token state = 1;
    for(int i = 0; i < TOKENBITSIZE; i++){
        // printf("%x\n", i, state);
        uint16_t ctxProb = ctx->probs[state];
        uint32_t bound = (ctx->range>>probBit)*ctxProb;
        // printf("%08d %08x\n", ctxProb, bound);

        // if(ctx->val - ctx->base < bound){
        if(ctx->val < bound){ // 0
            // printf("0\n");
            ctx->range = bound;

            ctxProb += (1<<probBit) - ctxProb >> probRate;
            ctx->probs[state] = ctxProb;
            state = state<<1;
        }else{ // 1
            // printf("1\n");
            ctx->range -= bound;
            ctx->val   -= bound;
            // ctx->base += bound;

            ctxProb -= ctxProb >> probRate;
            ctx->probs[state] = ctxProb;
            state = state<<1 | 1;
        }

        if(ctx->range <= 0x00ffffffu){
            ctx->range <<= 8;
            // ctx->base  <<= 8;
            ctx->val = ctx->val<<8 | getc(ctx->in);
        }

        // printf("%2d %08x %08x %08x\n", i, ctx->base, ctx->range, ctx->val);
    }

    return state;
}