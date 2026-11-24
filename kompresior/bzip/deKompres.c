#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"rle.h"
#include"bitIO.h"
#include"bws.h"
#include"mtf.h"
#include"huffman.h"


#define BLOCK_SIZE 8;

// -f file
// -s block size

// 0 end of block
// 1-255 byte type
// 256 R0
// 257 R1

typedef struct{
    uint8_t * text;
    uint32_t n;
} block;

block* block_init(uint32_t size){
    block* retVal = malloc(sizeof(block));
    retVal->text = malloc(size);
    retVal->n = 0;
    return retVal;
}

int main(int argc, char** argv){
    char* fileName;
    char c;
    while((c = getopt(argc, argv, "f:")) != -1){
        switch (c){
            case 'f':
                fileName = optarg;
                break;
            default:
                break;
        }
    }
    bitI* in = bitI_init(fileName);
    if(!in){
        printf("nie ma pliku");
        return 1;
    }
    FILE* out = fopen("deOut.txt", "wb");

    uint32_t blockSize = bitI_getMulkiBits(in, 32);
    // blockSize = 0x0fffffff;
    block* loadePart = block_init(blockSize);
    rleText* rleBlock = rleText_init(blockSize);
    bitList* koding = CanonicalHuffman_innit();

    // fillBlock(in, loadePart, blockSize);
    // // printf("%u\n", loadePart->n);
    uint32_t origin;
    kodeTree* codeRoot;
    while ((origin = bitI_getMulkiBits(in, 32)) != EOF){
        static int blockNum = 0;
        printf("\r%d", ++blockNum);

        readCanonicalHuffman(in, koding);

        // koding_print(koding);
        // printf("\n");

        codeRoot = CanonicalHuffmanToTree(koding);
        // kodeTree_print(codeRoot, 0);
        uint16_t buf16;
        buf16 = readInCanonicalHuffman(in, codeRoot);
        rleBlock->n = 0;
        while (buf16 != EOB){
            rleBlock->text[rleBlock->n] = buf16;
            rleBlock->n++;
            // printf("%2x ", buf16);
            buf16 = readInCanonicalHuffman(in, codeRoot);
        }

        // if(blockNum == 16 || blockNum == 32){
        //     for(uint32_t i = 0; i<rleBlock->n; i++){
        //         printf("%d ", rleBlock->text[i]);
        //     }
        //     printf("\n");
        // }

        // printf("\n");
        loadePart->n = unRleZeros(loadePart->text, rleBlock);
        // for(int i=0; i<loadePart->n; i++){
        //     printf("%2x ", loadePart->text[i]);
        // }
        // printf("\n");
        unMoveToFront(loadePart->text, loadePart->n);
        // for(int i=0; i<loadePart->n; i++){
        //     printf("%2x ", loadePart->text[i]);
        // }
        // printf("\n");

        unMake_bws(loadePart->text, loadePart->n, origin);
        for(int i=0; i<loadePart->n; i++){
            fprintf(out, "%c", loadePart->text[i]);
            // putRleFore(out, loadePart->text[i]);
            // printf("%c", loadePart->text[i]);
        }
        // printf("\n");
    }
    fclose(out);
    // printf("end");
    return 0;
}