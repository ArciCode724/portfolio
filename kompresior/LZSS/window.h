#include<math.h>
#include<stdint.h>
#include"bitIO.h"


// najwiekszy numer jaki ta wartosc zmiesci to rozmiar naszego okna
typedef uint16_t uint;
#define MIN_ROZ 2*(uint)sizeof(uint)

typedef struct{
    uint* list;
    uint n;
    uint mn;
} uintList;

typedef struct{
    BYTE* tasma;
    uint center;
    // przeczytanie w pamiecie
    uint left; 
    // do przeczytania
    uint right;

    struct{
        // kombinacje trzech znakow stanowią baze indexów
        // numry w uintList -ofset+1 daja absolutny index kombinacji znakow w taśmie po czesci przeczytanie
        uintList* kombinacje;
        uint ofset; 
        int n;
    } quickFinder;
} window;

window* window_init();

// zwraca -1 kiedy nie morzna
int makeFinderIndex(window* base, uint index);

// przewuwa od prawa do lewa
void window_moveBy(window* where, uint by);

// znajduje czy po lewej stronie wystepuje 
// zwraca w index i len
// index jak bardzo w strecz
// len ile bytow
void window_find(window* where, uint* retIndex, uint* retLen);

void window_printTasme(window* where);

int window_fillQue(window* where, bitI* from);