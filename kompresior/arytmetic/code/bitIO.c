#include<stdio.h>
#include<stdlib.h>
#include"..\header\bitIO.h"


bitI* bitI_init(char* fileName){
    bitI* retVal = malloc(sizeof(bitI));
    retVal->file = fopen(fileName, "rb");
    if(!retVal->file)
    return NULL;
    retVal->alerdyRead = 8;
    return retVal;
}

int bitI_getBit(bitI* reader){
    if(reader->alerdyRead == 8){
        if(fscanf(reader->file, "%c", &reader->loaded) <= 0){
            return EOF;
        }
        reader->alerdyRead=0;
    }
    short toReturn = (reader->loaded & 128)/128;
    reader->loaded<<=1;
    reader->alerdyRead++;
    return toReturn;
};

int bitI_getMulkiBits(bitI* reader, int size){
    int retVal = 0;
    for(int i=0; i<size; i++){
        retVal<<=1;
        int bufer = bitI_getBit(reader);
        if(bufer == EOF){
            return EOF;
        }
        retVal |= bufer;
    }
    return retVal;
}

bitO* bitO_init(char* fileName){
    bitO* retVal = malloc(sizeof(bitI));
    retVal->file = fopen(fileName, "wb");
    if(!retVal->file)
    return NULL;
    retVal->alerdyWriten = 0;
    retVal->loaded='\0';
    return retVal;
};

//left to right
void bitO_putBit(bitO* writer, short what){
    writer->loaded<<=1;
    writer->loaded |= (what != 0);
    writer->alerdyWriten++;
    if(writer->alerdyWriten == 8){
        fprintf(writer->file, "%c", writer->loaded);
        writer->alerdyWriten = 0;
    }
};

void bitO_putMulkiBits(bitO* writer, int what, int size){
    int temp = 0;
    for (int i = 0; i < size; i++){
        temp<<=1;
        temp |= what & 1;
        what>>=1;
    }
    // printf("%d", temp);
    for (int i = 0; i < size; i++){
        bitO_putBit(writer, temp & 1);
        temp>>=1;
    }
}

void bitI_close(bitI* toFree){
    fclose(toFree->file);
    free(toFree);
};

void bitO_close(bitO* toFree){
    // bitO_putMulkiBits(toFree, 0, 7);
    fclose(toFree->file);
    free(toFree);
};