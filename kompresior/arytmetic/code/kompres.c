#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include"..\header\propability.h"
#include"..\header\bitIO.h"
#include"..\header\bounds.h"
#include"..\header\lineMem.h"

void printInBit(uint64_t a){
    for(int i=0; i<64;i++){
        printf("%lld", a>>63);
        a<<=1;
    }
    printf("\n");
}

void encode(bitO* out, bounds* boundsBox, prop* propTable, int16_t znak){
    uint32_t down, up, sum;
    prop_off(propTable, znak, &down, &up, &sum);
    // printf("%3d :%d %d %d\n", znak, down, up, sum);
    // printf("%c",znak);
    bounds_encode(boundsBox, propTable, znak);
    bounds_printWsplne(boundsBox, out);
    bounds_wrapUnderflow(boundsBox);
}

int main(int argc, char** argv){
    char* fileName = NULL;
    char c;
    while((c = getopt(argc, argv, "f:")) != -1){
        switch (c){
            case 'f':
                fileName = optarg;
                break;
            default:
                break;
        }
    }
    FILE* in = fopen(fileName, "rb");
    if(!in){
        printf("nie ma pliku");
        return 1;
    }
    bitO* out = bitO_init("out.txt");
        
    lineMem* memory = lineMem_init(0xffffffff); // 0xfffffff0 
    propLavels* propList = propLavels_init();
    bounds* boundsBox = bounds_init();

    struct{
        uint8_t list[MAXKONTEKST];
        int n;
    } konteks;
    konteks.n = 0;
    

    uint8_t ct;
    uint8_t remoud[MAXKONTEKST+1];
    while (fscanf(in, "%c", &ct) > 0){
        // for(int i = 0; i< konteks.n; i++){
        //     printf("%c", konteks.list[i]);
        // }
        // printf(";\n");

        for(int l = MAXLEVEL; l>=0; l--){
            prop* propFind = propLavels_findProp(propList, l, konteks.list, konteks.n);
            if(!propFind){
                continue;
            }
            uint32_t down, up, sum;
            prop_off(propFind, ct, &down, &up, &sum);
            if(down == up){
                encode(out, boundsBox, propFind, EOP);
            }else{
                encode(out, boundsBox, propFind, ct);
                break;
            }
        }
        // encode(out, boundsBox, propTable, ct);

        lineMem_add(memory, ct);
        if(lineMem_getSoonRemoveKontekst(memory, remoud, MAXKONTEKST+1)){
            propLavels_lower(propList, remoud, MAXKONTEKST+1);
        }
        propLavels_inkris(propList, ct, konteks.list, konteks.n);

        if(konteks.n != MAXKONTEKST){
            konteks.n++;
        }else{
            for(int i=1; i<konteks.n; i++){
                konteks.list[i-1] = konteks.list[i];
            }
        }
        konteks.list[konteks.n-1] = ct;
    }
    for(int l = MAXLEVEL; l>=0; l--){
        prop* propFind = propLavels_findProp(propList, l, konteks.list, konteks.n);
        uint32_t down, up, sum;
        prop_off(propFind, ct, &down, &up, &sum);
        encode(out, boundsBox, propFind, EOP);
    }


    bitO_putMulkiBits(out, 127, 8);
    printf("end\n");

    return 0;
}