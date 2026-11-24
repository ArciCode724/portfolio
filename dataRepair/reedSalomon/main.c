#include<stdio.h>
#include".\gf256.h"
#include".\poly.h"

void printInBin(int number, int len){
    int printPos = 1<<(len-1);
    // printf("%d %d\n", numLen, printPos);
    for (int i = 0; i < len; i++){
        printf("%d", (number&printPos) != 0);
        printPos>>=1;
    }
    printf("\n");
}

void protectData(poli* data, int protectNum){
    poliResize(data, data->deg+protectNum);
    for (int i = 0; i <= data->deg-protectNum; i++){
        data->coef[i+protectNum] = data->coef[i];
        data->coef[i] = 0;
    }
    
    poli generator = mallocPoli(0);
    generator.coef[0] = 1;
    poli mutiBy = mallocPoli(1);
    mutiBy.coef[1] = 1;
    for (int i = 0; i < protectNum; i++){
        mutiBy.coef[0] = galoisPow2(i);
        poliMulti(generator, mutiBy, &generator);
    }

    poli outRest = mutiBy;

    poliDivis(*data, generator, NULL, &outRest);

    for (int i = 0; i <= outRest.deg; i++){
        data->coef[i] = outRest.coef[i];
    }
    
    freePoli(generator);
    freePoli(outRest);
}

void extendedEuclidean(poli syndrome, int protectNum, poli* outErrorLocator, poli* outValueLocator){
    if(protectNum/2 == 0){ return;}
    poli p1 = mallocPoli(syndrome.deg+1);
    p1.coef[p1.deg] = 1;
    poli p2 = mallocPoli(syndrome.deg);
    for (int i = 0; i <= syndrome.deg; i++){
        p2.coef[i] = syndrome.coef[i];
    }

    poli divProd = mallocPoli(0);

    poli a1 = mallocPoli(0);
    a1.coef[0] = 0;
    poli a2 = mallocPoli(0);
    a2.coef[0] = 1; 
    
    do{
        poliDivis(p1, p2, &divProd, &p1);

        poliMulti(divProd, a2, &divProd);
        poliMinus(divProd, a1, &a1);
        
        poli temp = p1;
        p1 = p2;
        p2 = temp;

        temp = a1;
        a1 = a2;
        a2 = temp;
    } while (p2.deg >= protectNum/2);

    // printPoli(p2, '\n');
    poliResize(outValueLocator, p2.deg);
    for (int i = 0; i <= p2.deg; i++){
        outValueLocator->coef[i] = p2.coef[i];
    }

    // printPoli(a2, '\n');
    poliResize(outErrorLocator, a2.deg);
    for (int i = 0; i <= a2.deg; i++){
        outErrorLocator->coef[i] = a2.coef[i];
    }

    freePoli(p1);
    freePoli(p2);
    freePoli(divProd);
    freePoli(a1);
    freePoli(a2);
}

// 1 - udalo sie; 0 - nie da sie naprawic
int repairData(poli* data, int protectNum){
    poli syndrome = mallocPoli(protectNum-1);
    for (int i = 0; i < protectNum; i++){
        syndrome.coef[i] = poliValuAt(*data, galoisPow2(i));
    }
    poliShorten(&syndrome);
    printPoli(syndrome, '\n');
    if(syndrome.deg == 0 && syndrome.coef[0] == 0){
        return 1;
    }
    printf("\n");

    poli error = mallocPoli(protectNum/2);
    poli value = mallocPoli(protectNum/2);
    extendedEuclidean(syndrome, protectNum, &error, &value);
    printPoli(value, '\n');
    printPoli(error, '\n');

    uint8_t areErrors = 0;
    uint8_t repairAt[data->deg + 1];
    for (int i = 0; i <= data->deg; i++){
        if(poliValuAt(error, galoisPow2(-i)) == 0){
            repairAt[i] = 1;
            areErrors = 1;
        }else{
            repairAt[i] = 0;
        }
    }
    if(areErrors == 0){
        return 0;
    }

    // pohodan error
    for (int i = 0; i <= error.deg; i+=2){
        error.coef[i] = 0;
    }
    for (int i = 1; i <= error.deg; i++){
        error.coef[i-1] = error.coef[i];
    }
    error.deg--;
    poliShorten(&error);

    // naprawianie
    for (int i = 0; i <= data->deg; i++){
        if(repairAt[i] == 0){
            continue;
        }
        uint8_t repair = galoisPow2(i);
        repair = galoisMulti(repair, poliValuAt(value, galoisPow2(-i)) );
        repair = galoisDivis(repair, poliValuAt(error, galoisPow2(-i)) );
        printf("%d %d; ", i, repair);
        data->coef[i] = galoisAdd(data->coef[i], repair);
    }
    printf("\n");

    freePoli(syndrome);
    freePoli(error);
    freePoli(value);

    return 1;
}

int main(int argc, char const *argv[]){
    uint8_t data[] = {'a','a'};
    poli dataP = mallocPoli(1);
    for (int i = 0; i < 2; i++){
        dataP.coef[i] = data[i];
    }
    protectData(&dataP, 4);
    printPoli(dataP, '\n');
    for (int i = 0; i < 6; i++){
        printf("%d: %d; ", galoisPow2(i), poliValuAt(dataP, galoisPow2(i)) );
    }
    putchar('\n');

    // dataP.coef[2] = 0;
    // dataP.coef[1] = 0;
    dataP.coef[2] = 0;
    // printPoli(dataP, '\n');

    printf("%d\n", repairData(&dataP, 4));

    printPoli(dataP, '\n');

    // printGaloisMutiCtx();

    return 0;
}
