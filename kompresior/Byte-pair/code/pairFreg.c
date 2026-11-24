#include"../head/pairFreg.h"
#include<stdlib.h>
#include<stdio.h>

// https://en.wikipedia.org/wiki/Linear_probing

pairFreg* pairFreg_malloc(){
    const size_t startSize = 1;
    pairFreg* retVal = malloc(sizeof(pairFreg));
    retVal->keys = malloc(sizeof(pair)*startSize);
    retVal->vals = malloc(sizeof(int)*startSize);
    for(size_t i=0; i<startSize; i++) retVal->vals[i] = 0;
    retVal->size = startSize;
    retVal->fill = 0;

    retVal->newKeys = malloc(sizeof(pair)*retVal->size);
    retVal->newKeysNum = 0;

    return retVal;
}

void pairFreg_free(pairFreg* toFree){
    free(toFree->keys);
    free(toFree->vals);
    free(toFree->newKeys);
    free(toFree);
}

void pairFreg_print(pairFreg* toPrint){
    for (size_t i = 0; i < toPrint->size; i++){
        printf("%3lld; %3d", i, toPrint->vals[i]);
        if(toPrint->vals[i] != 0) printf("; %3d, %3d", toPrint->keys[i].l, toPrint->keys[i].r);
        printf("\n");
    }
    printf("\n");
}

size_t pairFreg_mostOftenPair(pairFreg* frag){
    int biget = 0;
    for(int i = 0; i < frag->size; i++){
        // if(frag->vals[i] > 2) return i;
        if(frag->vals[i] > frag->vals[biget]) biget = i;
    }
    return biget;
}

static size_t hash(pair key, size_t size){
    // return (3*key.l + 5*key.r)%size;
    return ((key.l + key.r )*(key.l + key.r + 1)/2 + key.l)%size;
} 

static void changeSize(pairFreg* frag, size_t newSize){
    // printf("a");
    pair* new_keys = malloc(sizeof(pair)*newSize);
    int*  new_vals = malloc(sizeof(int)*newSize);
    for(size_t i=0; i < newSize; i++) new_vals[i] = 0;

    // printf("old size %lld\nnew size %lld\n file %lld\n",frag->size, newSize, frag->fill);
    for (int i = 0; i < frag->size; i++){
        if( frag->vals[i] == 0 ) continue;
        // printf("a");
       
        size_t hashCode = hash(frag->keys[i], newSize);
        for(;;){
            if( new_vals[hashCode] == 0 ){
                new_keys[hashCode] = frag->keys[i];
                new_vals[hashCode] = frag->vals[i];
                break;
            }
            hashCode = (hashCode+1)%newSize;
        }
    }
    // printf("\n");

    free(frag->keys);
    free(frag->vals);
    frag->keys = new_keys;
    frag->vals = new_vals;
    frag->size = newSize;
    // printf("a");
    frag->newKeys = realloc(frag->newKeys, sizeof(pair)*(frag->newKeysNum + frag->size - frag->fill));
}

static void incresSize(pairFreg* frag){
    // changeSize(frag, frag->size*2);
    changeSize(frag, frag->fill*2);
}

static void decresSize(pairFreg* frag){
    // changeSize(frag, frag->size/2);
    // printf("%lld\n", frag->fill*2 + 1);
    changeSize(frag, frag->fill*2 + 1);
}

void pairFreg_fregUpPair(pairFreg* frag, pair key){
    // if(frag->size/4 <= frag->fill/3) incresSize(frag);
    size_t hashCode = hash(key, frag->size);
    while(1){
        if(frag->vals[hashCode] == 0){
            frag->keys[hashCode] = key;
            frag->vals[hashCode] = 1;
            frag->fill++;

            frag->newKeys[frag->newKeysNum++] = key;
            // if(0.75 < (double)frag->fill/frag->size) incresSize(frag);
            if(frag->size/4 <= frag->fill/3) incresSize(frag);
            return;
        }

        if( frag->keys[hashCode].l == key.l
         && frag->keys[hashCode].r == key.r
        ){
            frag->vals[hashCode]++;
            return;
        };

        hashCode = (hashCode+1)%frag->size;
    }
}

static size_t cyclicDistance(size_t l, size_t r, size_t n){
    if(l <= r) return r-l;
    return n-l+r;
}

static void fillNull(pairFreg* frag, size_t toFill){
    // printf("a");
    size_t d1 = 0;
    for(size_t i = (toFill+1)%frag->size; frag->vals[i] != 0; i = (i+1)%frag->size){
        // size_t d1 = cyclicDistance(toFill, i, frag->size);
        d1 = (d1+1)%frag->size;
        size_t d2 = cyclicDistance( hash(frag->keys[i],frag->size), i, frag->size);
        if( d1 <= d2 ){
            frag->keys[toFill] = frag->keys[i];
            frag->vals[toFill] = frag->vals[i];

            frag->vals[i] = 0;
            toFill = i;
            d1 = 0;
        }
    }
    // printf("b\n");
}

static void removeEntry(pairFreg* frag, size_t hashCode){
    frag->vals[hashCode] = 0;
    frag->fill--;
    
    if(frag->fill < frag->size/4){
        // printf("a");
        decresSize(frag);
        // printf("b\n");
    }
    else {
        // printf("c");
        fillNull(frag, hashCode);
        // printf("d\n");
    }
}

void pairFreg_fregDownPairMin(pairFreg* frag, pair key, int minFreg){
    size_t hashCode = hash(key, frag->size);
    while(frag->vals[hashCode] != 0){
        if( frag->keys[hashCode].l == key.l
         && frag->keys[hashCode].r == key.r
        ){
            frag->vals[hashCode]--;

            if(frag->vals[hashCode] < minFreg){
                removeEntry(frag, hashCode);
            }
            return;
        };

        hashCode = (hashCode+1)%frag->size;
    }
}

void pairFreg_fregDownPair(pairFreg* frag, pair key){
    pairFreg_fregDownPairMin(frag, key, 1);
}

void pairFreg_checkBound(pairFreg* frag, int min){ 
    // for(size_t i = 0; i < frag->newKeysNum; i++) printf("%d %d; ",frag->newKeys[i].l, frag->newKeys[i].r);
    // printf("\n");  
    for(size_t i = 0; i < frag->newKeysNum; i++){
        // pairFreg_print(frag);
        // for(size_t i = 0; i < frag->newKeysNum; i++) printf("%d %d; ",frag->newKeys[i].l, frag->newKeys[i].r);
        // printf("\n");  
        pair key = frag->newKeys[i];
        size_t hashCode = hash(key, frag->size);
        while(frag->vals[hashCode] != 0){
            if( frag->keys[hashCode].l == key.l
            &&  frag->keys[hashCode].r == key.r
            ){
                break;
            };
            
            hashCode = (hashCode+1)%frag->size;
        }
        if(frag->vals[hashCode] < min){
            // printf("remove %lld %lld, %lld", key.l, key.r ,hashCode);
            removeEntry(frag, hashCode);
            // printf("c\n");
        }
        // printf("\n");
    }
    frag->newKeysNum = 0;
}

void pairFreg_removePair(pairFreg* frag, pair key){
    size_t hashCode = hash(key, frag->size);
    while(frag->vals[hashCode] != 0){
        if( frag->keys[hashCode].l == key.l
         && frag->keys[hashCode].r == key.r
        ){
            removeEntry(frag, hashCode);
            break;
        };

        hashCode = (hashCode+1)%frag->size;
    }
}
