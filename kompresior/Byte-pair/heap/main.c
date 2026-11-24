#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string.h>
#include"../head/tokens.h"
#include"../head/pairFreg.h"
#include"../head/entropy.h"

// sprawdza rownisz czy rozmiar jest odpowiedni do dalszego operowania
long getFileSize(const char* fileName){
    long fileSize = 0;
    FILE* in = fopen(fileName, "rb");
    fseek(in, 0, SEEK_END);
    fileSize = ftell(in);
    fclose(in);

    if(fileSize < 0){
        fprintf(stderr, "to Big File 1");
        exit(1);
    }
    if( SIZE_MAX/sizeof(token) < fileSize ){
        fprintf(stderr, "to Big File 2");
        exit(1);
    }

    return fileSize;
}

void printTokens(pair* tokenDef, int tokenDefSize){
    int stosNum = 0;
    token* stos = malloc(sizeof(token)*(tokenDefSize+1));

    FILE* out = fopen("token.txt", "wb");

    // +256 poniewasz w tokenDef nie ma zapisanego 0-255 id
    for(int i = 0; i < tokenDefSize; i++){
        stosNum = 2;
        stos[0] = tokenDef[i].r;
        stos[1] = tokenDef[i].l;
        fprintf(out, "%5d: \"", i+256);
        while(stosNum > 0){
            stosNum--;
            if(stos[stosNum] < 256){
                // if(32 <= stos[stosNum] && stos[stosNum] <= 127) fprintf(out, "%c", stos[stosNum]);
                // else fprintf(out, "%02X",stos[stosNum]);
                fprintf(out, "%c", stos[stosNum]);
            }else{
                int tokenId = stos[stosNum] - 256;
                stos[stosNum++] = tokenDef[tokenId].r;
                stos[stosNum++] = tokenDef[tokenId].l; 
            }
        }
        fprintf(out, "\"\n");
    }

    fclose(out);
    free(stos);
}

