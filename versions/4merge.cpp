#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "../include/main.h"

#include <SDL2/SDL.h>

// #define GRAPHICS

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

    for (int i = 0; i < 20; i++)
    {
    // #ifdef GRAPHICS
        for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SDL_SCREEN_WIDTH; x += 4)
            {
                float x0[4] = { -1.f + x * 2.f / SDL_SCREEN_WIDTH,          -1.f + (x + 1) * 2.f / SDL_SCREEN_WIDTH,
                                -1.f + (x + 2) * 2.f / SDL_SCREEN_WIDTH,    -1.f + (x + 3) * 2.f / SDL_SCREEN_WIDTH};

                float y0[4] = { -1.f + y * 2.f / SDL_SCREEN_WIDTH,          -1.f + y * 2.f / SDL_SCREEN_WIDTH,
                                -1.f + y * 2.f / SDL_SCREEN_WIDTH,          -1.f + y * 2.f / SDL_SCREEN_WIDTH};


                float z_x[4] = {0.f, 0.f, 0.f, 0.f};
                float z_y[4] = {0.f, 0.f, 0.f, 0.f};

                float z_x2[4] = {0.f, 0.f, 0.f, 0.f};
                float z_y2[4] = {0.f, 0.f, 0.f, 0.f};

                float r4[4] = {0.f, 0.f, 0.f, 0.f};
                float rmax[4] = {4.f, 4.f, 4.f, 4.f};
                float mask[4] = {};

                volatile int it = 0;


                for (; it < Niter; it++)
                {

                    mul4(z_y, z_x, z_y);
                    mulnum(z_y, 2, z_y);
                    add4(z_y, y0, z_y);

                    sub4(z_x2, z_y2, z_x);
                    add4(z_x, x0, z_x);

                    add4(z_x2, z_y2, r4);

                    cmple4(r4, rmax, mask);

                    if (isnull4(mask)) break;

                    mul4(z_x, z_x, z_x2);
                    mul4(z_y, z_y, z_y2);

                }

                // LOG("iter = %d", it);

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

// TODO: inline in header

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
