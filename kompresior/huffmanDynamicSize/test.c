#include<stdio.h>
#include<string.h>
#include"charlist.h"

int countOcurenc(char* fileName, charList* word){
    if(word->n == 0)
        return 0;
    int retVal = 0;
    FILE* f = fopen(fileName, "rb");

    charList* buffer = charList_init();

    char c;
    // ładujmey poczatek
    for (int i = 0; i < word->n; i++){
        if(fscanf(f, "%c", &c) == EOF)
            return 0;
        charList_addChar(buffer, c);
    }
    while (1){
        if(charList_comp(word, buffer))
            retVal++;
        charList_popFrontChar(buffer);
        if(fscanf(f, "%c", &c) == EOF)
            break;
        charList_addChar(buffer, c);
    }

    charList_free(buffer);
    return retVal;
}

void findKeys(){
    // char* fileName = "PRZY.pdf";
    char* fileName = "test.txt";
    FILE* f = fopen(fileName, "rb");
    charList* findWord = charList_init();
    char c;
    fscanf(f, "%c", &c);
    charList_addChar(findWord, c);
    int lastScore = countOcurenc(fileName, findWord);
    int curentScore;
    printf("%d " , lastScore);
    while (1){
        if(fscanf(f, "%c", &c) == EOF)
            break;
        charList_addChar(findWord, c);
        curentScore = countOcurenc(fileName, findWord);
        printf("%d " , curentScore * findWord->n);
        if(1==curentScore)
            break;
        lastScore = curentScore;
    }
    charList_popChar(findWord);
    printf("\n%d: ", findWord->n);
    charList_print(findWord);
}

int main(){
    findKeys();

    return 0;
}