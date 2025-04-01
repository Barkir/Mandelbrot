#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "../include/main.h"

#include <SDL2/SDL.h>

#define GRAPHICS

int main(int argc, char * argv[])
{

/*---------------------------------------------------------------------------------------------------------*/

    int graph_flag = SSE_NGRAPH;
    if (argc >= 2)
    {
        if (!strcmp(argv[1], "--graph"))        {graph_flag = SSE_GRAPH;   LOG("set GRAPH flag"); }
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
        for (int y = 0; y < SDL_SCREEN_WIDTH; y++)
        {
            for (int x = 0; x < SDL_SCREEN_HEIGHT; x++)
            {
                float x0 = (-1.f + x * 2.f / SDL_SCREEN_WIDTH);
                float y0 = (-1.f + y * 2.f / SDL_SCREEN_HEIGHT);

                float z_x = 0.f, z_y = 0.f;
                float z_x2 = 0.f, z_y2 = 0.f;

                int it = 0;

                for (; it < Niter && z_x2 + z_y2 < rMax; it++)
                {
                    z_y = 2 * z_x * z_y + y0;
                    z_x = z_x2 - z_y2 + x0;

                    z_x2 = z_x * z_x;
                    z_y2 = z_y * z_y;
                }
#ifdef GRAPHICS
                SDL_SetRenderDrawColor(renderer, 0, it * 2 + 2,  it * 2 + 5, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, (x), (y));
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
