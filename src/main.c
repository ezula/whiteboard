/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * main.c.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "linkedlist.h"
#include "shape.h"

#define STATE_NOTHING 0
#define STATE_DRAWING 1

int main(void) {
    int loop;
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int fpsClock;
    int realfps, fpscount, state;
    linked_list_t shapes;
    linked_list_t undoBuffer;
    Shape *shape;
    SDL_Point p;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Initialization error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    window = SDL_CreateWindow("Whiteboard", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    glEnable(GL_MULTISAMPLE);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST );
     
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    
    loop = 1;
    realfps = 0;
    fpscount = 0;

    ll_init(&shapes);
    state = 0;
    srand(time(NULL));

    fpsClock = time(NULL);

    while (loop) {

        SDL_WaitEvent(&event);

        do {
            switch (event.type) {
                case SDL_QUIT:
                    /* Quit the application */
                    loop = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        shape = malloc(sizeof(Shape));
                        initShape(shape);
                        ll_add(&shapes, shape);
                        shape->color.r = rand() % 255;
                        shape->color.g = rand() % 255;
                        shape->color.b = rand() % 255;
                        shape->color.a = 255;
                        shape->thickness = 5;
                        state = STATE_DRAWING;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        state = STATE_NOTHING;
                        removeRedundantPoints(shape);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (state == STATE_DRAWING) {
                        p.x = event.motion.x;
                        p.y = event.motion.y;
                        addPoint(shape, p);
                    }
                    break;
            }
        } while(SDL_PollEvent(&event));

        // calculate fps
        fpscount++;
        if (time(NULL) > fpsClock) {
            realfps = fpscount;  
            fpscount = 0; 
            fpsClock = time(NULL);
        }

        printf("FPS: %d\n", realfps);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        ll_traverse(&shapes, renderer, drawShapeCallback);
        SDL_RenderPresent(renderer);
    }
    ll_traverse(&shapes, NULL, freeShapeCallback);
    ll_free(&shapes);

    //Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
