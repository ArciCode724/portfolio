#include<stdlib.h>
#include<stdio.h>
#include"huffman.h"
#include"bitIO.h"

bitList* CanonicalHuffman_innit(){
    bitList* retVal = malloc(sizeof(bitList)*CODESNUM);
    for(uint16_t i=0; i<CODESNUM; i++){
        retVal[i].n = 0;
        retVal[i].bits = malloc(1);
    }
    return retVal;
}

void koding_print(bitList* koding){
    for(int i=0; i<CODESNUM; i++){
        if(koding[i].n == 0){
            continue;
        }
        printf("%d: ", i);
        for(int j=0; j<koding[i].n; j++){
            printf("%d", koding[i].bits[j]);
        }
        printf("\n");
    }
}

kodeTree* kodeTree_init(uint16_t value){
    kodeTree* retVal = malloc(sizeof(kodeTree));
    retVal->left = NULL;
    retVal->right = NULL;
    retVal->value = value;
    return retVal;
}

void kodeTree_free(kodeTree* toFree){
    if(!toFree)
        return;

    kodeTree_free(toFree->left);
    kodeTree_free(toFree->right);
    free(toFree);
};

void kodeTree_print(kodeTree* toPrint, uint16_t deep){
    if(!toPrint->left){
        printf("%*d\n", deep, toPrint->value);
        return;
    }
    kodeTree_print(toPrint->right, deep+2);
    printf("%*c\n", deep, '|');
    kodeTree_print(toPrint->left, deep+2);
}

void TreeAddValue(kodeTree* tree, uint16_t value, bitList path, uint16_t deep){
    if(!tree->left){
        tree->left = kodeTree_init(0);
        tree->right = kodeTree_init(0);
    }
    if(path.n-1 == deep){
        if(path.bits[deep] == 0){
            tree->left->value = value;
        }else{
            tree->right->value = value;
        }
        return;
    }
    if(path.bits[deep] == 0){
        TreeAddValue(tree->left, value, path, deep+1);
    }else{
        TreeAddValue(tree->right, value, path, deep+1);
    }
};

kodeTree* CanonicalHuffmanToTree(bitList* code){
    kodeTree* root = kodeTree_init(0);
    for(uint16_t i=0; i<CODESNUM; i++){
        if(code[i].n == 0){
            continue;
        }
        // printf("%x ", i);
        TreeAddValue(root, i, code[i], 0);
    }
    return root;
}

void kodeTree_readLeafLens(bitList* save, kodeTree* tree, uint16_t deep){
    if(tree->left){
        deep++;
        kodeTree_readLeafLens(save, tree->left, deep);
        kodeTree_readLeafLens(save, tree->right, deep);
    }else{
        save[tree->value].n = deep;
        save[tree->value].bits = realloc(save[tree->value].bits, deep);
    }
}

void generateCanonicalHuffman(bitList* save){
    uint16_t znak[CODESNUM];
    uint16_t n = 0;

    for(uint16_t i=0; i<CODESNUM; i++){
        if(save[i].n == 0)
            continue;
        znak[n] = i;
        n++;
    }

    // znak po liczbach z save a drugorzednie po alfabecie
    for(uint16_t jump = n/2; jump > 0; jump/=2){
        for(uint16_t i=jump; i<n; i++){
            for(uint16_t j=i; j>=jump; j-=jump){
                if(save[znak[j-jump]].n < save[znak[j]].n)
                    break;
                if(save[znak[j-jump]].n == save[znak[j]].n && znak[j-jump] < znak[j])
                    break;
                uint16_t tempZ = znak[j-jump];
                znak[j-jump] = znak[j];
                znak[j] = tempZ;
            }
        }
    }

    for(uint16_t i=0; i< save[znak[0]].n; i++)
        save[znak[0]].bits[i] = 0;
    uint16_t d;
    for(uint16_t i = 1; i< n; i++){
        for(d=0; d<save[znak[i-1]].n; d++)
            save[znak[i]].bits[d] = save[znak[i-1]].bits[d];
        for(d=save[znak[i-1]].n; d<save[znak[i]].n; d++)
            save[znak[i]].bits[d] = 0;
        for(d=save[znak[i-1]].n-1; d>0 && save[znak[i]].bits[d]; d--)
            save[znak[i]].bits[d] = 0;
        save[znak[i]].bits[d] = 1;
    }

    // for(uint16_t i = 0; i< n; i++){
    //     printf("(%3d:%2u): ", znak[i], save[znak[i]].n);
    //     for(uint16_t j = 0; j<save[znak[i]].n; j++){
    //         printf("%d ", save[znak[i]].bits[j]);
    //     }
    //     printf(";\n");
    // }
}

