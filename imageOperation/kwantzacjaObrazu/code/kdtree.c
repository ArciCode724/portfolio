#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"..\header\kdtree.h"

#include<stdio.h>

// typedef struct _vec2{
//     int a[2];
// } vec2;

#define swapVoid(a,b) {void* t=a; a=b; b=t;}
#define pointerToElem(t,i) t->data + i*t->elemSize
#define NOCHILD -3
#define LEFTCHILD -2
#define RIGHTCHILD -1

// ktury element to mediana
// quickSelect implementation
static int findMedian(void* list, int numData, int elemSize, int d, int (*comp)(const void* a, const void* b, int d) ){
    if(numData == 1){
        return 0;
    }
    void* elemens[numData];
    for (int i = 0; i < numData; i++){
        elemens[i] = list + i*elemSize;
    //     printf("%d ", *((int*)(elemens[i])) );
    }
    // printf("\n");
    
    void** lookList = elemens;
    int elemNumber = numData;
    int lookFor = numData/2;
    while(1){
        swapVoid(lookList[0], lookList[lookFor]);
        int split = 0;
        for (int i = 1; i < elemNumber; i++){
            // printf("%d ", comp(lookList[i], lookList[0], d) );
            if(comp(lookList[i], lookList[0], d) >= 0){
                continue;
            }
            split++;
            swapVoid(lookList[split], lookList[i]);
        }
        swapVoid(lookList[0], lookList[split]);

        // printf("%d %d\n", split, lookFor);
        // for (int i = 0; i < numData; i++){
        //     printf("%d ", *((int*)(elemens[i])) );
        // }
        // printf("\n");

        if(split == lookFor){
            break;
        }
        if(split < lookFor){
            split++;
            elemNumber -= split;
            lookFor -= split;
            lookList += split;
        }else if(split > lookFor){
            elemNumber = split;
        }
        // printf("b");
    }

    // printf("%d\n", *((int*)(elemens[numData/2])) );

    return (elemens[numData/2] - list)/elemSize;
}

// #define swapMemory(list, i, j, size) {}
static void swapMemory(void* list, int i, int j, int size){
    uint8_t t[size];
    void* pa = list + i*size;
    void* pb = list + j*size;

    memcpy(t, pa, size);
    memcpy(pa, pb, size);
    memcpy(pb, t, size);
}

// rozmiar zajmowanej pamieci to numChunk*tree->elemSize
static void orderData(kdtree* tree, int chunkStart, int numChunk, int curentD){
    if(numChunk == 0){
        return;
    }
    if(numChunk == 1){
        tree->split[chunkStart] = NOCHILD;
        return;
    }
    void* curentChunk = tree->data + tree->elemSize*chunkStart;
    int medianPos = findMedian(curentChunk, numChunk, tree->elemSize, curentD, tree->comp);
    swapMemory(curentChunk, 0, medianPos, tree->elemSize);
    int splitAt = 1;
    for (int i = 1; i < numChunk; i++){
        // printf("%d ", comp(lookList[i], lookList[0], d) );
        if(tree->comp(curentChunk + i*tree->elemSize, curentChunk, curentD) >= 0){
            continue;
        }
        swapMemory(curentChunk, splitAt, i, tree->elemSize);
        splitAt++;
    }
    tree->split[chunkStart] = chunkStart + splitAt;
    if(numChunk == splitAt){
        tree->split[chunkStart] = LEFTCHILD;
    }else if(splitAt == 1){
        tree->split[chunkStart] = RIGHTCHILD;
    }

    orderData(tree, chunkStart+1, splitAt-1, (curentD+1)%(tree->dimention) );
    orderData(tree, chunkStart + splitAt, numChunk-splitAt, (curentD+1)%(tree->dimention));
}

kdtree* createKdtree(void* data, int numData, int elemSize, int dimention, int (*comp)(const void* a, const void* b, int d)){
    kdtree* retVal = malloc(sizeof(kdtree));
    retVal->data = malloc(numData*elemSize);
    retVal->split = malloc(numData*sizeof(int));
    memcpy(retVal->data, data, numData*elemSize);
    retVal->numData = numData;
    retVal->elemSize = elemSize;
    retVal->dimention = dimention;
    retVal->comp = comp;

    orderData(retVal, 0, numData, 0);
    // findMedian(retVal->data, numData, elemSize, dimention, comp);
    // for (int i = 0; i < numData; i++){
    //     printf("%2d ", i );
    // }
    // printf("\n");
    // for (int i = 0; i < numData; i++){
    //     printf(" (%d %d)", ((vec2*)(retVal->data))[i].a[0], ((vec2*)(retVal->data))[i].a[1] );
    // }
    // printf("\n");
    //     for (int i = 0; i < numData; i++){
    //     printf("%8d", retVal->split[i] );
    // }
    // printf("\n");

    return retVal;
}

