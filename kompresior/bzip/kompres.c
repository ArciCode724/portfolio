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

void fillBlock(FILE* sorse, block* save, uint32_t size){
    uint32_t i;
    uint8_t  c;
    save->n = 0;
    for(i=0; i<size; i++){
        if(fscanf(sorse, "%c", &c) == EOF)
            break;
        save->text[i] = c;
        save->n++;
    }
}

int main(int argc, char** argv){
    uint32_t blockSize = 65536;
    char* fileName;
    char c;
    while((c = getopt(argc, argv, "f:s:")) != -1){
        switch (c){
            case 's':
                blockSize = atoi(optarg);
                break;
            case 'f':
                fileName = optarg;
                break;
            default:
                break;
        }
    }
    FILE* in = fopen(fileName, "rb");
    if(!in){
        printf("nie ma pliku");
        return 1;
    }
    bitO* out = bitO_init("out.txt");

    block* loadePart = block_init(blockSize);
    rleText* rleBlock = rleText_init(blockSize);
    bitO_putMulkiBits(out, blockSize, 32);
    bitList* koding = CanonicalHuffman_innit();

    fillBlock(in, loadePart, blockSize);
    // printf("%u\n", loadePart->n);

    while (loadePart->n){
        static int blockNum = 0;
        printf("\r%d", ++blockNum);
        // for(int i=0; i<loadePart->n; i++){
        //     printf("%2x ", loadePart->text[i]);
        // }
        // printf("\n");

        uint32_t origin = make_bws(loadePart->text, loadePart->n);
        bitO_putMulkiBits(out,origin,32);
        // for(int i=0; i<loadePart->n; i++){
        //     printf("%x ", loadePart->text[i]);
        // }
        // printf(": %u\n", origin);
        
        moveToFront(loadePart->text, loadePart->n);
        // for(int i=0; i<loadePart->n; i++){
        //     printf("%2x ", loadePart->text[i]);
        // }
        // printf("\n");
        
        rleZeros(rleBlock, loadePart->text, loadePart->n);
        // printf("\n");
        
        makeCanonicalHuffman(koding, rleBlock->text, rleBlock->n);

        // koding_print(koding);
        // printf("\n");

        writeCanonicalHuffman(out, koding);
        for(uint32_t i=0; i<rleBlock->n; i++){
            writeInCanonicalHuffman(out, koding, rleBlock->text[i]);
        }

        writeInCanonicalHuffman(out, koding, EOB);

        fillBlock(in, loadePart, blockSize);
        // printf("\n");
    }
    
    bitO_putMulkiBits(out,0,7);
    // printf("end");
    return 0;
}