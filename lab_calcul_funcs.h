#ifndef LAB
#define LAB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const int measure_num = 100;

long FileSize (FILE* file);
float* ReadText (long size, FILE* file);

float GetAvrVal (float* vals, int measure_num);
float GetStdDev (float* vals, float avr_val, int measure_num);

#endif
