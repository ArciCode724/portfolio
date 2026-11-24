#include<stdio.h>
#include<complex.h>

int main(int argc, char const *argv[])
{
    struct{
        int aa;
    } *aa;
    printf("%u\n", aa);
    aa += 1;
    printf("%u\n", aa);
    aa += 2;
    printf("%u\n", aa);
    double complex b = 1;
    for(int i =0; i< 10; i++){
        double complex a = 2*cexp(I * 6.28 * i/10);
        printf("%f %f %f\n", creal(a), cimag(a), cabs(a)); 
        b *= a;
        printf("%f %f %f\n\n", creal(b), cimag(b), cabs(b));
    }
    return 0;
}
