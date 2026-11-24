#include<stdio.h>
#include"window.h"
#include"bitIO.h"

int main(int argc, char *argv[]){
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

    bitI* in = bitI_init(argv[1]);
    bitO* out = bitO_init("compres.txt");
    window* okno = window_init();
    window_fillQue(okno, in);
    // printf("%d", makeFinderIndex(okno, okno->center+1));
    uint index,len;
    while (okno->right > 0){
        window_find(okno, &index, &len);
        if(index==0){
            bitO_putBit(out, '0');
            bitO_putMulkiBits(out, okno->tasma[okno->center], 8);
            // printf("%c", okno->tasma[okno->center]);
            window_moveBy(okno, 1);
            fileReaded.readed+=1;
        }else{
            bitO_putBit(out, '1');
            bitO_putMulkiBits(out, index, 8*sizeof(uint));
            bitO_putMulkiBits(out, len, 8*sizeof(uint));
            window_moveBy(okno, len);
            // printf(" <%d:%d> ", index, len);
            fileReaded.readed+=len;
        }
        window_fillQue(okno, in);

        printf("\r%*ld", fileReaded.show, fileReaded.readed);
    }
    bitO_putMulkiBits(out, 0, 7);
    
    printf("\n");
    // for(int i=0; i< okno->quickFinder.n; i++){
    //     if(okno->quickFinder.kombinacje[i].n == 0)
    //     continue;
    //     printf("%x: ", i);
    //     for (int j=0; j < okno->quickFinder.kombinacje[i].n; j++)
    //     printf("%d ", okno->quickFinder.kombinacje[i].list[j]+1-okno->quickFinder.ofset); // +1-okno->quickFinder.ofset
    //     printf("\n");
    // }

    printf("end\n");
    return 0;
}