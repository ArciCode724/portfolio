#include<stdio.h>
#include<string.h>
#include<stdint.h>

// ile bitow do precyzji prawdopodobienstwa
#define PROBBITS 12

void makePropability(const char* fileName, uint32_t licznik[256]){
    for(int i = 0; i < 256; i++) licznik[i] = 0;
    FILE* in = fopen(fileName, "rb");
    int byte;
    int size = 0;
    while ( (byte = fgetc(in)) != EOF){
        licznik[byte]++;
        size++;
    }
    fclose(in);
    printf("size %d\n", size);

    int newSize = 0;
    for(int i = 0; i < 256; i++){
        // printf("%d\n", licznik[i]<<PROBBITS);
        int noZero = licznik[i] != 0;
        licznik[i] = (licznik[i]<<PROBBITS)/size;
        if(noZero && licznik[i] == 0) licznik[i] = 1;
        newSize += licznik[i];
    }
    for(int i = 0; newSize != 1<<PROBBITS && i < 256; i++){
        if(licznik[i] == 0) continue;
        int dif = (1<<PROBBITS) - newSize;
        newSize -= licznik[i];
        licznik[i] += dif;
        if(licznik[i] <= 0) licznik[i] = 1;
        newSize += licznik[i];
    }
}

void compres(const char* fileName){
    uint32_t prob[256];
    makePropability(fileName, prob);
    int cdf[256];
    cdf[0] = 0;
    for (int i = 1; i < 256; i++) cdf[i] = cdf[i-1] + prob[i-1];
    
    FILE* in = fopen(fileName, "rb");
    FILE* out = fopen("comp.rans", "wb");

    uint32_t bufer32[1];
    uint16_t bufer16[1];
    for (int i = 0; i < 256; i++){
        bufer16[0] = prob[i];
        fwrite(bufer16, sizeof(uint16_t), 1, out);
    }
    for(int i = 0; i < 256; i++){
        if(prob[i] == 0) continue;
        printf("%3d %3d %3d\n", i, prob[i], cdf[i]);
    }


    // uint32_t x = 0;
    uint32_t x = (1<<16) - 1;
    int byte;
    fseek(in, 0, SEEK_END);
    while(ftell(in) != 0){
        fseek(in, -1, SEEK_CUR);
        byte = fgetc(in);
        fseek(in, -1, SEEK_CUR);

        if( x >= prob[byte]<<(32-PROBBITS) ){
            // printf("\n");
            bufer16[0] = x;
            fwrite(bufer16, 2, 1, out);
            x >>= 16;
        }
        x = ((x/prob[byte])<<PROBBITS) + (x%prob[byte]) + cdf[byte];
        // x++;
        // printf("%u ", x);
    }
    

    bufer32[0] = x;
    // printf("\n%d ", bufer[0]);
    fwrite(bufer32, 4, 1, out);

    fclose(in);
    fclose(out);
}

void deCompres(const char* fileName){
    FILE* in = fopen(fileName, "rb");

    uint32_t bufer32[1];
    uint16_t bufer16[1];

    int prob[256];
    for (int i = 0; i < 256; i++){
        fread(bufer16, sizeof(uint16_t), 1, in);
        prob[i] = bufer16[0];
    }
    int cdf[256];
    cdf[0] = 0;
    for (int i = 1; i < 256; i++) cdf[i] = cdf[i-1] + prob[i-1];
    
    for(int i = 0; i < 256; i++){
        if(prob[i] == 0) continue;
        printf("%3d %3d %3d\n", i, prob[i], cdf[i]);
    }

    printf("reded: %d\n", ftell(in));
    
    uint8_t decodeS[1<<PROBBITS];
    int a=0;
    for (int i = 0; i < 256; i++){
        for(int j = 0; j < prob[i]; j++) decodeS[a++] = i;
    }

    int toRead = ftell(in);
    fseek(in, 0, SEEK_END);
    toRead = ftell(in) - toRead;
    
    FILE* out = fopen("deComp.txt", "wb");
    
    fseek(in, -4, SEEK_CUR);
    fread(bufer32, 4, 1, in);
    fseek(in, -4, SEEK_CUR);
    toRead -= 4;
    // printf("%x ", bufer[0]);

    uint32_t x = bufer32[0];
    int mask = (1<<PROBBITS)-1;
    while(x >= (1<<16)){
    // while(x > 0){
        // x--;
        uint8_t s = decodeS[x & mask];
        // printf("%u ", x);
        x = prob[s]*(x>>PROBBITS) + (x & mask) - cdf[s];
        fputc(s, out);
        // printf("%c",s);
        if(toRead > 0 && x < (1<<16)){
            fseek(in, -2, SEEK_CUR);
            fread(bufer16, 2, 1, in);
            fseek(in, -2, SEEK_CUR);
            toRead -= 2;
            // printf("%x ", bufer[0]);

            x = (x<<16) + bufer16[0];
        }
    }


    fclose(in);
    fclose(out);
}

int main(int argc, const char** argv){
    if(argc < 3) return 1;
    const char* fileName = argv[2];

    if(strcmp(argv[1], "-c") == 0){
        compres(fileName);
    }else if(strcmp(argv[1], "-d") == 0){
        deCompres(fileName);
    }
    printf("end");
}