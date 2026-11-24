#ifndef FHT_H
#define FHT_H

void walshOrder(int* out, int level);

// Fast Walsh–Hadamard transform
void fht(int* list, int level);

void ifht(int* list, int level);

void fht2d(int* list, int level);

void ifht2d(int* list, int level);

#endif