#include<stdio.h>
#include"byteList.h"
#include"kodeTable.h"

int main(int argc, char const *argv[]){
    FILE* testF = fopen(argv[1], "r");
    if(! testF){
        printf("brak zrodla");
        return 1;
    }

    struct{
        long readed;
        long size;
        int show;
    } fileReaded;
    fseek(testF, 0, SEEK_END);
    fileReaded.size = ftell(testF);
    fclose(testF);

    fileReaded.readed = 0;
    fileReaded.show = 0;
    for(long i= fileReaded.size; i>0; i/=10)
        // printf("%ld\n", i);
        fileReaded.show++;
    fileReaded.show = (fileReaded.show < 0) ? 0 : fileReaded.show;
     printf("%ld\n", fileReaded.size);
    

    int byteSize = 9;
    kodeTable* koder = kodeTable_init(1);
    // bitI* f = bitI_init("test.txt");
    bitI* f = bitI_init((char*)argv[1]);
    bitO* output = bitO_init(
        (argc > 2) ? (char*)argv[2] : "compres.txt"
    );
    BYTE b;
    byteList* wordingStirng = byteList_init();
    int lastIndex, curentIndex;
    while (bitI_getByte(f, &b) != EOF){
        fileReaded.readed++;
        printf("\r%*ld", fileReaded.show, fileReaded.readed);

        // printf("%c", b);
        byteList_addByte(wordingStirng, b);
        curentIndex = kodeTable_findIndex(koder, wordingStirng);
        if(curentIndex != -1){
            lastIndex = curentIndex;
            continue;
        }
        // printf("\n%d: %d", byteSize, koder->kode[lastIndex]);    
        bitO_putMulkiBits(output, koder->kode[lastIndex], byteSize);
        kodeTable_addText(koder, wordingStirng);
        if(koder->mn == koder->n)
            byteSize++;
        wordingStirng = byteList_init();
        byteList_addByte(wordingStirng, b);
        lastIndex = kodeTable_findIndex(koder, wordingStirng);
    }
    // printf("%d: %d\n", byteSize, koder->kode[lastIndex]);   
    bitO_putMulkiBits(output, koder->kode[lastIndex], byteSize);
    bitO_putMulkiBits(output, 0, 7);
    // printf("%d\n", byteSize);
    // getc(stdin);
    return 0;
}
