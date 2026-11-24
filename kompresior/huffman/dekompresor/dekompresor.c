#include<stdio.h>
#include<stdlib.h>
#include"bitIo.h"
#include"translator.h"

int main(int argc, char** argv){
    if(argc<3){
        fprintf(stderr, "[!] brak argumentów\n");
        return 1;
    }

    FILE *ptr = fopen(argv[1],"rb");  // r for read, b for binary
    if(!ptr){
        fprintf(stderr, "[!] nie da sie\n");
        return 2;
    }
    fclose(ptr);
    bitI* file = bitI_init(argv[1]);
    translator* readLang = translator_init(file);

    for (int i = 0; i < readLang->n; i++){
        printf("%2x: %s\n", readLang->leter[i], readLang->inBits[i]);
    }
    
    bitO* writer = bitO_init(argv[2]);
    char bit = bitI_getBit(file);
    while (bit != '1')
        bit = bitI_getBit(file);

    bit = bitI_getBit(file);
    while(bit != EOF){
        if(translator_addBit(readLang, bit))
            bitO_putByte(writer, translator_getLeter(readLang));
        bit = bitI_getBit(file);
    }
    translator_free(readLang);
    bitI_close(file);
    bitO_close(writer);

    return 0;
}