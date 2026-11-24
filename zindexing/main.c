#include<stdio.h>
#include<stdint.h>

void printBits(uint32_t a){
    int bytCount = 0;
    for(int i = 0; i < 32; i++){
        if(bytCount++ == 8){
            bytCount = 1;
            printf(" ");
        }
        printf("%d", (a>>31)&1);
        a<<=1;
    }
    printf("\n");
}

uint32_t inflate2d(uint32_t n){
    n = (n | n << 8) & 0x00ff00ff;
    n = (n | n << 4) & 0x0f0f0f0f;
    n = (n | n << 2) & 0x33333333;
    n = (n | n << 1) & 0x55555555;
    return n;
}

uint32_t inflate3d(uint32_t n){
    n = (n | n << 16) & 0xff0000ff;
    n = (n | n <<  8) & 0x0f00f00f;
    n = (n | n <<  4) & 0xc30c30c3;
    n = (n | n <<  2) & 0x49249249;
    return n;
}

int main(){
    printBits(inflate2d(0xffff));
    printBits(inflate3d(0xffff));
}