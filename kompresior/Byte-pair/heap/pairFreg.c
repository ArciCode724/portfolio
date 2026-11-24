#include"../head/pairFreg.h"
#include<stdlib.h>
#include<stdio.h>

// https://en.wikipedia.org/wiki/Linear_probing

#define noKey -1

pairFreg* pairFreg_malloc(){
    const size_t startSize = 1;
    pairFreg* retVal = malloc(sizeof(pairFreg));

    retVal->hashMap = malloc(sizeof(fregEntry)*startSize);
    for(int i=0;i<startSize;i++) retVal->hashMap[i].val = noKey;
    retVal->hashFill = 0;
    retVal->hashSize = startSize;

    retVal->heap = malloc(sizeof(fregEntry)*startSize);
    retVal->heapFill = 0;
    retVal->heapSize = startSize;

    return retVal;
}

void pairFreg_free(pairFreg* toFree){
    free(toFree->heap);
    free(toFree->hashMap);
    free(toFree);
}

void pairFreg_print(pairFreg* toPrint){
    printf("hashMap\n");
    for (size_t i = 0; i < toPrint->hashSize; i++){
        printf("%3lld; %3d", i, toPrint->hashMap[i].val);
        if(toPrint->hashMap[i].val != noKey){
            printf("; %3d, %3d", toPrint->hashMap[i].key.l, toPrint->hashMap[i].key.r);
        }
        printf("\n");
    }
    printf("heap\n");
    for (size_t i = 0; i < toPrint->heapFill; i++){
        printf("%3lld; %3d", i, toPrint->heap[i].val);
        printf("; %3d, %3d", toPrint->heap[i].key.l, toPrint->heap[i].key.r);
        printf("\n");
    }
}

fregEntry pairFreg_mostOftenPair(pairFreg* frag){
    return frag->heap[0];
}

static size_t hash(pair key, size_t size){
    // return (3*key.l + 5*key.r)%size;
    return ((key.l + key.r )*(key.l + key.r + 1)/2 + key.l)%size;
} 

static void changeSize(pairFreg* frag, size_t newSize){
    fregEntry* newHash = malloc(sizeof(fregEntry)*newSize);
    for(int i=0; i < newSize; i++) newHash[i].val = noKey;

    for (int i = 0; i < frag->hashSize; i++){
        if( frag->hashMap[i].val == noKey ) continue;
       
        size_t hashCode = hash(frag->hashMap[i].key, newSize);
        for(;;){
            if( newHash[hashCode].val == noKey ){
                newHash[hashCode] = frag->hashMap[i];
                break;
            }
            hashCode = (hashCode+1)%newSize;
        }
    }
    free(frag->hashMap);
    frag->hashMap = newHash;
    frag->hashSize = newSize;
}

static void incresSize(pairFreg* frag){
    changeSize(frag, frag->hashSize*2);
}

static void decresSize(pairFreg* frag){
    changeSize(frag, frag->hashSize/2);
}

static fregEntry* getHashEntry(pairFreg* frag, pair key){
    size_t hashCode = hash(key, frag->hashSize);
    while(1){
        if(frag->hashMap[hashCode].val == noKey){
            fprintf(stderr, "szukanie czego nie ma");
            exit(1);
        }

        if( frag->hashMap[hashCode].key.l == key.l
         && frag->hashMap[hashCode].key.r == key.r
        ){
            return frag->hashMap + hashCode;
        };

        hashCode = (hashCode+1)%frag->hashSize;
    }
}

static void heapSwap(pairFreg* frag, fregEntry* hashEntryA, fregEntry* hashEntryB){
    fregEntry tempheap = frag->heap[hashEntryA->val];
    frag->heap[hashEntryA->val] = frag->heap[hashEntryB->val];
    frag->heap[hashEntryB->val] = tempheap;

    int tempHashVal = hashEntryA->val;
    hashEntryA->val = hashEntryB->val;
    hashEntryB->val = tempHashVal;
}

static void heapUp(pairFreg* frag, fregEntry* hashEntry){
    int partent = (hashEntry->val-1)/2;
    while(partent >= 0){
        if(frag->heap[partent].val >= frag->heap[hashEntry->val].val) return;
        heapSwap(frag, hashEntry, getHashEntry(frag, frag->heap[partent].key));

        partent = (hashEntry->val-1)/2;
    }
}

