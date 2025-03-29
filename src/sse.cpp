#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "../include/main.h"

#include <xmmintrin.h>

#include <SDL2/SDL.h>

#define GRAPHICS

#define z(array)  array[0], array[1], array[2], array[3]

int main(int argc, char * argv[])
{

/*---------------------------------------------------------------------------------------------------------*/

    int graph_flag = NO_GRAPH;
    if (argc >= 2)
    {
        if (!strcmp(argv[1], "--graph"))        {graph_flag = GRAPH;   LOG("set GRAPH flag"); }
        else if (!strcmp(argv[1], "--ngraph"))  {LOG("set NO_GRAPH flag");}

    }

/*--------------------------------------------------------------------------------------------------------*/

#ifdef GRAPHICS

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

#endif

/*--------------------------------------------------------------------------------------------------------*/

#ifdef GRAPHICS

    while (!quit)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) quit = true;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);

#endif

        for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SDL_SCREEN_WIDTH; x += 4)
            {

                __m128 x0 = _mm_set_ps( -1.f + x * 2.f / SDL_SCREEN_WIDTH,          -1.f + (x + 1) * 2.f / SDL_SCREEN_WIDTH,
                                -1.f + (x + 2) * 2.f / SDL_SCREEN_WIDTH,    -1.f + (x + 3) * 2.f / SDL_SCREEN_WIDTH);

                __m128 y0 = _mm_set_ps( -1.f + y * 2.f / SDL_SCREEN_WIDTH,          -1.f + y * 2.f / SDL_SCREEN_WIDTH,
                                -1.f + y * 2.f / SDL_SCREEN_WIDTH,          -1.f + y * 2.f / SDL_SCREEN_WIDTH);


                __m128 z_x = _mm_setzero_ps();
                __m128 z_y = _mm_setzero_ps();

                __m128 z_x2 = _mm_setzero_ps();
                __m128 z_y2 = _mm_setzero_ps();

                __m128 rmax = _mm_set_ps1(rMax);

                volatile int it = 0;


                for (; it < Niter; it++)
                {

                    z_y = _mm_mul_ps(z_y, z_x);
                    z_y = _mm_mul_ps(z_y, _mm_set_ps1(2));
                    z_y = _mm_add_ps(z_y, y0);

                    z_x = _mm_sub_ps(z_x2, z_y2);
                    z_x = _mm_add_ps(z_x, x0);

                    __m128 r2 = _mm_add_ps(z_x2, z_y2);
                    __m128 mask = _mm_cmple_ps(r2, rmax);

                    int mask2int = _mm_movemask_ps(mask);

                    if (mask2int == 0x00) break;

                    z_x2 = _mm_mul_ps(z_x, z_x);
                    z_y2 = _mm_mul_ps(z_y, z_y);


                }

        // #endif
#ifdef GRAPHICS
                SDL_SetRenderDrawColor(renderer, 0, it * 2 + 2,  it * 2 + 5, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y);
                SDL_RenderDrawPoint(renderer, x+1, y);
                SDL_RenderDrawPoint(renderer, x+2, y);
                SDL_RenderDrawPoint(renderer, x+3, y);

#endif
            }
    }
#ifdef GRAPHICS
        SDL_RenderPresent(renderer);
    }
#endif

#ifdef GRAPHICS

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

#endif

}
