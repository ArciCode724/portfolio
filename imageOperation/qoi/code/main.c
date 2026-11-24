#include<stdio.h>
#include"../header/qoi.h"
#include"../header/spngio.h"
// #include<stdlib.h>
#include<string.h>

static int isInList(char** list, int numList, char* find){
    for (int i = 0; i < numList; i++){
        if(strcmp(list[i], find) == 0){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if(argc < 3){ 
        puts("potrzeba pliku in and out");
        return 1;
    }
    char* inFileName = argv[1];
    char* outFileName = argv[2];

    char* inType = strrchr(inFileName, '.');
    if(!inType){ 
        puts("brakuje typu w arg1");
        return 1; 
    }
    inType++;
    char* outType = strrchr(outFileName, '.');
    if(!outType){ 
        puts("brakuje typu w arg2");
        return 1; 
    }
    outType++;

    char* suportedType[2] = {"qoi", "png"};
    if(!isInList(suportedType, 2, inType)){
        printf("typ '%s' nie jest wspierany", inType);
        return 1;
    }
    if(!isInList(suportedType, 2, outType)){
        printf("typ '%s' nie jest wspierany", outType);
        return 1;
    }

    FILE* test = fopen(inFileName, "r");
    if(!test){
        puts("plik in nie istnieje");
        return 1;
    }
    fclose(test);

    // input testing end

    qoiFile* data = qoiFile_init();

    if(strcmp(inType, "qoi")==0){
        int err;
        err = qoi_decode(inFileName, data);
    }else if(strcmp(inType, "png")==0){
        spng_decode(inFileName, data);
    }

    if(strcmp(outType, "qoi")==0){
        qoi_encode(outFileName, data);
    }
    else if(strcmp(outType, "png")==0){
        spng_encode(outFileName, data);
    }

    qoiFile_free(data);
    puts("end");
    return 0;
}
