#ifndef MAND
#define MAND

using namespace sf;

// #define TEST

const int max_pixel_idx = 256;
const int width         = 1400;
const int height        = 800;
const float dx          = 1/800.f;
const float dy          = 1/800.f;
const float x_shift     = width/10;
const int radius        = 10;

const __m128 RADIUS = _mm_set_ps1(10.f);

inline void WindowUpdate ();
inline void CreateWindow ();

int GetPixelColorSmpVers (float x_start, float y_start);
void GetPixelColorMidVers (float* x_start_vector, float* y_start_vector, int* pixel_idx_vector);
void GetPixelColorFastVers (__m128 x_start_vector, __m128 y_start_vector, __m128* color);

void DrawMaldelbrotSmpVers (Image* image, float scale, float users_shift);
void DrawMaldelbrotMidVers (Image* image, float scale, float users_shift);
void DrawMaldelbrotFastVers (Image* image, float scale, float users_shift);

void CountFPS (unsigned long long start, unsigned long long finish);

#endif
