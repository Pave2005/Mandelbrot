#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <xmmintrin.h>

#include "Mandelbrot.h"

//#define FPS

#ifdef FPS
void CountFPS (unsigned long long start, unsigned long long finish)
{
    unsigned long long dif = finish - start;
    int fps_value = (int)(1 / dif);
    printf ("%d", fps_value);
}
#endif

int GetPixelColorSmpVers (float x_start, float y_start)
{
    int x = x_start;
    int y = y_start;
    int x_sqr = x_start * x_start;
    int y_sqr = y_start * y_start;

    int pixel_idx = 0;

    while (pixel_idx < max_pixel_idx && x_sqr + y_sqr <= radius * radius)
    {
        x_sqr = x * x;
        y_sqr = y * y;

        x = x_sqr - y_sqr + x_start;
        y = 2 * x * y + y_start;

        pixel_idx++;
    }

    if (pixel_idx == max_pixel_idx)
        return 0;
    else
        return pixel_idx;
}

int* GetPixelColorMidVers (int* x_start_vector, int* y_start_vector)
{
    int x_vector[4] = {}; for (int i = 0; i < 4; i++) x_vector[i] = x_start_vector[i];
    int y_vector[4] = {}; for (int i = 0; i < 4; i++) y_vector[i] = y_start_vector[i];

    int pixel_idx_vector[4] = {}; // спросить у криса про локальные переменные.

    for (int pixel_idx = 0; pixel_idx < max_pixel_idx; pixel_idx++)
    {
        long long x_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) x_sqr_vector[i] = x_vector[i] * x_vector[i];

        long long y_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) y_sqr_vector[i] = y_vector[i] * y_vector[i];

        long long xy_vector[4]    = {};
        for (int i = 0; i < 4; i++)    xy_vector[i] = x_vector[i] * y_vector[i];

        long long r_sqr_vector[4] = {};
        for (int i = 0; i < 4; i++) r_sqr_vector[i] = x_sqr_vector[i] * y_sqr_vector[i];


        int all_vectors_smaller[4] = {};
        for (int i = 0; i < 4; i++) if (r_sqr_vector[i] <= radius * radius) all_vectors_smaller[i] = 1;

        int mask = 0; for (int i = 0; i < 4; i++) mask |= (all_vectors_smaller[i] << i);

        if (mask == 0) break;

        for (int i = 0; i < 4; i++) x_vector[i] = x_sqr_vector[i] - y_sqr_vector[i] + x_start_vector[i];

        for (int i = 0; i < 4; i++) y_vector[i] = 2 * xy_vector[i] + y_start_vector[i];

        for (int i = 0; i < 4; i++) pixel_idx_vector[i] = pixel_idx_vector[i] + all_vectors_smaller[i];
    }

    return pixel_idx_vector;
}


void GetPixelColorFastVers (__m128 x_start_vector, __m128 y_start_vector, __m128 color)
{
    __m128 x_vector = _mm_set_ps1 (0);
    __m128 y_vector = _mm_set_ps1 (0);

    _mm_storeu_si128 ((__m128i*)&x_vector, (__m128i)x_start_vector);
    _mm_storeu_si128 ((__m128i*)&y_vector, (__m128i)y_start_vector);

    for (int pixel_idx = 0; pixel_idx < max_pixel_idx; pixel_idx++)
    {
        __m128 x_sqr_vector = _mm_mul_ps (x_vector, x_vector);
        __m128 y_sqr_vector = _mm_mul_ps (y_vector, x_vector);
        __m128 xy_vector    = _mm_mul_ps (x_vector, y_vector);
        __m128 r_sqr_vector = _mm_add_ps (x_sqr_vector, y_sqr_vector);

        __m128 all_vectors_smaller = _mm_cmple_ps (r_sqr_vector, RADIUS);

        int mask = _mm_movemask_ps ((__m128)all_vectors_smaller);

        if (mask == 0) break;

        color = (__m128)_mm_sub_epi32 ((__m128i)color, (__m128i)all_vectors_smaller);

        x_vector = _mm_add_ps (_mm_sub_ps (x_sqr_vector, y_sqr_vector), x_start_vector);
        y_vector = _mm_add_ps (_mm_add_ps (xy_vector, xy_vector), y_start_vector);
    }
}


void DrawMaldelbrotSmpVers (Image image)
{
    int x_start = - (width / 2);
    int y_start = height / 2;

    #ifdef FPS
    unsigned long long start = __rdtsc();
    #endif

    for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
    {
        int y = y_start - y_pixel_counter;

        for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter++)
        {
            int x = x_start + x_pixel_counter;

            int color = GetPixelColorSmpVers (x, y);

            #ifndef FPS
            Color sfColor ((color * 6) % 256, color, (color * 10) % 256);
            image.setPixel (x_pixel_counter, y_pixel_counter, sfColor);
            #endif
        }
    }

    #ifdef FPS
    unsigned long long finish = __rdtsc();
    CountFPS (start, finish);
    #endif
}


void DrawMaldelbrotMidVers (Image image)
{
    int x_start = - (width / 2);
    int y_start = height / 2;

    for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
    {
        int y = y_start - y_pixel_counter;

        for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter += 4)
        {
            int x = x_start + x_pixel_counter;

            int x_start_vector[4] = {x, x + 1, x + 2, x + 3};
            int y_start_vector[4] = {y, y, y, y};

            int* color_vector = GetPixelColorMidVers (x_start_vector, y_start_vector);

            #ifndef FPS
            for (int i = 0; i < 4; i++)
            {
                Color sfColor ((color_vector[i] * 6) % 256, 0, (color_vector[i] * 10) % 256);
                image.setPixel (x_pixel_counter + i, y_pixel_counter, sfColor);
            }
            #endif
        }
    }
}


void DrawMaldelbrotFastVers (Image image)
{
    int x_start = - (width / 2);
    int y_start = height / 2;

    for (int y_pixel_counter = 0; y_pixel_counter < height; y_pixel_counter++)
    {
        int y = y_start - y_pixel_counter;

        for (int x_pixel_counter = 0; x_pixel_counter < width; x_pixel_counter += 4)
        {
            int x = x_start + x_pixel_counter;

            __m128 x_start_vector = _mm_set_ps ((float)(x + 3), (float)(x + 2), (float)(x + 1), (float)x);
            __m128 y_start_vector = _mm_set_ps1 ((float)y);

            __m128 pixel_idx_vector = (__m128)_mm_setzero_si128();

            GetPixelColorFastVers (x_start_vector, y_start_vector, pixel_idx_vector);

            int* color_vector = (int*)&pixel_idx_vector;

            for (int i = 0; i < 4; i++)
            {
                Color sfColor ((color_vector[i] * 6) % 256, 0, (color_vector[i] * 10) % 256);
                image.setPixel (x_pixel_counter + i, y_pixel_counter, sfColor);
            }
        }
    }
}
