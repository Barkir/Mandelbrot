#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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

    if (!SDL_Init(SDL_INIT_EVERYTHING)) return 1;

    SDL_Window * window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_SCREEN_WIDTH,
                                            SDL_SCREEN_HEIGHT,
                                            SDL_WINDOW_SHOWN);

    if (!window) return 1;

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("consolas.ttf", 24);
    if (!font)
        printf("Failed to load font! Using simple rendering.\n");

    bool quit = false;
    SDL_Event event;

    Uint32 fps_lasttime = SDL_GetTicks();
    Uint32 fps_frames = 0;
    char fps_text[128];

    while (!quit)
    {
        fps_frames++;
        if (fps_lasttime < SDL_GetTicks() - 1000)
        {
            float fps = fps_frames / ((SDL_GetTicks() - fps_lasttime) / 1000.0f);
            snprintf(fps_text, sizeof(fps_text), "FPS: %.1f | Zoom: %.3f | Pos: (%.3f, %.3f)",
                    fps,
                    1.0f/param->TransMat.ampl,
                    param->TransMat.delx,
                    param->TransMat.dely);
            SDL_SetWindowTitle(window, fps_text);
            fps_lasttime = SDL_GetTicks();
            fps_frames = 0;
        }

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
                            param->TransMat.dely += 0.1 * param->TransMat.ampl;
                            break;
                        case SDL_SCANCODE_S:
                            param->TransMat.dely -= 0.1 * param->TransMat.ampl;
                            break;
                        case SDL_SCANCODE_D:
                            param->TransMat.delx -= 0.1 * param->TransMat.ampl;
                            break;
                        case SDL_SCANCODE_A:
                            param->TransMat.delx += 0.1 * param->TransMat.ampl;
                            break;

                        case SDL_SCANCODE_P:
                            param->TransMat.ampl *= 1.1;
                            break;

                        case SDL_SCANCODE_O:
                            param->TransMat.ampl /= 1.1;
                            break;

                        default: break;
                    }

                default: break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        param->Function(param, dots);

        for (int y = 0; y < SDL_SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SDL_SCREEN_WIDTH; x++)
            {
                int it = dots[x + y * SDL_SCREEN_HEIGHT];
                SDL_SetRenderDrawColor(renderer, 19, it * 2 + 2, it * 5 + 1, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        if (font)
        {
            SDL_Color white = {255, 255, 255, 255};
            SDL_Color red = {255, 0, 0, 255};
            SDL_Color bg = {0, 0, 0, 128};

            SDL_Surface* text_surface = TTF_RenderText_Shaded(font, fps_text, white, bg);
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

            SDL_Rect text_rect = {10, 10, text_surface->w, text_surface->h};

            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(text_texture);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_Rect bg_rect = {10, 10, 50, 30};
            SDL_RenderFillRect(renderer, &bg_rect);

        }

        SDL_RenderPresent(renderer);
        memset(dots, 0, sizeof(int) * SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT);
    }

    if (font) {
        TTF_CloseFont(font);
        TTF_Quit();
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
