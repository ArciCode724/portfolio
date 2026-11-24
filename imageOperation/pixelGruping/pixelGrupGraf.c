#include"pixelGrupGraf.h"
#include<stdlib.h>
#include<stdio.h>

#define HASHBIGERBY 3

static uint32_t colorDistance(color8_3 a, color8_3 b){
    uint32_t retVal = 0;
    for (int i = 0; i < 3; i++){
        int16_t t = a.c[i] - b.c[i];
        retVal += t*t;
    }
    return retVal;
}

// --- hash

static uint64_t hashCon(pixelGrupGraf* graf, connection key){
    uint64_t a = key.between[0] - graf->grups;
    uint64_t b = key.between[1] - graf->grups;
    return (b*(b-1)/2 + a) % graf->hashNum;
}

static uint64_t hashGet(pixelGrupGraf* graf, connection key){
    uint64_t hash = hashCon(graf,key);
    // printf("(hash:%d)", hash);
    while (
        graf->conHashMap[hash].key[0] != key.between[0]
    ||  graf->conHashMap[hash].key[1] != key.between[1]
    ){
        hash = (hash+1)%graf->hashNum;
    }
    return graf->conHashMap[hash].val;
}

static void hashAdd(pixelGrupGraf* graf, connection key, uint64_t val){
    uint64_t hash = hashCon(graf,key);
    while (graf->conHashMap[hash].key[1] != NULL){
        hash = (hash+1)%graf->hashNum;
    }
    for (int i = 0; i < 2; i++){
        graf->conHashMap[hash].key[i] = key.between[i];
    }
    graf->conHashMap[hash].val = val;
}

static void hashDel(pixelGrupGraf* graf, connection key){
    uint64_t hash = hashCon(graf,key);
    while (
        graf->conHashMap[hash].key[0] != key.between[0]
    ||  graf->conHashMap[hash].key[1] != key.between[1]
    ){
        hash = (hash+1)%graf->hashNum;
    }
    for (int i = 0; i < 2; i++){
        graf->conHashMap[hash].key[i] = NULL;
    }
}

static void hashUpd(pixelGrupGraf* graf, connection key, uint64_t val){
    uint64_t hash = hashCon(graf,key);
    while (
        graf->conHashMap[hash].key[0] != key.between[0]
    ||  graf->conHashMap[hash].key[1] != key.between[1]
    ){
        hash = (hash+1)%graf->hashNum;
    }
    graf->conHashMap[hash].val = val;
}

// hash ---

// --- heap

static void heapSwap(pixelGrupGraf* graf, uint64_t i, uint64_t j){
    connection t = graf->conHeap[i];
    graf->conHeap[i] = graf->conHeap[j];
    graf->conHeap[j] = t;
    hashUpd(graf, graf->conHeap[i], i);
    hashUpd(graf, graf->conHeap[j], j);
}

static void heapUp(pixelGrupGraf* graf, uint64_t i){
    uint64_t p = (i-1)/2;
    while (i > 0 && graf->conHeap[i].distance < graf->conHeap[p].distance){
        heapSwap(graf, i,p);
        i = p; 
        p = (i-1)/2;
    }
}

static void heapDown(pixelGrupGraf* graf, uint64_t i){
    uint64_t c1 = 2*i+1;
    uint64_t c2 = 2*i+2;
    uint64_t toSwap;
    while (1){
        toSwap = i;
        if(c1 < graf->heapNum && graf->conHeap[c1].distance < graf->conHeap[toSwap].distance){ toSwap = c1; }
        if(c2 < graf->heapNum && graf->conHeap[c2].distance < graf->conHeap[toSwap].distance){ toSwap = c2; };
        if(toSwap == i){ break;}
        heapSwap(graf, i,toSwap);
        i = toSwap; 
        c1 = 2*i+1;
        c2 = 2*i+2;
    }
}

static void heapUpdate(pixelGrupGraf* graf, uint64_t i){
    heapUp(graf, i);
    heapDown(graf, i);
}

// heap ---

static void makeConection(pixelGrupGraf* graf, pixelGrupe* i, pixelGrupe* j){
    if(i>j){pixelGrupe* t=i; i=j; j=t;}

    connection newCon;
    newCon.between[0] = i;
    newCon.between[1] = j;
    newCon.distance = colorDistance(i->color, j->color);

    // printf("%d ", i->pixelsNum);
    i->neighborsId[i->neighborsNum++] = j;
    // printf("%d\n", i->pixelsNum);
    j->neighborsId[j->neighborsNum++] = i;

    graf->conHeap[graf->heapNum] = newCon;
    hashAdd(graf, newCon, graf->heapNum);
    heapUp(graf, graf->heapNum);
    graf->heapNum++;
}

static void brakeConection(pixelGrupGraf* graf, pixelGrupe* i, pixelGrupe* j){
    if(i > j){pixelGrupe* t=i;i=j;j=t;}
    // printf("%d %d\n", i-graf->grups, j-graf->grups);
    connection key = { {i,j},0 };
    uint64_t conPos = hashGet(graf,key);
    uint64_t partent;
    while (conPos > 0){
        partent = (conPos-1)/2;
        heapSwap(graf, conPos, partent);
        conPos = partent;
    }
    heapSwap(graf, 0, --graf->heapNum);
    heapDown(graf, 0);
    hashDel(graf,key);

    uint64_t index;
    for (index = 0; i->neighborsId[index] != j; index++);
    i->neighborsId[index] = i->neighborsId[--i->neighborsNum];

    for (index = 0; j->neighborsId[index] != i; index++);
    j->neighborsId[index] = j->neighborsId[--j->neighborsNum];
}

