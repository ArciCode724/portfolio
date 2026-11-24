#include<stdlib.h>
#include"..\header\bounds.h"

#define hightsBite64 0x8000000000000000

bounds* bounds_init(){
    bounds* retVal = malloc(sizeof(bounds));
    retVal->low = 0;
    retVal->high = 0xffffffffffffffff;
    retVal->underflow = 0;
    return retVal;
}

void bounds_encode(bounds* data, prop* kodeTable, uint16_t znak){
    uint32_t down, up, sum;
    prop_off(kodeTable, znak, &down, &up, &sum);
    uint64_t range = data->high - data->low;
    data->high = data->low + range/sum*up;
    data->low = data->low + range/sum*down;
}

uint16_t bounds_findZnak(bounds* data, prop* kodeTable, uint64_t kode){
    uint32_t down, up, sum;
    uint64_t range, tempHigh, tempLow;
    for(uint16_t i=0; i<kodeTable->n; i++){
        prop_off(kodeTable, i, &down, &up, &sum);
        range = data->high - data->low;
        tempHigh = data->low + range/sum*up;
        tempLow = data->low + range/sum*down;
        if(kode >= tempLow && kode < tempHigh){
            // printf("%llu %llu %llu\n", tempLow,kode,tempHigh);
            return i;
        }
    }
    return 0xffff;
}

void bounds_moveLeft(bounds* data){
    data->low<<=1;
    data->high = (data->high<<1) |1;
}

int bounds_wrapUnderflow(bounds* data){
    // idiot fail safe
    if((data->high & hightsBite64) == (data->low & hightsBite64))
        return 0;

    bounds_moveLeft(data);

    int retVal = 0;
    while((data->low & ~data->high) & hightsBite64){
        data->underflow++;
        retVal++;
        bounds_moveLeft(data);
    }
    
    data->low>>=1;
    data->high=data->high>>1 |hightsBite64;
    return retVal;
}

void bounds_printFront(bounds* data, bitO* out){
    // printf("%d", (int)(data->low>>63));
    bitO_putBit(out, data->low>>63);
    bounds_moveLeft(data);
}

void bounds_printWsplne(bounds* data, bitO* out){
    uint64_t wspulnie = ~(data->low^data->high);
    if(!(wspulnie & hightsBite64)){
        return;
    }
    uint8_t underFlowType = !(data->low>>63);
    bounds_printFront(data, out);
    wspulnie<<=1;
    // printf("(%u)",data->underflow); 
    for(;data->underflow>0; data->underflow--){
        bitO_putBit(out, underFlowType);
        // printf("%d", underFlowType);
    }
    while (wspulnie & hightsBite64){
        bounds_printFront(data, out);
        wspulnie<<=1;
    }
}

void bounds_close(bounds* data, bitO* out){
    bounds_printWsplne(data, out);
    bitO_putMulkiBits(out, 0b1, 1);
    free(data);
}

uint16_t bounds_removeSame(bounds* data){
    uint64_t wspulnie = ~(data->low^data->high);
    if(!(wspulnie & hightsBite64)){
        return 0;
    }
    uint16_t retVal = 0;
    while (wspulnie & hightsBite64){
        bounds_moveLeft(data);
        wspulnie<<=1;
        retVal++;
    }
    return retVal;
}
