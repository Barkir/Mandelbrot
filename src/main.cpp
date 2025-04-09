#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <x86intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <avxintrin.h>
#include <cpuid.h>

#include <SDL2/SDL.h>

#include "../include/main.h"

void        print4  (float *);
void inline add4    (float *, float *, float *);
void inline sub4    (float *, float *, float *);
void inline mul4    (float *, float *, float *);
int  inline isnull4 (float *);
void inline mulnum  (float *, float, float *);
void inline cpy4    (float *, float *);
void inline cmple4  (float *, float *, float *);

inline void Mandelbrot   (void *, int *, void *);
inline void Mandelbrot4  (void *, int *, void *);
inline void Mandelbrot128(void *, int *, void *);
inline void Mandelbrot256(void *, int *, void *);
inline void Mandelbrot512(void *, int *, void *);
int SDL_Mandelbrot       (void *, int *);

/*
 *  ~This is a mandelbrot manual~
 *
 *  To run this program type in:
 *  ./run --{graph/ngraph} {optimization mode} {iterations}
 *
*/


int main(int argc, char * argv[])
{
    int * mandelbrot = NULL;
    uint64_t start = 0, end = 0;
    Transform mat = {1, 0, 0};
    Mandelparam param = {};

    if (process_cmd(&param, argc, argv) < 0) return 0;



    if (param.graph_flag == SSE_NGRAPH)
    {
        FILE * tfile = fopen(param.tfile, "w");
        if (!tfile) return SSE_FILE_OPEN_ERROR;

        for (int i = 0; i < param.iter; i++)
        {
            mandelbrot = (int*) aligned_alloc(param.pksz * 4, SDL_SCREEN_HEIGHT * SDL_SCREEN_WIDTH * sizeof(int));

            if (!mandelbrot) return SSE_MEMALLOC_ERROR;
            start = _rdtsc();
            param.func(&param, mandelbrot, &mat);
            end = _rdtsc();

            fprintf(tfile, "%ld\n", end - start);

            free(mandelbrot);
        }

        fclose(tfile);
        return 0;
    }

    mandelbrot = (int*) aligned_alloc(param.pksz * 4, SDL_SCREEN_HEIGHT * SDL_SCREEN_WIDTH * sizeof(int));
    if (!mandelbrot) return SSE_MEMALLOC_ERROR;
    SDL_Mandelbrot(&param, mandelbrot);
    free(mandelbrot);
    return 0;

}

int process_cmd(Mandelparam * param, int argc, char * argv[])
{

    if (argc == 4)
    {

        switch (atoi(argv[2]))
            {
                case 128:   mandel_param_init(param, 4, 2.f / SDL_SCREEN_WIDTH, 2.f / SDL_SCREEN_HEIGHT, 4, "info/mandel128.txt");
                            param->func = Mandelbrot128;
                            break;

                case 256:   mandel_param_init(param, 8, 2.f / SDL_SCREEN_WIDTH, 2.f / SDL_SCREEN_HEIGHT, 4, "info/mandel256.txt");
                            param->func = Mandelbrot256;
                            break;

                case 4:     mandel_param_init(param, 4, 2.f / SDL_SCREEN_WIDTH, 2.f / SDL_SCREEN_HEIGHT, 4, "info/mandel4.txt");
                            param->func = Mandelbrot4;
                            break;

                case 1:     mandel_param_init(param, 1, 2.f / SDL_SCREEN_WIDTH, 2.f / SDL_SCREEN_HEIGHT, 4, "info/mandeldefault.txt");
                            param->func = Mandelbrot;
                            break;


                default:    mandel_param_init(param, 4, 2.f / SDL_SCREEN_WIDTH, 2.f / SDL_SCREEN_HEIGHT, 4, "info/mandeldef.txt");
                            param->func = Mandelbrot128;
                            break;
            }

        if (!strcmp(argv[1], "--ngraph"))
        {

            param->iter = atoi(argv[3]);
            param->graph_flag = SSE_NGRAPH;
            return SSE_NGRAPH;
        }

        else if (!strcmp(argv[1], "--graph"))
        {
            param->iter = atoi(argv[3]);
            param->graph_flag = SSE_GRAPH;
            return SSE_GRAPH;
        }
    }

    return -fprintf(stderr, "Typo in: ./run <--ngraph/--graph> <optimization mode> <iterations number>\n");
}


    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

    /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */


