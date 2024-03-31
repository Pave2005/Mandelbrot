#include "lab_calcul_funcs.h"

float* ReadText (long size, FILE* file)
{
    float* buf = (float*)calloc (size + 1, sizeof (float));
    fread (buf, sizeof (float), size, file);
    return buf;
}

long FileSize (FILE* file)
{
    fseek (file, 0, SEEK_END);
    long size = ftell (file);
    fseek (file, 0, SEEK_SET);
    return size;
}

float GetAvrVal (float* vals, int measure_num)
{
    float sum = 0;

    for (int i = 0; i < measure_num; i++)
    {
        sum += vals[i];
    }

    return sum / measure_num;
}

float GetStdDev (float* vals, float avr_val, int measure_num)
{
    float std_dev_sum = 0;

    for (int i = 0; i < measure_num; i++)
    {
        float dif = vals[i] - avr_val;
        std_dev_sum += dif * dif;
    }

    return sqrt (std_dev_sum / measure_num);
}
