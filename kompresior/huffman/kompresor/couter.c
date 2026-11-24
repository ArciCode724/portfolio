#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"couter.h"
#include"bitIo.h"

couter* couter_init(){
    couter* retVal = malloc(sizeof(couter));
    retVal->n = (int)pow(2, 8*sizeof(BYTE));
    retVal->what = malloc(sizeof(BYTE)*retVal->n);
    retVal->ile = malloc(sizeof(int)*retVal->n);

    for(int i=0; i<retVal->n; i++){
        retVal->ile[i] = 0;
        retVal->what[i] = i;
    }

    return retVal;
};

void couter_free(couter* toTree){
    free(toTree->ile);
    free(toTree->what);
    free(toTree);
}

// tylko dla posotrownego po ->what
void couter_count(couter* write, char* fileName){
    bitI* reader = bitI_init(fileName);
    BYTE b;
    while (bitI_getByte(reader, &b)){
        write->ile[b]++;
    }
    bitI_close(reader);
};

void couter_swap(couter* base, int a, int b){
    BYTE tempBYTE = base->what[a];
    base->what[a] = base->what[b];
    base->what[b] = tempBYTE;

    int tempInt = base->ile[a];
    base->ile[a] = base->ile[b];
    base->ile[b] = tempInt;
}

int couter_qsort_split(couter* sort, int start, int end){
    int pivit = sort->ile[start];
    int splitPoint = start+1;

    for (int i = start+1; i < end; i++){
        if(sort->ile[i] > pivit){
            couter_swap(sort, splitPoint, i);
            splitPoint++;
        }
    }
    couter_swap(sort, start, splitPoint-1);
    return splitPoint;
};

void couter_qsort(couter* sort, int start, int end){
    // printf("%d - %d: ", start, end);
    if(end - start <= 1)
        return;

    int center = couter_qsort_split(sort, start, end);
    // printf("%d\n", center);
    couter_qsort(sort, start, center);
    couter_qsort(sort, center, end);
}

void couter_print(couter* toPrint){
    for (int i = 0; i < toPrint->n; i++){
        // if(toPrint->ile[i]==0)
        //     continue;
        printf("%*x: %d\n", (int)(2*sizeof(BYTE)), toPrint->what[i], toPrint->ile[i]);
    }
}

// on sorted
void couter_remowEmpy(couter* renmowFrom){
    for (; !renmowFrom->ile[renmowFrom->n-1]; renmowFrom->n--);
    renmowFrom->what = realloc(renmowFrom->what,sizeof(BYTE)*renmowFrom->n);
    renmowFrom->ile = realloc(renmowFrom->ile,sizeof(int)*renmowFrom->n);
}

double couter_entory(couter* of){
    int suma = 0;
    for (int i = 0; i < of->n; i++){
        suma+=of->ile[i];
    }
    double entroy = 0;
    for (int i = 0; i < of->n; i++){
        if(of->ile[i] == 0)
            continue;
        double prob = of->ile[i]/(double)suma;
        entroy -= prob*log2(prob);
    }
    return entroy;
}
