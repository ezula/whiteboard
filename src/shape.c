#include <stdio.h>
#include <stdlib.h>
#include "shape.h"

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

    //glEnable(GL_LINE_SMOOTH);
    SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, s->color.a);
    glLineWidth(s->thickness);
    SDL_RenderDrawLines(renderer, s->points, s->count);
}

void freeShapeCallback(void * null, void * shape) {
    Shape *s = (Shape *)shape;
    free(s->points);
    free(s);
}