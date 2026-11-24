#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>
#include"..\header\propability.h"
#include"..\header\bitIO.h"
#include"..\header\bounds.h"
#include"..\header\lineMem.h"

#define hightsBite64 0x8000000000000000

void printInBit(uint64_t a){
    for(int i=0; i<64;i++){
        printf("%lld", a>>63);
        a<<=1;
    }
    printf("\n");
}

uint64_t loadFromFile(bitI* in, int amount){
    uint64_t bufer = 0;
    int bitBuff;
    for(int i = amount; i>0; i--){
        bitBuff = bitI_getBit(in);
        if(bitBuff == EOF){
            bitBuff = 1;
            printf("A");
        }
        bufer = bufer<<1 | bitBuff;
    }
    return bufer;
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
    bitI* in = bitI_init(fileName);
    if(!in){
        printf("nie ma pliku");
        return 1;
    }
    FILE* out = fopen("deOut.txt", "wb");
        
    lineMem* memory = lineMem_init(0xffffffff); // 0xfffffff0 
    propLavels* propList = propLavels_init();
    uint64_t kodedMaseg = loadFromFile(in, 64);
    // printInBit(kodedMaseg);
    
    bounds* boundsBox = bounds_init();

    struct{
        uint8_t list[MAXKONTEKST];
        int n;
    } konteks;
    konteks.n = 0;

    uint16_t ct;
    uint16_t bitRemoves;
    for(int l=MAXLEVEL; l>=0; l--){
        prop* propFind = propLavels_findProp(propList, l, konteks.list, konteks.n);
        if(!propFind){
            continue;
        }
        ct = bounds_findZnak(boundsBox, propFind, kodedMaseg);
        uint32_t down, up, sum;
        prop_off(propFind, ct, &down, &up, &sum);
        // printf("%3d :%d %d %d\n", ct, down, up, sum);
        if(ct != EOP){
            // printf("%c",ct);
            fprintf(out, "%c", ct);
        }
        
        bounds_encode(boundsBox, propFind, ct);
        bitRemoves = bounds_removeSame(boundsBox);
        kodedMaseg = (kodedMaseg<<bitRemoves) | loadFromFile(in, bitRemoves);

        uint64_t save = kodedMaseg & hightsBite64;
        kodedMaseg<<=1;
        uint64_t newInfo = 0;
        for(int i = bounds_wrapUnderflow(boundsBox); i>0; i--){
            kodedMaseg<<=1;
            newInfo = newInfo<<1 | loadFromFile(in, 1);
        }
        kodedMaseg>>=1;
        kodedMaseg = kodedMaseg | save | newInfo;

        if(ct==EOP){
            continue;
        }
        l = MAXLEVEL+1;

        uint8_t remoud[MAXKONTEKST+1];
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

    return 0;
}