void makeCanonicalHuffman(bitList* save, uint16_t* list, uint32_t n){
    for(uint16_t i=0; i<CODESNUM; i++){
        save[i].n = 0;
    }

    // zliczanie wystapien w list
    // counter i znak sa traktowanie jako jedna table z dwoma zedami
    uint32_t counter[CODESNUM];
    uint16_t znak[CODESNUM];
    for(uint16_t i=0; i<CODESNUM; i++){
        counter[i] = 0;
        znak[i] = i;
    }
    counter[EOB] = 1;
    for(uint32_t i=0; i<n; i++){
        counter[list[i]]++;
    }

    // shell sort counter i znak po counter
    for(uint32_t jump = CODESNUM/2; jump > 0; jump/=2){
        for(uint32_t i=jump; i<CODESNUM; i++){
            for(uint32_t j=i; j>=jump && counter[j-jump] < counter[j]; j-=jump){
                uint32_t tempC = counter[j-jump];
                counter[j-jump] = counter[j];
                counter[j] = tempC;

                uint16_t tempZ = znak[j-jump];
                znak[j-jump] = znak[j];
                znak[j] = tempZ;
            }
        }
    }

    // for(uint16_t i = 0; i< CODESNUM; i++){
    //     if(counter[i] == 0) 
    //         continue;
    //     printf("%3x:%u; ", znak[i], counter[i]);
    // }
    // printf("\n");

    uint16_t branczNumber = 0;
    kodeTree* treeQue[CODESNUM];
    for(uint16_t i=0; i<CODESNUM && counter[i]; i++){
        treeQue[i] = kodeTree_init(znak[i]);
        branczNumber++;
    }

    for(uint16_t i = branczNumber-1; i > 0; i--){
        kodeTree* tempT = kodeTree_init(0);
        tempT->left = treeQue[i-1];
        tempT->right = treeQue[i];
        treeQue[i-1] = tempT;
        counter[i-1] += counter[i]; 
        // branczNumber--;

        for(uint16_t j = i-1; j>0 && counter[j] > counter[j-1]; j--){
            uint32_t tempC = counter[j-1];
            counter[j-1] = counter[j];
            counter[j] = tempC;

            tempT = treeQue[j-1];
            treeQue[j-1] = treeQue[j];
            treeQue[j] = tempT;
        }
    }
    kodeTree_readLeafLens(save, treeQue[0], 0);
    // kodeTree_print(treeQue[0], 0);
    kodeTree_free(treeQue[0]);

    // for(uint16_t i = 0; i< CODESNUM; i++){
    //     if(save[i].n == 0) 
    //         continue;
    //     printf("%3u:%u; ", i, save[i].n);
    // }
    // printf("\n");

    generateCanonicalHuffman(save);
}

void writeInCanonicalHuffman(bitO* out, bitList* kode, uint16_t leter){
    for(uint16_t i=0; i < kode[leter].n; i++)
        bitO_putBit(out, kode[leter].bits[i]);
}

void writeCanonicalHuffman(bitO* out, bitList* list){
    for(uint16_t i=0; i<CODESNUM; i++){
        if(list[i].n){
            bitO_putBit(out, 1);
        }else{
            bitO_putBit(out, 0);
        }
    }

    uint16_t lastLen = 0;
    // zapisujemy w formacjie delta sposobe uranrym
    // 11 =+1; 10 =-1; 0 = stop;

    for(uint16_t i=0; i<CODESNUM; i++){
        if(list[i].n == 0){
            continue;
        }
        uint16_t delta;
        uint8_t type;
        if(lastLen < list[i].n){
            // add
            type = 1;
            delta = list[i].n - lastLen;
        }else{
            // odejmin
            type = 0;
            delta = lastLen - list[i].n;
        }
        lastLen = list[i].n;
        for(uint16_t j=0; j<delta;j++){
            bitO_putBit(out, 1);
            bitO_putBit(out, type);
        }
        bitO_putBit(out, 0);
    }
}

uint16_t readInCanonicalHuffman(bitI* in, kodeTree* kode){
    while (kode->left){
        if(bitI_getBit(in) == 0){
            kode = kode->left;
        }else{
            kode = kode->right;
        }
    }
    return kode->value;
}

void readCanonicalHuffman(bitI* in, bitList* list){
    for(uint16_t i=0; i<CODESNUM;i++){
        list[i].n = bitI_getBit(in);
    }

    uint16_t lastLen = 0;
    for(uint16_t i=0; i<CODESNUM; i++){
        if(list[i].n == 0){
            continue;
        }
        while(bitI_getBit(in)){
            lastLen += 2*bitI_getBit(in) - 1;
        }
        list[i].n = lastLen;
        list[i].bits = realloc(list[i].bits, list[i].n); // ty chuju pamiejtaj to realocka
    }
    generateCanonicalHuffman(list);
};
