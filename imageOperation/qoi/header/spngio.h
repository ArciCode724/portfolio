#ifndef SPNGIO_H
#define SPNGIO_H

#include"qoi.h"

void spng_decode(char* fileName, qoiFile* out);

void spng_encode(char* fileName, qoiFile* in);

#endif