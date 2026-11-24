#include<stdio.h>
#include<stdlib.h>
#include"byteList.h"
#include"kodeTable.h"

int main(int argc, char const *argv[]){
    FILE* testF = fopen(argv[1], "r");
    if(! testF){
        printf("brak zrodla");
        return 1;
    }

    struct{
        double readed;
        double size;
        int show;
    } fileReaded;
    fseek(testF, 0, SEEK_END);
    fileReaded.size = 8*(double)ftell(testF);
    fclose(testF);

    fileReaded.readed = 0;
    fileReaded.show = -4;
    for(long i= (long)fileReaded.size; i>1; i/=10)
        // printf("%ld\n", i);
        fileReaded.show++;
    fileReaded.show = (fileReaded.show < 0) ? 0 : fileReaded.show;


    int byteSize = 9;
    kodeTable* koder = kodeTable_init(1);
    byteList** idKoder = malloc(sizeof(byteList*)*koder->mn);
    for (int i = 0; i < koder->n; i++)
        idKoder[i] = koder->text[i];
    
    bitI* intput = bitI_init((char*)argv[1]);
    // bitO* output = bitO_init("decompres.txt");
    bitO* output = bitO_init(
        (argc > 2) ? (char*)argv[2] : "decompres.txt"
    );
    byteList* wordingStirng = byteList_init();
    byteList* loadedByties = byteList_init();
    int loadedIndex = bitI_getMulkiBits(intput, byteSize);
    // int curentIndex, lastIndex;
    while (loadedIndex != EOF){
        fileReaded.readed+=byteSize;
        printf("\r%1.*lf", fileReaded.show, fileReaded.readed/fileReaded.size);

        loadedByties->n = 0;
        // printf("%d: %d\n", byteSize, loadedIndex);
        if(loadedIndex == koder->n){
            // printf("a\n");
            for(int i=0; i<wordingStirng->n;i++){
                byteList_addByte(loadedByties, wordingStirng->txt[i]);
            }
            byteList_addByte(loadedByties, wordingStirng->txt[0]);
        }else{
            for(int i=0; i<idKoder[loadedIndex]->n;i++){
                byteList_addByte(loadedByties, idKoder[loadedIndex]->txt[i]);
            }
        }
        // byteList_print(loadedByties, 1);
        // printf("\n");
        for(int i=0; i<loadedByties->n;i++){
            // printf("%c", loadedByties->txt[i]);
            bitO_putByte(output, loadedByties->txt[i]);
            byteList_addByte(wordingStirng, loadedByties->txt[i]);
            if(kodeTable_findIndex(koder, wordingStirng) != -1){
                continue;
            }
            // printf("%d ", koder->kode[lastIndex]);    
            if(kodeTable_addText(koder, wordingStirng)){
                idKoder = realloc(idKoder, sizeof(byteList*)*koder->mn);
            }
            idKoder[koder->n-1] = wordingStirng;
            wordingStirng = byteList_init();
            byteList_addByte(wordingStirng, loadedByties->txt[i]);
        }
        // printf("\n");
        if(koder->mn - koder->n == 1)
            byteSize++;
        loadedIndex = bitI_getMulkiBits(intput, byteSize);
    }
    printf("\r%1.*lf", fileReaded.show, 1.0);

    // printf("sucest");
    return 0;
}
