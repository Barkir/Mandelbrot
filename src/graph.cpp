#include <stdlib.h>
#include <SDL2/SDL.h>

#include "../include/graph.h"
#include "../include/main.h"
#include "../include/process_cmd.h"



int SDLMandelbrotExecute(void * paramPointer)
{
    MandelSettings * param = (MandelSettings*) paramPointer;
    int * dots = (int*) aligned_alloc(param->PackSize * 4, SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT * sizeof(int));
    if (!dots) return MandelError(MANDEL_MEMALLOC_ERROR);
    SDLMandelbrot(param, dots);

    free(dots);

}

int SDLMandelbrot(void * paramPointer, int * dots)
{
    MandelSettings * param = (MandelSettings*) paramPointer;
    Transform mat = param->TransMat;

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

        param->Function(param, dots);

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
