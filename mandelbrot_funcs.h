#ifndef MAND
#define MAND

using namespace sf;

const int max_pixel_idx = 256;
const int radius        = 50;
const int width         = 800;
const int height        = 600;

const __m128 RADIUS = _mm_set_ps1(100.f);


inline void WindowUpdate ();
inline void CreateWindow ();

inline int GetPixelColorSmpVers (float x_start, float y_start);
int* GetPixelColorMidVers (int* x_start_vector, int* y_start_vector);
void GetPixelColorFastVers (__m128 x_start_vector, __m128 y_start_vector, __m128 color);

void DrawMaldelbrotSmpVers (Image image);
void DrawMaldelbrotMidVers (Image image);
void DrawMaldelbrotFastVers (Image image);

void CountFPS (unsigned long long start, unsigned long long finish);

#endif
