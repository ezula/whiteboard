/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * main.c.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
	printf("hej");

    int loop;
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect rect;
    int fpsClock;
    int realfps, fpscount;

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Hello World!", 100, 100, 640, 480,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    rect.x = 100;
    rect.y = 100;
    rect.w = 100;
    rect.h = 100;

    loop = 1;
    realfps = 0;
    fpscount = 0;

    fpsClock = time(NULL);
    while (loop) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            loop = 0;


        // calculate fps
        fpscount++;
        if (time(NULL) > fpsClock) {
            realfps = fpscount;  
            fpscount = 0; 
            fpsClock = time(NULL);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderDrawText()
        SDL_RenderPresent(renderer);
    }
    SDL_Quit();
    return 0;
}
