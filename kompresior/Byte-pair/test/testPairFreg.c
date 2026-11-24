#include<stdio.h>
#include"../code/pairFreg.c"

int main(){
    pairFreg* freg = pairFreg_malloc();
    // pairFreg_print(freg);
    pairFreg_fregUpPair(freg, (pair){.l = 0, .r = 5});
    pairFreg_fregUpPair(freg, (pair){.l = 1, .r = 2});
    pairFreg_fregUpPair(freg, (pair){.l = 0, .r = 0});
    pairFreg_fregUpPair(freg, (pair){.l = 0, .r = 0});
    pairFreg_fregUpPair(freg, (pair){.l = 2, .r = 7});
    pairFreg_fregUpPair(freg, (pair){.l = 2, .r = 7});
    pairFreg_print(freg);

    pairFreg_mostOftenPair(freg);

    // pairFreg_fregDownPairMin(freg, (pair){.l = 0, .r = 0}, 2);
    pairFreg_print(freg);
}