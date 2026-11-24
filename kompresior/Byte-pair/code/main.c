#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#include"../head/tokens.h"
#include"../head/pairFreg.h"
#include"../head/entropy.h"
#include"../head/ariCoding.h"

// sprawdza rownisz czy rozmiar jest odpowiedni do dalszego operowania
long getFileSize(const char* fileName){
    long fileSize = 0;
    FILE* in = fopen(fileName, "rb");
    fseek(in, 0, SEEK_END);
    fileSize = ftell(in);
    fclose(in);

    if(fileSize < 0){
        fprintf(stderr, "to Big File");
        exit(1);
    }
    if( SIZE_MAX/sizeof(token) < fileSize ){
        fprintf(stderr, "to Big File");
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

int recountPair(token* text, long textSize, pair toCount){
    int retVal = 0;
    for(long i = 1; i < textSize; i++){
        if(text[i]   != toCount.r) continue;
        if(text[i-1] != toCount.l) continue;
        retVal++;
        i++;
    }
    return retVal;
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
    entropy_ctx textEntropt;
    init_entropyCtx(&textEntropt, fileContent, fileSize);

    // printf("1entropy %lf\n", entropyOfText(fileContent, fileSize));
    // pairFreg_print(freg);

    const int maxNewTokenNum = (1<<(8*sizeof(token)))-256-1; // 32768
    pair* tokenDef = malloc(sizeof(pair)*maxNewTokenNum);
    token tokenDefSize = 0;

    double oldSize = 2*tokenDefSize + fileSize*entropyOfText(fileContent, fileSize)/8;
    double newSize = 0;

    const int minFreg = 4;
    for(int t = 0; t < maxNewTokenNum; t++){
        pairFreg_checkBound(freg, minFreg);

        size_t pos = pairFreg_mostOftenPair(freg);
        printf("%6d : %6d; %7lld; \r", t, freg->vals[pos], freg->fill);
        
        int newTokenCound = freg->vals[pos];
        if(newTokenCound < minFreg) break;
        int newTokenId = tokenDefSize+256; // 256 to rozmiar poczatkowego alfabetu
        tokenDef[tokenDefSize++] = freg->keys[pos];
        
        pair newToken = freg->keys[pos];
        // printf("new tok %d %d\n", newToken.l, newToken.r);

        // printf("new Token: %3d %3d; ", newToken.l, newToken.r);
        // printf("%d ->", newTokenCound);
        // if(newToken.l == newToken.r) newTokenCound = recountPair(fileContent, fileSize, newToken);
        // printf("%d\n", newTokenCound);
        // printf("1entropy %f\n", predictEntropy(&textEntropt, newToken, newTokenCound));
        // updateEntropyCtx(&textEntropt, newToken, newTokenCound, newTokenId);
                
        // replac
        pairFreg_removePair(freg, newToken);
        int newFileSize = 0;
        for(int i = 0; i < fileSize; i++){
            // printf("\r                        \r%6d a", t);
            if(i + 1 < fileSize && fileContent[i] == newToken.l && fileContent[i+1] == newToken.r){
                if(i > 0){
                    pairFreg_fregUpPair(freg, (pair){.l = fileContent[newFileSize-1], .r = newTokenId});

                    pairFreg_fregDownPairMin(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]}, minFreg);
                    // pairFreg_fregDownPair(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]});
                }
                // pairFreg_fregDownPair(freg, newToken);

                fileContent[newFileSize++] = newTokenId;
                i++;
            }else{
                if(newFileSize > 0 && fileContent[newFileSize-1] == newTokenId){
                    pairFreg_fregUpPair(freg, (pair){.l = newTokenId, .r = fileContent[i]});

                    pairFreg_fregDownPairMin(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]}, minFreg);
                    // pairFreg_fregDownPair(freg, (pair){.l = fileContent[i-1], .r = fileContent[i]});
                }
                fileContent[newFileSize++] = fileContent[i];
            }
            // printf("b");
        }
        fileSize = newFileSize;

        // printf("2entropy %lf\n", textEntropt.entropy);
        // printf("3entropy %lf\n\n", entropyOfText(fileContent, fileSize));
        // newSize = 2 + 2*tokenDefSize + fileSize*entropyOfText(fileContent, fileSize)/8;
        // printf("size change: %lf\n", oldSize - newSize);
        // oldSize = newSize;


        // break;
    }
    printf("\n");
    printTokens(tokenDef, tokenDefSize);

    // printf("1entropy %lf\n", textEntropt.entropy);
    printf("size %lf\n", 2 + tokenDefSize*2 + fileSize*entropyOfText(fileContent, fileSize)/8);

    FILE* out = fopen("comp.bp", "wb");

    fwrite(&tokenDefSize, sizeof(token), 1, out);
    fwrite(tokenDef, sizeof(pair), tokenDefSize, out);

    ariCtxEncode* ectx = ari_encodingContex(out);
    for(long i = 0; i < fileSize; i++){
        ari_encodeToken(ectx, fileContent[i]);
    }
    ari_encodeToken(ectx, END_OF_CODE);
    ari_flushEncode(ectx);
    free(ectx);
    // fwrite(fileContent, sizeof(token), fileSize, out);


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
    printf("%d\n", tokenDefSize);
    pair* tokenDef = malloc(sizeof(pair)*tokenDefSize);
    fread(tokenDef, sizeof(pair), tokenDefSize, in);
    // for(int i = 0; i < tokenDefSize; i++){
    //     printf("%d %d\n", tokenDef[i].l, tokenDef[i].r);
    // }

    int stosNum = 0;
    token* stos = malloc(sizeof(token)*(tokenDefSize+1));

    ariCtxDecode* dctx = ari_decodingContex(in);

    while(1){
        stosNum = 1;

        stos[0] = ari_decodeToken(dctx);
        if(stos[0] == END_OF_CODE) break;
        // fread(stos, sizeof(token), 1, in);
        // if(feof(in)) break;

        // printf("%d %d\n", stos[0], END_OF_CODE);
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
    free(dctx);

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

    clock_t start = clock();

    if(strcmp(argv[1], "-c") == 0){
        compres(fileName);
    }else if(strcmp(argv[1], "-d") == 0){
        deCompres(fileName);
    }

    printf("time: %ld\n", (clock()-start)/CLOCKS_PER_SEC );

    printf("end");
}