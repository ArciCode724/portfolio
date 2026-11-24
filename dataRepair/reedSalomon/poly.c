#include"poly.h"
#include"gf256.h"
#include<stdlib.h>
#include<stdio.h>

// jest poporawnei poniewasz potrzebuyjemy pamieci deg+1
#define quantSize(x) (x/16+1)*16

poli mallocPoli(uint8_t deg){
    poli retVal;
    retVal.deg = deg;
    retVal.coef = calloc(quantSize(deg), sizeof(uint8_t));
    return retVal;
}

void freePoli(poli toFree){
    free(toFree.coef);
}

void poliResize(poli* p, uint8_t deg){
    if(quantSize(p->deg) != quantSize(deg)){
        p->coef = realloc(p->coef, sizeof(uint8_t)*quantSize(deg));
    }
    if(p->deg < deg){
        for (int i = p->deg+1; i <= deg; i++){
            p->coef[i] = 0;
        }
    }
    p->deg = deg;
}

void poliShorten(poli* p){
    uint8_t newDeg = p->deg;
    while (newDeg != 0 && p->coef[newDeg] == 0){
        newDeg--;
    }
    poliResize(p, newDeg);
}

void printPoli(poli p, char end){
    printf("%d", p.coef[0]);
    for (int i = 1; i <= p.deg; i++){
        // if(p.coef[i] == 0){
        //     continue;
        // }
        printf(" + %dx^%d", p.coef[i], i);
    }
    putchar(end);
}

void poliMinus(poli p1, poli p2, poli* out){
    if(p1.deg < p2.deg){
        poli temp = p1;
        p1 = p2;
        p2 = temp;
    }

    int toOut[p1.deg+1];
    int i = 0;
    for (; i <= p2.deg; i++){
        toOut[i] = galoisMinus(p1.coef[i], p2.coef[i]);
    }
    for (; i <= p1.deg; i++){
        toOut[i] = p1.coef[i];
    }
    poliResize(out, p1.deg);
    for (i = 0; i <= p1.deg; i++){
        out->coef[i] = toOut[i];
    } 
    poliShorten(out);
}

void poliMulti(poli p1, poli p2, poli* out){
    int outDeg = p1.deg+p2.deg;
    uint8_t toOut[out->deg+1];
    for(int i = 0; i <= outDeg; i++){
        toOut[i] = 0;
    }
    for(int i = 0; i <= p2.deg; i++){
        for (int j = 0; j <= p1.deg; j++){
            toOut[i+j] = galoisAdd( toOut[i+j], galoisMulti(p1.coef[j], p2.coef[i]) );
        }
    }
    poliResize(out, outDeg);
    for(int i = 0; i <= outDeg; i++){
        out->coef[i] = toOut[i];
    }
    poliShorten(out);
}

void poliDivis(poli p1, poli p2, poli* out, poli* outRest){
    uint8_t up[p1.deg+1];
    for (int i = 0; i <= p1.deg; i++){
        up[i] = galoisDivis( p1.coef[i], p2.coef[p2.deg] );
    }
    
    uint8_t down[p2.deg+1];
    for (int i = 0; i <= p2.deg; i++){
        down[i] = galoisDivis( p2.coef[i], p2.coef[p2.deg] );
    }

    int outDeg = p1.deg-p2.deg;
    uint8_t toOut[outDeg + 1];
    
    for (int i = outDeg; i >= 0; i--){
        toOut[i] = up[i+p2.deg];
        for (int j = 0; j <= p2.deg; j++){
            up[i+j] = galoisMinus( up[i+j], galoisMulti(down[j], toOut[i]) );
        }
    }

    if(out != NULL){
        poliResize(out, outDeg);
        for(int i = 0; i <= outDeg; i++){
            out->coef[i] = toOut[i];
        }
        poliShorten(out);
    }

    if(outRest != NULL){
        poliResize(outRest, p2.deg-1);
        for(int i = 0; i <= p2.deg-1; i++){
            outRest->coef[i] = galoisMulti( up[i], p2.coef[p2.deg] );
        }
        poliShorten(outRest);
    }
}

uint8_t poliValuAt(poli p, uint8_t x){
    // uint8_t retVal = 0;
    // uint8_t multi = 1;
    // for (int i = 0; i <= p.deg; i++){
    //     retVal = galoisAdd(retVal, galoisMulti(p.coef[i], multi, ctx));
    //     multi = galoisMulti(x, multi, ctx);
    // }
    uint8_t retVal = 0;
    for (int i = p.deg; i >= 0; i--){
        retVal = galoisMulti(x, retVal);
        retVal = galoisAdd(retVal, p.coef[i]);
    } 

    return retVal;
}