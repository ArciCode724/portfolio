#ifndef FFT_H  
#define FFT_H

#include<complex.h>

void fft(complex* data, int numData, complex* out);

void ifft(complex* data, int numData, complex* out);

void fft2d(complex* data, int numX, int numY, complex* out);

void ifft2d(complex* data, int numX, int numY, complex* out);

#endif 