void compres(const char* fileName){
    long fileSize = getFileSize(fileName);
    token* fileContent = malloc(sizeof(token)*fileSize);

    // lading file to memory
    FILE* in = fopen(fileName, "rb");
    for(long i = 0; !feof(in) ; i++) fileContent[i] = getc(in);
    fclose(in);
    // for(long i = 0; i < fileSize ; i++) printf("%d ", fileContent[i]);
    // printf("\n");

    pairFreg* freg = pairFreg_malloc();
    for(size_t i = 0; i + 1 < fileSize; i++){
        pair toAdd = {.l = fileContent[i], .r = fileContent[i+1]};
        pairFreg_fregUpPair(freg, toAdd);
    }
    // pairFreg_print(freg);

    const int maxNewTokenNum = (1<<(8*sizeof(token)))-256; // 32768
    pair* tokenDef = malloc(sizeof(pair)*maxNewTokenNum);
    token tokenDefSize = 0;

    const int minFreg = 3;
    for(int t = 0; t < maxNewTokenNum; t++){
        pairFreg_pruneHeap(freg, minFreg);

        // int noFrag = 0;
        // for(int i = 0; i<freg->heapFill; i++) if(freg->heap[i].val < minFreg && freg->heap[i].val != -1) noFrag++;

        fregEntry entry = pairFreg_mostOftenPair(freg);
        printf("%6d : %6d;\r", t, entry.val);
        // printf("size %lf\n", fileSize*entropyOfText(fileContent, fileSize));
        if(entry.val < minFreg) break;
        int newTokenId = tokenDefSize+256; // 256 to rozmiar poczatkowego alfabetu
        tokenDef[tokenDefSize++] = entry.key;
        
        pair newToken = entry.key;
        // printf("newToken: %d %d\r", newToken.l, newToken.r);
        
        // replac
        // pairFreg_removePair(freg, newToken);
        int newFileSize = 0;
        for(int i = 0; i < fileSize; i++){
            // printf("%d: ", i);
            // for(long i = 0; i < fileSize ; i++) printf("%d ", fileContent[i]);
            // printf("\n");
            // pairFreg_print(freg);
            if(i + 1 < fileSize && fileContent[i] == newToken.l && fileContent[i+1] == newToken.r){
                if(i > 0){
                    pairFreg_fregUpPair(freg, (pair){.l = fileContent[newFileSize-1], .r = newTokenId});

                    pairFreg_fregDownPair(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]});
                }
                pairFreg_fregDownPair(freg, newToken);

                fileContent[newFileSize++] = newTokenId;
                i++;
            }else{
                if(newFileSize > 0 && fileContent[newFileSize-1] == newTokenId){
                    pairFreg_fregUpPair(freg, (pair){.l = newTokenId, .r = fileContent[i]});

                    pairFreg_fregDownPair(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]});
                }
                fileContent[newFileSize++] = fileContent[i];
            }
        }
        fileSize = newFileSize;

        // printf("%lld/%lld = %lf\n", freg->fill, freg->size, (double)freg->fill/freg->size);
        // for(long i = 0; i < fileSize ; i++) printf("%d ", fileContent[i]); 
        // printf("\n");
        // pairFreg_print(freg);
        // break;
    }
    printf("\n");
    // for(long i = 0; i < fileSize ; i++) printf("%d ", fileContent[i]); 
    // printf("\n");
    // pairFreg_print(freg);
    // printf("%ld/%ld", freg->fill, freg->size);
    printTokens(tokenDef, tokenDefSize);

    printf("entropy %lf\n", entropyOfText(fileContent, fileSize));

    // char outFileName[strlen(fileName)+3];
    // strcpy(outFileName, fileName);
    // strcat(outFileName, ".bp");
    // printf("out: %s\n", outFileName);
    // FILE* out = fopen(outFileName, "wb");
    FILE* out = fopen("comp.bp", "wb");

    
    fwrite(&tokenDefSize, sizeof(token), 1, out);
    fwrite(tokenDef, sizeof(pair), tokenDefSize, out);
    fwrite(fileContent, sizeof(token), fileSize, out);
    fclose(out);

    
    pairFreg_free(freg);
    free(tokenDef);
    free(fileContent);
}

void deCompres(const char* fileName){
    FILE* in = fopen(fileName, "rb");
    FILE* out = fopen("decomp.txt", "wb");

    token tokenDefSize;
    fread(&tokenDefSize, sizeof(token), 1, in);
    // printf("%d\n", tokenDefSize);
    pair* tokenDef = malloc(sizeof(pair)*tokenDefSize);
    fread(tokenDef, sizeof(pair), tokenDefSize, in);
    // for(int i = 0; i < tokenDefSize; i++){
    //     printf("%d %d\n", tokenDef[i].l, tokenDef[i].r);
    // }

    int stosNum = 0;
    token* stos = malloc(sizeof(token)*(tokenDefSize+1));

    while(1){
        stosNum = 1;
        fread(stos, sizeof(token), 1, in);
        if(feof(in)) break;
        printf("%d ", stos[0]);
        while(stosNum > 0){
            stosNum--;
            if(stos[stosNum] < 256){
                uint8_t toWrite = stos[stosNum];
                // printf("%c", toWrite);
                fwrite(&toWrite, 1, 1, out);
            }else{
                int tokenId = stos[stosNum] - 256;
                stos[stosNum++] = tokenDef[tokenId].r;
                stos[stosNum++] = tokenDef[tokenId].l; 
            }
        }
    }

    free(stos);
    fclose(in);
    fclose(out);
    free(tokenDef);
}

int main(int argc, const char** argv){
    if(argc < 3){
        fprintf(stderr, "za malo argumentow");
        exit(1);
    }
    const char* fileName = argv[2];

    if(strcmp(argv[1], "-c") == 0){
        compres(fileName);
    }else if(strcmp(argv[1], "-d") == 0){
        deCompres(fileName);
    }
    printf("\nend");
}