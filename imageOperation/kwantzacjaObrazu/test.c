#include<stdio.h>
#include<math.h>
#include<stdint.h>

// typedef uint8_t rgb[3];


int main(){
    double f = INFINITY;
    printf("%f\n", f);
    printf("%f\n", 1.0/0.0);
    printf("%d\n", f == 1.0/0.0);
    printf("%d\n", f > 1.0/0.0);
    printf("%d\n", f < 1.0/0.0);
    return 0;
}