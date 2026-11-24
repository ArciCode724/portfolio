#include"bitIO.h"
#include"window.h"

int main(int argc, char *argv[]){
    FILE* testF = fopen("compres.txt", "r");
    struct{
        long readed;
        long size;
        int show;
    } fileReaded;
    fseek(testF, 0, SEEK_END);
    fileReaded.size = 8*ftell(testF);
    fclose(testF);
    fileReaded.readed = 0;
    fileReaded.show = 0;
    for(long i= fileReaded.size; i>0; i/=10)
        // printf("%ld\n", i);
        fileReaded.show++;
    fileReaded.show = (fileReaded.show < 0) ? 0 : fileReaded.show;
    printf("%ld\n", fileReaded.size);

    bitI* in = bitI_init("compres.txt");
    bitO* out = bitO_init("decompres.txt");
    window* okon = window_init();

    while (1){
        char type = bitI_getBit(in);
        fileReaded.readed++;
        if(type == '0'){
            int b = bitI_getMulkiBits(in, 8);
            if(b == EOF)
            break;
            bitO_putMulkiBits(out, b, 8);
            okon->tasma[okon->center] = b;
            window_moveBy(okon, 1);
            fileReaded.readed+=8;
        }else{
            int index = bitI_getMulkiBits(in, 8*sizeof(uint));
            int len = bitI_getMulkiBits(in, 8*sizeof(uint));
            if(len == EOF)
                break;
            for(int i=0; i<len; i++){
                bitO_putMulkiBits(out, okon->tasma[okon->center-index+i], 8);
                okon->tasma[okon->center+i] = okon->tasma[okon->center-index+i];
                okon->right++;
            }
            window_moveBy(okon, len);
            fileReaded.readed+=16*sizeof(uint);
        }
        printf("\r%*ld", fileReaded.show, fileReaded.readed);
    }
    // printf("end\n");
    return 0;
}