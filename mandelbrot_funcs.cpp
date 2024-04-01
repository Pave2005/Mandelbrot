#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <xmmintrin.h>
#include <x86intrin.h>

#include "Mandelbrot.h"

int FpsCounter (clock_t start, clock_t finish)
{
    float time  = TIME(start, finish);
    int fps = (int)(1 / time);
    return fps;
}

int GetPixelColorSmpVers (float x_start, float y_start)
{
    float x = x_start;
    float y = y_start;
    float radius_sqr = radius * radius;

    int pixel_idx = 0;

    for (; pixel_idx < max_pixel_idx; pixel_idx++)
    {
        float x_sqr = x * x;
        float y_sqr = y * y;
        float xy    = x * y;

        if (x_sqr + y_sqr > radius_sqr) break;

        x = x_sqr - y_sqr + x_start;
        y = 2 * xy + y_start;
    }

    if (pixel_idx == max_pixel_idx)
        return 0;
    else
        return pixel_idx;
}

void GetPixelColorMidVers (float* x_start_vector, float* y_start_vector, int* pixel_idx_vector)
{
    float x_vector[4] = {}; for (int i = 0; i < 4; i++) x_vector[i] = x_start_vector[i];
    float y_vector[4] = {}; for (int i = 0; i < 4; i++) y_vector[i] = y_start_vector[i];

    for (int pixel_idx = 0; pixel_idx < max_pixel_idx; pixel_idx++)
    {
        float x_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) x_sqr_vector[i] = x_vector[i] * x_vector[i];

        float y_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) y_sqr_vector[i] = y_vector[i] * y_vector[i];

        float xy_vector[4]    = {};
        for (int i = 0; i < 4; i++) xy_vector[i] = x_vector[i] * y_vector[i];

        float r_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) r_sqr_vector[i] = x_sqr_vector[i] + y_sqr_vector[i];

        float radius_sqr = radius * radius;

        int all_vectors_smaller[4] = {};
        for (int i = 0; i < 4; i++) if (r_sqr_vector[i] <= radius_sqr) all_vectors_smaller[i] = 1;

        int mask = 0; for (int i = 0; i < 4; i++) mask |= (all_vectors_smaller[i] << i);

        if (mask == 0) break;

        for (int i = 0; i < 4; i++) x_vector[i] = x_sqr_vector[i] - y_sqr_vector[i] + x_start_vector[i];

        for (int i = 0; i < 4; i++) y_vector[i] = 2 * xy_vector[i] + y_start_vector[i];

        for (int i = 0; i < 4; i++) pixel_idx_vector[i] = pixel_idx_vector[i] + all_vectors_smaller[i];
    }
}


void GetPixelColorFastVers (__m128 x_start_vector, __m128 y_start_vector, __m128* color)
{
    __m128 x_vector = _mm_set_ps1 (0);
    __m128 y_vector = _mm_set_ps1 (0);

    _mm_storeu_si128 ((__m128i*)&x_vector, (__m128i)x_start_vector);
    _mm_storeu_si128 ((__m128i*)&y_vector, (__m128i)y_start_vector);

    for (int pixel_idx = 0; pixel_idx < max_pixel_idx; pixel_idx++)
    {
        __m128 x_sqr_vector = _mm_mul_ps (x_vector, x_vector);
        __m128 y_sqr_vector = _mm_mul_ps (y_vector, y_vector);
        __m128 xy_vector    = _mm_mul_ps (x_vector, y_vector);
        __m128 r_sqr_vector = _mm_add_ps (x_sqr_vector, y_sqr_vector);
        __m128 radius_sqr   = _mm_mul_ps (RADIUS, RADIUS);

        __m128 all_vectors_smaller = _mm_cmple_ps (r_sqr_vector, radius_sqr);

        int mask = _mm_movemask_ps ((__m128)all_vectors_smaller);

        if (mask == 0) break;

        *color = (__m128)_mm_sub_epi32 ((__m128i)*color, (__m128i)all_vectors_smaller);

        x_vector = _mm_add_ps (_mm_sub_ps (x_sqr_vector, y_sqr_vector), x_start_vector);
        y_vector = _mm_add_ps (_mm_add_ps (xy_vector, xy_vector), y_start_vector);
    }
}