pixelGrupGraf* create_pixelGrupGraf(color8_3* imageData, uint32_t width, uint32_t height){
    pixelGrupGraf* retVal = malloc(sizeof(pixelGrupGraf));

    uint64_t grupsNum = width*height;
    retVal->grups = malloc(sizeof(pixelGrupe)*grupsNum);
    retVal->conHeap = malloc(sizeof(connection)*grupsNum*2);
    retVal->heapNum = 0;
    retVal->conHashMap = calloc(grupsNum*3, sizeof(struct {pixelGrupe* key[2]; uint64_t val;}));
    retVal->hashNum = grupsNum*HASHBIGERBY;

    // // innit grups
    for (int i = 0; i < grupsNum; i++){
        retVal->grups[i].color = imageData[i];

        retVal->grups[i].pixelsId = malloc(sizeof(uint64_t));
        retVal->grups[i].pixelsId[0] = i;
        retVal->grups[i].pixelsNum = 1;

        retVal->grups[i].neighborsId = malloc(sizeof(pixelGrupe)*4);
        retVal->grups[i].neighborsNum = 0;
    }

    // // // innit conection
    uint64_t i = 0, j = width;
    for (uint32_t y = 0; y < height-1; y++){
        for (uint32_t x = 0; x < width; x++){
            makeConection(retVal, retVal->grups+i,retVal->grups+j);
            i++; j++;
        }
    }
    i = 0, j = 1;
    for (uint32_t y = 0; y < height; y++){
        for (uint32_t x = 0; x < width-1; x++){
            makeConection(retVal, retVal->grups+i,retVal->grups+j);
            i++; j++;
        }
        i++; j++;
    }
    
    return retVal;
}

void free_pixelGrupGraf(pixelGrupGraf* toFree){
    free(toFree->conHeap);
    free(toFree->conHashMap);
    for (uint64_t i = 0; i < toFree->hashNum/HASHBIGERBY; i++){
        free(toFree->grups[i].neighborsId);
        free(toFree->grups[i].pixelsId);
    }
    free(toFree->grups);
}

connection minDistance(pixelGrupGraf* graf){
    return graf->conHeap[0];
}

void pixelGrupGraf_mergeConnection(pixelGrupGraf* graf, connection con){
    pixelGrupe* a = con.between[0];
    pixelGrupe* b = con.between[1];

    brakeConection(graf, con.between[0], con.between[1]);
    
    // avg color
    for (uint64_t i = 0; i < 3; i++) {
        double newColor = 0;
        newColor += (double)a->pixelsNum * a->color.c[i];
        newColor += (double)b->pixelsNum * b->color.c[i];
        newColor /= a->pixelsNum+b->pixelsNum;
        newColor += 0.5;

        a->color.c[i] = newColor;
    }
    
    // merge pixels list color
    int newPizelSize = a->pixelsNum+b->pixelsNum;
    a->pixelsId = realloc(a->pixelsId, newPizelSize*sizeof(uint64_t));
    for (uint64_t i = 0; i < b->pixelsNum; i++){
        a->pixelsId[i+a->pixelsNum] = b->pixelsId[i];
    }
    a->pixelsNum = newPizelSize;
    b->pixelsNum = 0;
    free(b->pixelsId);
    b->pixelsId = NULL;

    // update Conncetion
    for (uint64_t i = 0; i < a->neighborsNum; i++){
        connection key = {{a,a->neighborsId[i]},0};
        if(a > a->neighborsId[i]){ 
            key.between[0] = a->neighborsId[i];
            key.between[1] = a;
        }
        uint64_t heapIndex = hashGet(graf, key);
        graf->conHeap[heapIndex].distance = colorDistance(a->color, a->neighborsId[i]->color);
        heapUpdate(graf, heapIndex);
    }
    
    // merge neibors
    uint64_t newNeithSize = a->neighborsNum+b->neighborsNum;
    pixelGrupe** tempNeighborsB = malloc(sizeof(pixelGrupe*)*b->neighborsNum);
    uint64_t tempNeighborsBNum = b->neighborsNum;
    for (uint64_t i = 0; i < b->neighborsNum; i++){ tempNeighborsB[i] = b->neighborsId[i];}
    for (uint64_t i = 0; i < tempNeighborsBNum; i++){ brakeConection(graf, b, tempNeighborsB[i]);}
    free(b->neighborsId);
    b->neighborsId = NULL;

    a->neighborsId = realloc(a->neighborsId, newNeithSize*sizeof(uint64_t));
    for (uint64_t i = 0; i < tempNeighborsBNum; i++){
        int exist = 0;
        for (uint64_t j = 0; j < a->neighborsNum; j++){
            if(a->neighborsId[j] == tempNeighborsB[i]){ exist = 1; break; }
        }
        if(exist){
            continue;
        }
        makeConection(graf, a, tempNeighborsB[i]);
    }
    free(tempNeighborsB);
}

void pixelGrupGrafToimage(pixelGrupGraf* graf, color8_3* imageDataOut){
    for(uint64_t i=0; i<graf->hashNum/HASHBIGERBY; i++){
        for(uint64_t j=0; j<graf->grups[i].pixelsNum; j++){
            imageDataOut[ graf->grups[i].pixelsId[j] ] = graf->grups[i].color;
        }
    }
}