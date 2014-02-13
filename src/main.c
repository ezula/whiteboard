/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * main.c.
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

int main(void)
{
	printf("hej");

    int gogogo = 1;
    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindow("Hello World!", 100, 100, 640, 480,SDL_WINDOW_SHOWN);
    while (gogogo) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            gogogo = 0;
    }
    SDL_Quit();
    return 0;
}
