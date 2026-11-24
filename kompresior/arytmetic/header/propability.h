#ifndef PROPABILITY_H
#define PROPABILITY_H

#include<stdint.h>

#define EOP 256

// przyjmie maksymalnie 2^32-1 symboli
typedef struct{
    uint16_t n;
    uint32_t* steps;
} prop;

prop* prop_init(uint16_t n);

void prop_free(prop* list);

void prop_inkris(prop* list, uint16_t what, int by);

void prop_off(prop* list, uint16_t off, uint32_t* down, uint32_t* up, uint32_t* sum);

// dla wiekszego niż 2 nie ma miejca hyba że żacza bym robil jakies machlojki z dzieleniem pamieci 
#define MAXKONTEKST 2
#define MAXLEVEL MAXKONTEKST+1

// 0 - bazowe niezmiene prawdopodobiensto
// 1 - prawdopodobienstow dla zerowego kontestu 
// 2 - prawdopodobienstow dla kontestu jednego znaku 
// 3 - prawdopodobienstow dla kontestu dwuch znaku 
typedef struct{
    prop* lowLevels[2];
    prop** highLevels[MAXKONTEKST];
} propLavels;

propLavels* propLavels_init();

prop* propLavels_findProp(propLavels* list, int level, uint8_t* kontekst, int kontekstSize);

void propLavels_inkris(propLavels* list, uint16_t what, uint8_t* kontekst, int kontekstSize);

void propLavels_lower(propLavels* list, uint8_t* soonRemowdKontekst, int kontekstSize);

#endif