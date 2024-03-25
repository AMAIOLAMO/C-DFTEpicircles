#ifndef FOURIER_H
#define FOURIER_H

#include <stdio.h>
#include "../include/epicircle.h"

void DFourierTranform(float *input, Epicircle *epicircles, size_t size);

void DFourierTransform2D(struct Vector2 *input, Epicircle *xEpicircles,
                         Epicircle *yEpicircles, size_t size);

#endif // !FOURIER_H
