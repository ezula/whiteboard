/* Whiteboard
 * Authors: Mikael Svens and Henrik Alnestig
 * Date: 2014-02-13
 *
 * main.c.
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "linkedlist.h"
#include "SDL2_gfx/SDL2_gfxPrimitives.h"

#define STATE_NOTHING 0
#define STATE_DRAWING 1

typedef struct Shape {
    SDL_Point *points;
    SDL_Color color;
    int thickness;
    int count;
    int capacity;
} Shape;

void initShape(Shape *shape) {
    shape->capacity = 50; //Start off with 50 points.
    shape->points = malloc((shape->capacity)*sizeof(SDL_Point)); 
    shape->count = 0;
}

int sqDistance(const SDL_Point *p1, const SDL_Point *p2) {
    return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

void removeRedundantPoints(Shape *shape) {
    SDL_Point *p;
    SDL_Point *q;
    SDL_Point *r;
    int i, j, det, pr, pq, qr, isLastRedundant, removed;

    isLastRedundant = 0;
    removed = 0;

    for (i = 2; i < shape->count; ++i) {
        //Check if r is on the line made up by p and q.
        p = &shape->points[i - 2];
        q = &shape->points[i - 1];
        r = &shape->points[i];
        //Calculate determinant as described here: http://stackoverflow.com/a/907491
        det = (q->x - p->x) * (r->y - p->y) - (q->y - p->y) * (r->x - p->x);
        pr = sqDistance(p, r);
        pq = sqDistance(p, q);
        qr = sqDistance(q, r);
        if (det == 0 && pr > pq && pr > qr) { //q needs to be BETWEEN p and r
            if (isLastRedundant) {
                //q of previous round, which is now p, is unneccesary.
                //Mark it so by e.g. setting its x value to -1.
                p->x = -1;
                removed++;
            }

            //We can't remove a redundant point on the same round it was found,
            //since the NEXT point needs to be redundant as well.
            isLastRedundant = 1;
        }
        else
            isLastRedundant = 0;
    }

    //Make a new list containting exactly all needed points with no extra space.
    SDL_Point *newPoints = malloc((shape->count - removed)*sizeof(SDL_Point));
    j = 0;
    for (i = 0; i < shape->count; ++i) {
        if (shape->points[i].x != -1) {
            newPoints[j] = shape->points[i];
            j++;
        }
    }
    free(shape->points);
    shape->points = newPoints;
    shape->count -= removed;
    shape->capacity = shape->count;
}

void addPoint(Shape *shape, const SDL_Point point) {
    if (shape->capacity == shape->count) { //If it's full
        SDL_Point *newPoints = malloc(2*(shape->capacity)*sizeof(SDL_Point));
        memcpy(newPoints, shape->points, shape->count*sizeof(SDL_Point));
        free(shape->points);
        shape->points = newPoints;
        shape->capacity *= 2; //Capacity doubled.
    }
    shape->points[shape->count] = point;
    shape->count++;
    printf("Count: %d\n", shape->count);
}

void drawShapeCallback(void * renderer, void * shape) {
    Shape *s = (Shape *)shape;
    int i;
    SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, s->color.a);
    
    glLineWidth(5);
    SDL_RenderDrawLines(renderer, s->points, s->count);
/*   for (i = 0; i < s->count - 1; ++i) {
       thickLineRGBA(renderer, s->points[i].x, s->points[i].y, s->points[i+1].x, 
           s->points[i+1].y, s->thickness, s->color.r, s->color.g, s->color.b, s->color.a);
       filledCircleRGBA(renderer, s->points[i].x, s->points[i].y, s->thickness * 0.5, 
           s->color.r, s->color.g, s->color.b, s->color.a);
   }
   filledCircleRGBA(renderer, s->points[i].x, s->points[i].y, s->thickness * 0.5, s->color.r, s->color.g, s->color.b, s->color.a);
*/}

void freeShapeCallback(void * null, void * shape) {
    Shape *s = (Shape *)shape;
    free(s->points);
    free(s);
}

int main(void) {
	printf("hej\n");

    int loop;
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    int fpsClock;
    int realfps, fpscount, state;
    linked_list_t shapes;
    Shape *shape;
    SDL_Point p;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Initialization error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Whiteboard", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
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
                        shape->thickness = 4;
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

        // if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(1)) {
        //     printf("Janne!\n");
        // }


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
