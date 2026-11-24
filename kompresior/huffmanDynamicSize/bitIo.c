#include<stdio.h>
#include<stdlib.h>
#include"bitIo.h"

void bitI_close(bitI* toFree){
    fclose(toFree->file);
    free(toFree);
};

void bitO_close(bitO* toFree){
    fclose(toFree->file);
    free(toFree);
};

bitI* bitI_init(char* fileName){
    bitI* retVal = malloc(sizeof(bitI));
    retVal->file = fopen(fileName, "rb");
    if(!retVal->file)
        return NULL;
    retVal->alerdyRead = 8;
    return retVal;
}

char bitI_getBit(bitI* reader){
    if(reader->alerdyRead == 8){
        if(fread(&reader->loaded,sizeof(BYTE),1,reader->file) == 0)
            return EOF;
        reader->alerdyRead=0;
    }
    char toReturn;
    if(reader->loaded & 128 ){
        toReturn = '1';
    }else{
        toReturn = '0';
    }
    reader->loaded<<=1;
    reader->alerdyRead++;
    return toReturn;
};

int bitI_getByte(bitI* reader, BYTE* save){
    return fread(save,1,1,reader->file);
};

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
void bitO_putBit(bitO* writer, char what){
    writer->loaded<<=1;
    if(what == '1'){
        writer->loaded |= 1;
    }
    writer->alerdyWriten++;
    if(writer->alerdyWriten == 8){
        fprintf(writer->file, "%c", writer->loaded);
        writer->alerdyWriten = 0;
    }
};

void bitO_putByte(bitO* writer, BYTE what){
    fprintf(writer->file, "%c", what);
}