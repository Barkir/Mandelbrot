#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <x86intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <avxintrin.h>

void inline print4  (float *);
void inline add4    (float *, float *, float *);
void inline sub4    (float *, float *, float *);
void inline mul4    (float *, float *, float *);
int  inline isnull4 (float *);
void inline mulnum  (float *, float, float *);
void inline cpy4    (float *, float *);
void inline cmple4  (float *, float *, float *);

inline void Mandelbrot   (void *, int *);
inline void Mandelbrot4  (void *, int *);
inline void Mandelbrot128(void *, int *);
inline void Mandelbrot256(void *, int *);
inline void Mandelbrot512(void *, int *);

int MandelbrotExecute(MandelSettings * param);

inline void Mandelbrot128(void * paramPointer, int * dots)
{
    MandelSettings * param = (MandelSettings*) paramPointer;

    float ampl = param->TransMat.ampl;
    float delx = param->TransMat.delx;
    float dely = param->TransMat.dely;

    alignas(16) __m128 dy     = _mm_set_ps1(param->StepY);
    alignas(16) __m128 dx     = _mm_set_ps1(param->StepX * param->PackSize);
    alignas(16) __m128 sdx    = _mm_set_ps1(param->StepX);
    alignas(16) __m128 bias   = _mm_set_ps(3.f, 2.f, 1.f, 0.f);
    alignas(16) __m128 rmax   = _mm_set1_ps(param->Rmax);

    alignas(16) __m128 y0 = _mm_set_ps1(-1.0f);

    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++, y0 = _mm_add_ps(y0, dy))
    {
        alignas(16) __m128 x0 = _mm_add_ps(_mm_mul_ps(bias, sdx), _mm_set_ps1(-1.f));
        for (int x = 0; x < SDL_SCREEN_WIDTH; x += param->PackSize, x0 = _mm_add_ps(x0, dx))
        {
            alignas(16) __m128 z_x      = _mm_setzero_ps();
            alignas(16) __m128 z_y      = _mm_setzero_ps();
            alignas(16) __m128 z_x2     = _mm_setzero_ps();
            alignas(16) __m128 z_y2     = _mm_setzero_ps();
            alignas(16) __m128i iter    = _mm_setzero_si128();

            volatile int it = 0;
            for (int i = 0; i < Niter; i++)
            {
                alignas(16) __m128 r2 = _mm_add_ps(z_x2, z_y2);
                alignas(16) __m128 mask = _mm_cmple_ps(r2, rmax);
                int mask2int = _mm_movemask_ps(mask);
                if (mask2int == 0x00)
                {
                    alignas(16) int integers[4] = {};
                    _mm_store_si128((__m128i*)integers, iter);

                    for (int i = 0; i < param->PackSize; i++)
                        dots[(x + i) + y * SDL_SCREEN_HEIGHT] = integers[i];

                    break;
                }

                z_y = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(z_y, z_x), _mm_set_ps1(2.f)), _mm_add_ps(_mm_mul_ps(y0, _mm_set_ps1(ampl)), _mm_set_ps1(dely)));        /*<<< z_y = 2 * z_y * z_x + y0 >>>*/
                z_x = _mm_add_ps(_mm_sub_ps(z_x2, z_y2), _mm_add_ps(_mm_mul_ps(x0, _mm_set_ps1(ampl)), _mm_set_ps1(delx)));                                    /*<<< z_x = z_x2 - z_y2 + x0 >>>*/

                iter = _mm_sub_epi32(iter, _mm_castps_si128(mask));                                                                                            /* incrementing iterations on active r's */
                z_x2 = _mm_mul_ps(z_x, z_x);
                z_y2 = _mm_mul_ps(z_y, z_y);
            }
        }
    }
}

    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

inline void Mandelbrot256(void * paramPointer, int * dots)
{
    MandelSettings * param = (MandelSettings*) paramPointer;

    float ampl = param->TransMat.ampl;
    float delx = param->TransMat.delx;
    float dely = param->TransMat.dely;

    alignas(32) __m256 mdely = _mm256_set1_ps(dely);
    alignas(32) __m256 mdelx = _mm256_set1_ps(delx);
    alignas(32) __m256 dy =   _mm256_set1_ps(param->StepY);
    alignas(32) __m256 dx =   _mm256_set1_ps(param->StepX * param->PackSize);
    alignas(32) __m256 sdx =  _mm256_set1_ps(param->StepX);
    alignas(32) __m256 bias = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
    alignas(32) __m256 rmax = _mm256_set1_ps(param->Rmax);


    alignas(32) __m256 y0 = _mm256_set1_ps(-1.0f);

    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++, y0 = _mm256_add_ps(y0, dy))
    {

        alignas(32) __m256 x0 = _mm256_add_ps(_mm256_mul_ps(bias, sdx), _mm256_set1_ps(-1.f));

        for (int x = 0; x < SDL_SCREEN_WIDTH; x += param->PackSize, x0 = _mm256_add_ps(x0, dx))
        {
            alignas(32) __m256 z_x      = _mm256_setzero_ps();
            alignas(32) __m256 z_y      = _mm256_setzero_ps();
            alignas(32) __m256 z_x2     = _mm256_setzero_ps();
            alignas(32) __m256 z_y2     = _mm256_setzero_ps();
            alignas(32) __m256i iter    = _mm256_setzero_si256();

            for (int i = 0; i < Niter; i++)
            {
                z_y = _mm256_add_ps(_mm256_mul_ps(_mm256_mul_ps(z_y, z_x), _mm256_set1_ps(2)), _mm256_add_ps(_mm256_mul_ps(y0, _mm256_set1_ps(ampl)), mdely));
                z_x = _mm256_add_ps(_mm256_sub_ps(z_x2, z_y2), _mm256_add_ps(_mm256_mul_ps(x0, _mm256_set1_ps(ampl)), mdelx));

                __m256 r2 = _mm256_add_ps(z_x2, z_y2);
                __m256 mask = _mm256_cmp_ps(r2, rmax, _CMP_LT_OS);
                iter = _mm256_sub_epi32(iter, _mm256_castps_si256(mask));
                int mask2int = _mm256_movemask_ps(mask);
                if (mask2int == 0x00)
                {
                    alignas(32) int integers[8] = {};
                    _mm256_store_si256((__m256i*)integers, iter);

                    for (int i = 0; i < param->PackSize; i++)
                        dots[(x + i) + y * SDL_SCREEN_HEIGHT] = integers[i];

                    break;
                }
                z_x2 = _mm256_mul_ps(z_x, z_x);
                z_y2 = _mm256_mul_ps(z_y, z_y);
            }
        }
    }
}

    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