static void heapDown(pairFreg* frag, fregEntry* hashEntry){
    int childe1 = 2*hashEntry->val+1;
    int childe2 = 2*hashEntry->val+2;
    while (childe1 < frag->heapFill){
        int toSwap = childe1;
        if(childe2 < frag->heapFill && frag->heap[childe1].val < frag->heap[childe2].val ) toSwap = childe2;
        if(frag->heap[hashEntry->val].val >= frag->heap[toSwap].val) return;
        heapSwap(frag, hashEntry, getHashEntry(frag, frag->heap[toSwap].key));

        childe1 = 2*hashEntry->val+1;
        childe2 = 2*hashEntry->val+2;
    }
}

static void heapRemowe(pairFreg* frag, fregEntry* hashEntry){
    int partent = (hashEntry->val-1)/2;
    while(partent > 0){
        heapSwap(frag, hashEntry, getHashEntry(frag, frag->heap[partent].key));
        partent = (hashEntry->val-1)/2;
    }

    fregEntry* last = getHashEntry(frag, frag->heap[frag->heapFill-1].key);
    heapSwap(frag, hashEntry, last);
    frag->heapFill--;
    heapDown(frag, last);
}

void pairFreg_fregUpPair(pairFreg* frag, pair key){
    size_t hashCode = hash(key, frag->hashSize);
    while(1){
        fregEntry* entry = frag->hashMap + hashCode;

        if(entry->val == noKey){
            if(frag->heapFill >= frag->heapSize){
                frag->heapSize*=2;
                frag->heap = realloc(frag->heap, sizeof(fregEntry)*frag->heapSize);
            }
            int id = frag->heapFill++;

            entry->key = key;
            entry->val = id;

            frag->heap[id].key = key;
            frag->heap[id].val = 1;

            heapUp(frag, entry);

            frag->hashFill++;
        //     // if(0.75 < (double)frag->fill/frag->size) incresSize(frag);
            if(frag->hashSize/4 <= frag->hashFill/3) incresSize(frag);
            return;
        }

        if(entry->key.l == key.l
        && entry->key.r == key.r){
            frag->heap[entry->val].val++;

            // printf("%3d", frag->heap[entry->val].val);
            // printf("; %3d, %3d", frag->heap[entry->val].key.l, frag->heap[entry->val].key.r);
            // printf("\n");

            heapUp(frag, entry);
            return;
        };

        hashCode = (hashCode+1)%frag->hashSize;
    }
}

static size_t cyclicDistance(size_t l, size_t r, size_t n){
    if(l <= r) return r-l;
    return n-l+r;
}

static void fillNull(pairFreg* frag, size_t toFill){
    size_t d1 = 0;
    for(size_t i = (toFill+1)%frag->hashSize; frag->hashMap[i].val != noKey; i = (i+1)%frag->hashSize){
        // size_t d1 = cyclicDistance(toFill, i, frag->size);
        d1 = (d1+1)%frag->hashSize;
        size_t d2 = cyclicDistance(hash(frag->hashMap[i].key,frag->hashSize), i, frag->hashSize);
        if( d1 <= d2 ){
            frag->hashMap[toFill] = frag->hashMap[i];
            frag->hashMap[i].val = noKey;
            toFill = i;
            d1 = 0;
        }
    }
}

void pairFreg_fregDownPair(pairFreg* frag, pair key){
    size_t hashCode = hash(key, frag->hashSize);
    fregEntry* entry = frag->hashMap + hashCode;
    while(entry->val != noKey){

        if(entry->key.l == key.l
        && entry->key.r == key.r){
            frag->heap[entry->val].val--;
            heapDown(frag, entry);
            return;
        };

        hashCode = (hashCode+1)%frag->hashSize;
        entry = frag->hashMap + hashCode;
    }
}

static void removeHashEntry(pairFreg* frag, pair key){
    size_t hashCode = hash(key, frag->hashSize);
    fregEntry* entry = frag->hashMap + hashCode;
    while(entry->val != noKey){

        if(entry->key.l == key.l
        && entry->key.r == key.r){
            entry->val = noKey;
            frag->hashFill--;

            if(frag->hashFill < frag->hashSize/4) decresSize(frag);
            else fillNull(frag, hashCode);

            return;
        };

        hashCode = (hashCode+1)%frag->hashSize;
        entry = frag->hashMap + hashCode;
    }
}
            
void pairFreg_pruneHeap(pairFreg* frag, int min){
    for(int i=frag->heapFill-1; i>=0; i--){
        if(frag->heap[i].val >= min) break;
        removeHashEntry(frag, frag->heap[i].key);
        frag->heapFill--;
    }
    while(frag->heapFill < frag->heapSize/2){
        frag->heapSize/=2;
        frag->heap = realloc(frag->heap, sizeof(fregEntry)*frag->heapSize);
    }
}