void DrawMaldelbrotSmpVers (Image* image, float scale, float users_shift)
{
    float x_start = -(width / 2) - x_shift + users_shift;
    float y_start = height / 2;
    float x_trans = dx * scale;
    float y_trans = dy * scale;

    #ifdef TEST
    FILE* times_file = fopen ("times_file.txt", "w");

    float color_detect_time[measure_num] = {};

    clock_t t_start = clock ();

    int tmp = 0;   // так как без этой переменной не расчитывается color, так как он просто не используется.

    for (int i = 0; i < measure_num; i++)
    {
        clock_t start = clock ();
        #endif

        for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
        {
            float y = (y_start - y_pixel_counter) * y_trans;

            for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter++)
            {
                float x = (x_start + x_pixel_counter) * x_trans;

                int color = GetPixelColorSmpVers (x, y);

                #ifdef TEST
                tmp = color;
                #endif

                #ifndef TEST
                Color Color (0, (color * 6) % 256, (color * 10) % 256);
                (*image).setPixel (x_pixel_counter, y_pixel_counter, Color);
                #endif
            }

        }

        #ifdef TEST
        clock_t finish = clock ();
        color_detect_time[i] = TIME(start, finish);
    }
    printf ("tmp = %d\n", tmp); // добавлени для того, чтобы -O3 не удалял часть программы с поиском цвета

    clock_t t_finish = clock ();

    float total_time = TIME(t_start, t_finish);

    printf ("\ntotal time = %f seconds\n\n", total_time);

    fwrite (color_detect_time, sizeof (float), measure_num, times_file);
    fclose (times_file);
    #endif
}


void DrawMaldelbrotMidVers (Image* image, float scale, float users_shift)
{
    float x_start = - (width / 2) - x_shift + users_shift;
    float y_start = height / 2;
    float x_trans = dx * scale;
    float y_trans = dy * scale;

    #ifdef TEST
    FILE* times_file = fopen ("times_file.txt", "w");

    float color_detect_time[measure_num] = {};

    clock_t t_start = clock ();

    int tmp[4] = {};

    for (int i = 0; i < measure_num; i++)
    {
        clock_t start = clock ();
        #endif

        for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
        {
            float y = (y_start - y_pixel_counter) * y_trans;

            for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter += 4)
            {
                float x = (x_start + x_pixel_counter) * x_trans;

                float x_start_vector[4] = {x, x + x_trans, x + 2*x_trans, x + 3*x_trans};
                float y_start_vector[4] = {y, y, y, y};

                int color_vector[4] = {};

                #ifdef TEST
                for (int i = 0; i < 4; i++) tmp[i] = color_vector[i];
                #endif

                GetPixelColorMidVers (x_start_vector, y_start_vector, color_vector);

                #ifndef TEST
                for (int i = 0; i < 4; i++)
                {
                    Color Color (0, (color_vector[i] * 6) % 256, (color_vector[i] * 10) % 256);
                    (*image).setPixel (x_pixel_counter + i, y_pixel_counter, Color);
                }
                #endif
            }
        }

        #ifdef TEST
        clock_t finish = clock ();
        color_detect_time[i] = TIME(start, finish);
    }
    clock_t t_finish = clock ();

    printf ("%d, %d, %d, %d\n", tmp[0], tmp[1], tmp[2], tmp[3]);

    float total_time = TIME(t_start, t_finish);

    printf ("\ntotal time = %f seconds\n\n", total_time);

    fwrite (color_detect_time, sizeof (float), measure_num, times_file);
    fclose (times_file);
    #endif
}


void DrawMaldelbrotFastVers (Image* image, float scale, float users_shift)
{
    float x_start = - (width / 2) - x_shift + users_shift;
    float y_start = height / 2;
    float x_trans = dx * scale;
    float y_trans = dy * scale;

    #ifdef TEST
    FILE* times_file = fopen ("times_file.txt", "w");

    float color_detect_time[measure_num] = {};

    clock_t t_start = clock ();

    int tmp[4] = {};

    for (int i = 0; i < measure_num; i++)
    {
        clock_t start = clock ();
        #endif

        for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
        {
            float y = (y_start - y_pixel_counter) * y_trans;

            for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter += 4)
            {
                float x = (x_start + x_pixel_counter) * x_trans;

                __m128 x_start_vector = _mm_set_ps (x + 3*x_trans, x + 2*x_trans, x + x_trans, x);
                __m128 y_start_vector = _mm_set_ps1 ((float)y);

                __m128 pixel_idx_vector = _mm_set_ps1 (0);

                GetPixelColorFastVers (x_start_vector, y_start_vector, &pixel_idx_vector);

                int* color_vector = (int*)&pixel_idx_vector;

                #ifdef TEST
                for (int i = 0; i < 4; i++) tmp[i] = color_vector[i];
                #endif

                #ifndef TEST
                for (int i = 0; i < 4; i++)
                {
                    Color Color (0, (color_vector[i] * 6) % 256, (color_vector[i] * 10) % 256);
                    (*image).setPixel (x_pixel_counter + i, y_pixel_counter, Color);
                }
                #endif
            }
        }

        #ifdef TEST
        clock_t finish = clock ();
        color_detect_time[i] = TIME(start, finish);
    }
    clock_t t_finish = clock ();

    printf ("%d, %d, %d, %d\n", tmp[0], tmp[1], tmp[2], tmp[3]);

    float total_time = TIME(t_start, t_finish);

    printf ("\ntotal time = %f seconds\n\n", total_time);

    fwrite (color_detect_time, sizeof (float), measure_num, times_file);
    fclose (times_file);
    #endif
}