void freeKdtree(kdtree* toFree){
    free(toFree->data);
    free(toFree->split);
    free(toFree);
}

// ---

#define drawBack(s, n) { for (int i = 0; i < n-2; i++){ printf("%c  ", (s[i] != s[i+1]) ? '|' : ' ');} }

static void printNode(const void* node, uint8_t* side, int numSide, void (*printElem)(const void* a)){  
    drawBack(side, numSide);
    // printf("%d ", side[numSide-1]);
    if(numSide != 1){
        printf("+--");
    }
    // printf("%d\n",node);
    printElem(node);
    printf("\n");
}

static void kdtreePrintRecur(kdtree* tree, int node, uint8_t* side, int numSide, void (*printElem)(const void* a)){
    int curentSplit = tree->split[node];
    // printf("<%d>",curentSplit);
    if(curentSplit == NOCHILD ){ // end
        printNode(pointerToElem(tree, node), side, numSide, printElem);
        return;
    }
    int nextNode;
    if(curentSplit != LEFTCHILD){ // right
        nextNode = (curentSplit == RIGHTCHILD) ? node + 1 : curentSplit;
        side[numSide-1] = 1;
        kdtreePrintRecur(tree, nextNode, side, numSide+1, printElem);
        
        drawBack(side, numSide+1);
        printf("|\n");
    }
    printNode(pointerToElem(tree, node), side, numSide, printElem);
    if(curentSplit != RIGHTCHILD){ // left
        nextNode = node + 1;
        side[numSide-1] = 0;

        drawBack(side, numSide+1);
        printf("|\n");

        kdtreePrintRecur(tree, nextNode, side, numSide+1, printElem);
    }
};

void kdtreePrint(kdtree* tree, void (*printElem)(const void* a)){
    uint8_t side[tree->numData];
    kdtreePrintRecur(tree, 0, side, 1, printElem);
}

// ---

static int kdtreeFindClosestRecur(const void* closTo, int root, int d, kdtree* tree, double (*dist)(const void* a, const void* b, int d)){
    if(tree->split[root] == NOCHILD){
        return root;
    }
    d %= tree->dimention;
    int firstBranch, secendBranch;
    int branchExit[2] = {tree->split[root] != RIGHTCHILD, tree->split[root] != LEFTCHILD};
    firstBranch = root + 1;
    secendBranch = (tree->split[root] == RIGHTCHILD) ? root + 1 : tree->split[root];
    if(tree->comp(closTo, pointerToElem(tree, root), d) >= 0 ){
        firstBranch ^= secendBranch;
        secendBranch ^= firstBranch;
        firstBranch ^= secendBranch;

        branchExit[0] ^= branchExit[1];
        branchExit[1] ^= branchExit[0];
        branchExit[0] ^= branchExit[1];
    }
    int best;
    if(branchExit[0]){
        best = kdtreeFindClosestRecur(closTo, firstBranch, d+1, tree, dist);
    }else{
        best = root;
    }
    double bestDist = dist(closTo, pointerToElem(tree, best), -1);
    if( dist(closTo, pointerToElem(tree, root), d) <= bestDist){
        int potenBest = root;
        double potenlaDist = dist(closTo, pointerToElem(tree, root), -1);
        // printf("%lf %lf", bestDist, potenlaDist);
        if(bestDist > potenlaDist){
            best = potenBest;
            bestDist = potenlaDist;
        }

        if(branchExit[1]){
            potenBest = kdtreeFindClosestRecur(closTo, secendBranch, d+1, tree, dist);
            potenlaDist = dist(closTo, pointerToElem(tree, potenBest), -1);
            if(bestDist > potenlaDist){
                best = potenBest;
                bestDist = potenlaDist;
            }
        }
    }
    return best;
}

// // dist dla d=-1 podaj dystans na bazie calosi, d>=0 podaj dystans na bazie jednej plaszczyzny
void kdtreeFindClosest(void* out, const void* closTo, kdtree* tree, double (*dist)(const void* a, const void* b, int d)){
    int best = kdtreeFindClosestRecur(closTo, 0, 0, tree, dist);
    memcpy(out, pointerToElem(tree, best), tree->elemSize);
}