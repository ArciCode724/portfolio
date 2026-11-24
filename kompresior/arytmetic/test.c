#include<stdio.h>
#include"./header/propability.h"
#include"./header/lineMem.h"

int main(){

    uint8_t test[20];
    for(int i=0;i<10;i++){
        test[i] = i;
        test[10+i] = i;
    }
    propLavels* testList = propLavels_init();
    lineMem* lineTest = lineMem_init(100);
    struct {
        uint8_t list[2];
        int n;
    } konteks;
    konteks.n = 0;
    
    uint8_t ct;
    for(int i=0; i<20; i++){
        ct = test[i];

        // for(int j=0; j<konteks.n; j++){
        //     printf("%2d ", konteks.list[j]);
        // }
        // printf("%2d ", ct);
        propLavels_inkris(testList, ct, konteks.list, konteks.n);
        
        lineMem_add(lineTest, ct);
        uint8_t remoud[3];
        if(lineMem_getSoonRemoveKontekst(lineTest, remoud, 3)){
            for(int i=0;i<3;i++){
                printf("%2d ", remoud[i]);
            }
            printf("\n");
            propLavels_lower(testList, remoud, 3);
        }
        for(int i=1; i<konteks.n; i++){
            konteks.list[i-1] = konteks.list[i];
        }
        konteks.n += (konteks.n != 2);
        konteks.list[konteks.n-1] = ct;

    }
    printf("\n");
    konteks.list[0] = 5;
    konteks.list[1] = 6;
    prop* propFind = propLavels_findProp(testList, 3, konteks.list);
    for(int i=0; i<21; i++){
        printf("%3d: %2d %2d\n", i, propFind->steps[i], propFind->steps[i+1]);
    }
    printf("%3d: %2d %2d\n", 256, propFind->steps[256], propFind->steps[256+1]);
    // uint32_t down, up, sum;
    // prop_off(propFind, 3, &down, &up, &sum);
    // printf("%d %d %d\n", down, up, sum);

    return 0;
}