inline void Mandelbrot128(void * p, int * dots, void * m)
{

    int * dots_pointer = dots;
    Transform * mat = (Transform*) m;
    Mandelparam * param = (Mandelparam*) p;


    float ampl = mat->ampl;
    float delx = mat->delx;
    float dely = mat->dely;


    alignas(16) __m128 dy     = _mm_set_ps1(param->stpy);
    alignas(16) __m128 dx     = _mm_set_ps1(param->stpx * param->pksz);
    alignas(16) __m128 sdx    = _mm_set_ps1(param->stpx);
    alignas(16) __m128 bias   = _mm_set_ps(3.f, 2.f, 1.f, 0.f);
    alignas(16) __m128 rmax   = _mm_set1_ps(param->rmax);

    alignas(16) __m128 y0 = _mm_set_ps1(-1.0f);

    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++, y0 = _mm_add_ps(y0, dy))
    {
        alignas(16) __m128 x0 = _mm_add_ps(_mm_mul_ps(bias, sdx), _mm_set_ps1(-1.f));
        for (int x = 0; x < SDL_SCREEN_WIDTH; x += param->pksz, x0 = _mm_add_ps(x0, dx))
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

                    for (int i = 0; i < param->pksz; i++)
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

inline void Mandelbrot256(void * p, int * dots, void * m)
{
    Mandelparam * param = (Mandelparam*) p;
    Transform * mat = (Transform*) m;

    float ampl = mat->ampl;
    float dely = mat->dely;
    float delx = mat->delx;

    alignas(32) __m256 mdely = _mm256_set1_ps(dely);
    alignas(32) __m256 mdelx = _mm256_set1_ps(delx);
    alignas(32) __m256 dy =   _mm256_set1_ps(param->stpy);
    alignas(32) __m256 dx =   _mm256_set1_ps(param->stpx * param->pksz);
    alignas(32) __m256 sdx =  _mm256_set1_ps(param->stpx);
    alignas(32) __m256 bias = _mm256_set_ps(7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);
    alignas(32) __m256 rmax = _mm256_set1_ps(param->rmax);


    alignas(32) __m256 y0 = _mm256_set1_ps(-1.0f);

    for (int y = 0; y < SDL_SCREEN_HEIGHT; y++, y0 = _mm256_add_ps(y0, dy))
    {

        alignas(32) __m256 x0 = _mm256_add_ps(_mm256_mul_ps(bias, sdx), _mm256_set1_ps(-1.f));

        for (int x = 0; x < SDL_SCREEN_WIDTH; x += param->pksz, x0 = _mm256_add_ps(x0, dx))
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

                    for (int i = 0; i < param->pksz; i++)
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

inline void Mandelbrot4(void * p, int * dots, void * m)
{
    int * dots_pointer = dots;
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

inline void Mandelbrot(void * p, int * dots, void * m)
{
    int * dots_pointer = dots;
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

 /*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

int SDL_Mandelbrot(void * p, int * dots)
{
    Mandelparam * param = (Mandelparam*) p;
    if (!SDL_Init(SDL_INIT_EVERYTHING)) return 1;

    SDL_Window * window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_SCREEN_WIDTH,
                                            SDL_SCREEN_HEIGHT,
                                            SDL_WINDOW_SHOWN);

    if (!window) return 1;

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event event;

    Transform mat = {1, 0.f, 0.f};

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:  quit = true;
                                break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                            mat.dely += 0.1 * mat.ampl;
                            break;
                        case SDL_SCANCODE_S:
                            mat.dely -= 0.1 * mat.ampl;
                            break;
                        case SDL_SCANCODE_D:
                            mat.delx -= 0.1 * mat.ampl;
                            break;
                        case SDL_SCANCODE_A:
                            mat.delx += 0.1 * mat.ampl;
                            break;

                        case SDL_SCANCODE_P:
                            mat.ampl *= 1.1;
                            break;

                        case SDL_SCANCODE_O:
                            mat.ampl /= 1.1;
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);

        param->func(param, dots, &mat);

        for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SDL_SCREEN_WIDTH; x++)
            {
                int it = dots[x + y * SDL_SCREEN_HEIGHT];
                SDL_RenderDrawPoint(renderer, x, y);
                SDL_SetRenderDrawColor(renderer, 19, it * 2 + 2, it * 5 + 1, SDL_ALPHA_OPAQUE);

            }
        }
        SDL_RenderPresent(renderer);
        memset(dots, 0, sizeof(int) * SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;

}

/*- - - - - - - - - - - - - - - - - - - - </> - - - - - - - - - - - - - - - - - - */

int mandel_param_init(Mandelparam * param, int pksz, float stpx, float stpy, float rmax, const char * tfile)
{
    param->pksz = pksz;
    param->stpx = stpx;
    param->stpy = stpy;
    param->rmax = rmax;
    param->tfile = tfile;
    return 0;
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
