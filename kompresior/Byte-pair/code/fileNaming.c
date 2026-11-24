#include<string.h>

void getFileNameFromPath(char* path){
    char* a = strrchr(path, '\\');
    char* b = strrchr(path, '/');
    if( b > a) a = b;
    a++;
    strcpy(path, a);
}

void addExtension(char* fileName){
    strcat(fileName, ".bp");
}

void removeExtension(char* fileName){
    char* findedaT = strrchr(fileName, '.');
    *findedaT = 0;
}