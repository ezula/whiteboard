#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

typedef struct Shape {
    SDL_Point *points;
    SDL_Color color;
    int thickness;
    int count;
    int capacity;
} Shape;

void initShape(Shape *shape);
int sqDistance(const SDL_Point *p1, const SDL_Point *p2);
void removeRedundantPoints(Shape *shape);
void addPoint(Shape *shape, const SDL_Point point);
void drawShapeCallback(void * renderer, void * shape);
int freeShapeCallback(void * shape);