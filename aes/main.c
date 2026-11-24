#include<stdio.h>
#include"gf256.h"

uint8_t sBox(uint8_t b){
    b = (b != 0) ? gfDivis(1,b) : 0;
    return b ^ (b<<1 | b>>7) ^ (b<<2 | b>>6) ^ (b<<3 | b>>5) ^ (b<<4 | b>>4) ^ 0x63;
}

uint8_t invSBox(uint8_t b){
    b = (b<<1 | b>>7) ^ (b<<3 | b>>5) ^ (b<<6 | b>>2) ^ 0x05;
    return (b != 0) ? gfDivis(1,b) : 0;
}

void subBytes(uint8_t data[16]){
    for (int i = 0; i < 16; i++){
        data[i] = sBox(data[i]);
    }
}

void invSubBytes(uint8_t data[16]){
    for (int i = 0; i < 16; i++){
        data[i] = invSBox(data[i]);
    }
}

void shiftRows(uint8_t data[16]){
    for(int x=0; x<4; x++){
        uint8_t temp[4];
        for (int y = 0; y < 4; y++){
            temp[y] = data[4*y+x];
        }
        for (int y= 0; y < 4; y++){
            data[4*y+x] = temp[(x+y)%4];
        }
    }
}

void invShiftRows(uint8_t data[16]){
    for(int x=0; x<4; x++){
        uint8_t temp[4];
        for (int y = 0; y < 4; y++){
            temp[y] = data[4*y+x];
        }
        for (int y= 0; y < 4; y++){
            data[4*y+x] = temp[(y-x+4)%4];
        }
    }
}

void mixColumns(uint8_t data[16]){
    uint8_t multiBy[4] = {2,3,1,1};
    for (int y = 0; y < 4; y++){
        uint8_t temp[4];
        for(int x=0; x<4; x++){
            temp[x] = 0;
            for (int i = 0; i < 4; i++){
                temp[x] ^= gfMulti(multiBy[(i-x+4)%4], data[i]);
            }
        }

        for(int x=0; x<4; x++){
            data[x] = temp[x];
        }
        data += 4;
    }
}

void invMixColumns(uint8_t data[16]){
    uint8_t multiBy[4] = {14,11,13,9};
    for (int y = 0; y < 4; y++){
        uint8_t temp[4];
        for(int x=0; x<4; x++){
            temp[x] = 0;
            for (int i = 0; i < 4; i++){
                temp[x] ^= gfMulti(multiBy[(i-x+4)%4], data[i]);
            }
        }

        for(int x=0; x<4; x++){
            data[x] = temp[x];
        }
        data += 4;
    }
}

void addRoundKey(uint8_t data[16], uint32_t key[4]){
    for (int i = 0; i < 16; i++){
        data[i] ^= key[i/4]>>(i%4 *8);
    }
}

uint32_t rotWord(uint32_t w){
    return w>>8 | w<<24;
}

uint32_t subWord(uint32_t w){
    uint32_t retVal = 0;
    for (int i = 0; i < 32; i+=8){
        retVal |= sBox(w>>i)<<i;
    }
    return retVal;
}

// outExpandKey ma rozmiar roundNum*4
void keyExpansion(uint32_t* key, int wordNum, uint32_t* outExpandKey, int roundNum){
    uint32_t rcon[11] = {0,0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
    for (int i = 0; i < wordNum; i++){
        outExpandKey[i] = key[i];
    }
    for (int i = wordNum; i < roundNum*4; i++){
        if(i%wordNum==0){
            // printf("%d %x\n", i, rcon[i/4]);
            outExpandKey[i] = outExpandKey[i-wordNum] ^ subWord(rotWord(outExpandKey[i-1])) ^ rcon[i/4];
        }else{
            outExpandKey[i] = outExpandKey[i-wordNum] ^ outExpandKey[i-1];
        }
    }
}

void blockPrint(uint8_t data[16]){
    for (int i = 0; i < 16; i++){
        printf("%02x ", data[4*(i%4)+i/4]);
        if((i+1)%4==0){
            printf("\n");
        }
    }
    printf("\n");
}

void cipher(uint8_t data[16], uint32_t* key, int keyNum){
    uint32_t expandedKey[44];
    keyExpansion(key, 4, expandedKey, 11);
    uint32_t* pKey = expandedKey;

    addRoundKey(data, pKey);
    // blockPrint((uint8_t*)pKey);
    // blockPrint(data);
    for (int i = 0; i < 9; i++){
        pKey += 4;
        subBytes(data);
        shiftRows(data);
        mixColumns(data);
        // blockPrint((uint8_t*)pKey);
        addRoundKey(data, pKey);
    }
    pKey += 4;
    subBytes(data);
    shiftRows(data);
    addRoundKey(data, pKey);
}

void invCipher(uint8_t data[16], uint32_t* key, int keyNum){
    uint32_t expandedKey[44];
    keyExpansion(key, 4, expandedKey, 11);
    uint32_t* pKey = expandedKey + 40;

    addRoundKey(data, pKey);
    // // blockPrint((uint8_t*)pKey);
    // // blockPrint(data);
    for (int i = 0; i < 9; i++){
        pKey -= 4;
        invShiftRows(data);
        invSubBytes(data);
        addRoundKey(data, pKey);
        invMixColumns(data);
    //     // blockPrint((uint8_t*)pKey);
    }
    pKey -= 4;
    invShiftRows(data);
    invSubBytes(data);
    addRoundKey(data, pKey);
}

void testShiftRow(){
    uint8_t data[16];
    for (int i = 0; i < 16; i++){
        data[i] = i;
    }
    shiftRows(data);
    blockPrint(data);
    invShiftRows(data);
    blockPrint(data);
}

void testMixColumns(){
    uint8_t data[16] = {
        99, 0,0,0,
        71, 0,0,0,
        162, 0,0,0,
        240, 0,0,0
    };
    blockPrint(data);
    mixColumns(data);
    blockPrint(data);
    invMixColumns(data);
    blockPrint(data);
}

void testKeyExpand(){
    uint8_t key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    uint32_t keyExpanded[44];
    keyExpansion((uint32_t*)key, 4, keyExpanded, 11);
    for (int i = 0; i < 44; i++){
        // printf("%2d %6x\n", i, keyExpanded[i]);
        printf("%2d ", i, keyExpanded[i]);
        for (int j = 0; j < 4; j++){
            printf("%02x", keyExpanded[i]&0xff);
            keyExpanded[i]>>=8;
        }
        printf("\n");
    }
    
}

void testCipher(){
    uint8_t data[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
    uint8_t key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    blockPrint(data);
    cipher(data, (uint32_t*)key, 4);
    blockPrint(data);
    invCipher(data, (uint32_t*)key, 4);
    blockPrint(data);
}

int main(int argc, char const *argv[]){
    // testShiftRow();
    // testMixColumns();
    // testKeyExpand();
    testCipher();
    return 0;
}