inline void Mandelbrot4(void * paramPointer, int * dots)
{
    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SDL_SCREEN_WIDTH; x += 4)
        {
            float x0[4] = { -1.f + x * 2.f / SDL_SCREEN_WIDTH,          -1.f + (x + 1) * 2.f / SDL_SCREEN_WIDTH,
                            -1.f + (x + 2) * 2.f / SDL_SCREEN_WIDTH,    -1.f + (x + 3) * 2.f / SDL_SCREEN_WIDTH};

            float y0[4] = { -1.f + y * 2.f / SDL_SCREEN_HEIGHT,          -1.f + y * 2.f / SDL_SCREEN_HEIGHT,
                            -1.f + y * 2.f / SDL_SCREEN_HEIGHT,          -1.f + y * 2.f / SDL_SCREEN_HEIGHT};

            float z_x[4]    = {};
            float z_y[4]    = {};
            float z_x2[4]   = {};
            float z_y2[4]   = {};
            float r4[4]     = {};
            float mask[4]   = {};
            float rmax[4]   = {4.f, 4.f, 4.f, 4.f};

            float it[4] = {0, 0, 0, 0};
            for (int iters = 0; iters < Niter; iters++)
            {
                mul4(z_y, z_x, z_y);
                mulnum(z_y, 2, z_y);
                add4(z_y, y0, z_y);
                sub4(z_x2, z_y2, z_x);
                add4(z_x, x0, z_x);
                add4(z_x2, z_y2, r4);
                cmple4(r4, rmax, mask);
                add4(mask, it, it);

                if (isnull4(mask))
                {
                    for (int i = 0; i < 4; i++)
                        dots[(x + i) + y * SDL_SCREEN_HEIGHT] = it[i];
                    break;
                }
                mul4(z_x, z_x, z_x2);
                mul4(z_y, z_y, z_y2);
            }
        }
    }
}

    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

inline void Mandelbrot(void * paramPointer, int * dots)
{
    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SDL_SCREEN_WIDTH; x++)
        {
            float x0 = -1.f + x * 2.f / SDL_SCREEN_WIDTH;

            float y0 = -1.f + y * 2.f / SDL_SCREEN_HEIGHT;

            float z_x    = 0;
            float z_y    = 0;
            float z_x2   = 0;
            float z_y2   = 0;
            float rmax   = 4;

            volatile int it = 0;
            for (; it < Niter; it++)
            {
                z_y = 2 * z_y * z_x + y0;
                z_x = z_x2 - z_y2 + x0;

                if (z_x2 + z_y2 > rmax)
                {
                    dots[x + y * SDL_SCREEN_WIDTH] = it;
                    break;
                }

                z_x2 = z_x * z_x;
                z_y2 = z_y * z_y;

            }
        }
    }
}

void print4(float * val)
{
    LOG("[%f, %f, %f, %f]", val[0], val[1], val[2], val[3]);
}

void inline add4(float * a, float * b, float * save)
{
    for (int i = 0; i < 4; i++) save[i] = a[i] + b[i];
}

void inline sub4(float * a, float * b, float * save)
{
    for (int i = 0; i < 4; i++) save[i] = a[i] - b[i];
}

void inline mul4(float * a, float * b, float * save)
{
    for (int i = 0; i < 4; i++) save[i] = a[i] * b[i];
}

void inline mulnum(float * a, float val, float * save)
{
    for (int i = 0; i < 4; i++) save[i] = a[i] * val;
}

void inline cpy4(float * a, float * b)
{
    for (int i = 0; i < 4; i++) a[i] = b[i];
}

void inline cmple4(float * a, float * b, float * save)
{
    for (int i = 0; i < 4; i++)
    {
        if (a[i] <= b[i]) save[i] = 1;
        else save[i] = 0;
    }
}

int inline isnull4(float * a)
{
    return (a[0] == 0) && (a[1] == 0) && (a[2] == 0) && (a[3] == 0);
}


#endif
