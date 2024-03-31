#include <stdio.h>

#include "lab_calcul_funcs.h"

int main ()
{
    FILE* times_file = fopen ("times_file.txt", "r");

    float* times_buf = ReadText (FileSize (times_file), times_file);

    float avr_val = GetAvrVal (times_buf, measure_num);
    printf ("Average value = %f\n\n", avr_val);

    float std_dev = GetStdDev (times_buf, avr_val, measure_num);
    printf ("Standart deviation = %f\n\n", std_dev);
}
