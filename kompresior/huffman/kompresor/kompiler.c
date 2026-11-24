#include<stdio.h>
#include<stdlib.h>
#include"kompiler.h"
#include"couter.h"
#include"tree.h"
#include"translator.h"
#include"bitIo.h"

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

    couter* fileCouter = couter_init();
    couter_count(fileCouter, argv[1]);
    couter_qsort(fileCouter, 0, fileCouter->n);
    couter_remowEmpy(fileCouter);
    printf("entropia pliku: %lf\n", couter_entory(fileCouter));
    // couter_print(fileCouter);

    tree* entroTree = couter_to_tree(fileCouter);
    // printf("\n");
    tree_print(entroTree, 0);
    // printf("%d", tree_size(entroTree));
    // printf("\n");

    translator* howTranslat = tree_to_translator(entroTree);
    translator_print(howTranslat);

    bitO* write = bitO_init(argv[2]);
    tree_save(entroTree, write);
    tree_free(entroTree);

    double extra0 = 8 - ((int)couter_newLen(fileCouter, howTranslat) + 1) %8;
    couter_free(fileCouter);
    for(int i=0; i<extra0; i++)
        bitO_putBit(write,'0');
    bitO_putBit(write,'1');

    BYTE c;
    bitI* reader = bitI_init(argv[1]);
    while (bitI_getByte(reader, &c) != 0){
        char* key = howTranslat->inBits[c];
        for(;*key;key++){
            bitO_putBit(write, *key);
        }
    }
    bitO_close(write);

    return 0;
}