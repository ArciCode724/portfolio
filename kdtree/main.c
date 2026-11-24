#include<stdio.h>
#include"kdtree.h"

#include<math.h>

#include<stdlib.h>
#include<time.h>

int intComp(const void* a, const void* b, int d){
    return *(int*)a - *(int*)b;
}

double intDyst(const void* a, const void* b, int d){
    return (double)abs(*(int*)a - *(int*)b);
}

void intPrint(const void* a){
    printf("%d", *(int*)a);
}

void test1(){
    srand(time(NULL));
    int l[30];
    for (int i = 0; i < 30; i++){
        l[i] = rand()%(99-10) + 10;
        // l[i] = i;
    }
    // l[7] = l[8];
    // l[6] = l[8];
    kdtree* t = createKdtree(l, 2, sizeof(int), 1, intComp);

    int closest;
    int closestTo = 50;
    kdtreeFindClosest(&closest, &closestTo, t, intDyst);
    printf("%d\n", closest);

    kdtreePrint(t, intPrint);
    freeKdtree(t);
}

typedef struct _vec2{
    int a[2];
} vec2;

int vec2Comp(const void* a, const void* b, int d){
    return (*(vec2*)a).a[d] - (*(vec2*)b).a[d];
}

double vec2Dyst(const void* a, const void* b, int d){
    double sum = 0;
    double oneD[2];
    for (int i = 0; i < 2; i++){
        oneD[i] = ((vec2*)a)->a[i] - ((vec2*)b)->a[i];
        oneD[i] *= oneD[i];
        sum += oneD[i];
    }
    if(d == -1){
        return sum;
    }
    return oneD[d];
}

void vec2Print(const void* a){
    vec2 v = *(vec2*)a;
    vec2 origin = {50,50};
    double dystance = vec2Dyst(a, &origin, -1);
    printf("(%d %d) %.2lf", v.a[0], v.a[1], dystance);
}

void test2(){
    srand(time(NULL));
    vec2 l[30];
    for (int i = 0; i < 30; i++){
        l[i].a[0] = rand()%(99-10) + 10;
        l[i].a[1] = rand()%(99-10) + 10;
    }
    kdtree* t = createKdtree(l, 20, sizeof(vec2), 2, vec2Comp);

    vec2 closest;
    vec2 closestTo = {50,50};
    kdtreeFindClosest(&closest, &closestTo, t, vec2Dyst);
    vec2Print(&closest);
    printf("\n");

    kdtreePrint(t, vec2Print);
    freeKdtree(t);
}

int main(){
    // test1();
    test2();
    return 0;
}