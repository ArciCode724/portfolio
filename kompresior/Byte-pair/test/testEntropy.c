// #include"../head/entropy.h"
#include"stdio.h"
#include"../code/entropy.c"

int main(){
    int textSize = 5;
    token text[] = {0,0,2,2,3};
    printf("entropy %lf\n", entropyOfText(text, textSize));
}