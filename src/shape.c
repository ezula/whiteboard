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
}

void drawShapeCallback(void * renderer, void * shape) {
    Shape *s = (Shape *)shape;
    if (s->count < 1) return;
    //glEnable(GL_LINE_SMOOTH);
    SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, s->color.a);
    //Draw a single point so that SDL sets up the renderer correctly.
    //Otherwise, the proper openGL functions are not called.    
    SDL_RenderDrawPoint(renderer, s->points[0].x, s->points[0].y);

    drawLine(s);
}

int freeShapeCallback(void * shape) {
    Shape *s = (Shape *)shape;
    free(s->points);
    free(s);
    return 1;
}

typedef struct SDL_FPoint {
    float x;
    float y;
} SDL_FPoint;
// v0 and v1 are normalized
// t can vary between 0 and 1
// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
SDL_FPoint slerp2d(const SDL_FPoint *v0, const SDL_FPoint *v1, float t) {
    float dot = v0->x * v1->x + v0->y * v1->y; //Dot product.
    if( dot < -1.0f ) dot = -1.0f;
    if( dot > 1.0f ) dot = 1.0f;

    float theta_0 = acos(dot);
    float theta = theta_0 * t;

    SDL_FPoint v2;
    v2.x = -v0->y;
    v2.y = v0->x;

    float sin_t = sin(theta);
    float cos_t = cos(theta);

    SDL_FPoint ret;
    ret.x = v0->x*cos_t + v2.x*sin_t;
    ret.y = v0->y*cos_t + v2.y*sin_t;
    return ret;
}

void normalize(SDL_FPoint *p) {
    float len = sqrt(p->x * p->x + p->y * p->y); //too slow?
    float invlen = 1 / len;
    p->x = p->x * invlen;
    p->y = p->y * invlen;
}

void drawLine(const Shape *shape) {
    if(shape->count < 2) return;
    size_t i, j;
    float w = shape->thickness / 2.0f;
    SDL_FPoint a, a_perp, b, b_perp, p0, p1, p2, p3;
    const SDL_Point *cur, *nxt, *prv;

    glBegin(GL_TRIANGLES);
    for(i = 0; i < shape->count-1; ++i) {
        cur = &shape->points[i];
        nxt = &shape->points[i+1];

        b.x = nxt->x - cur->x;
        b.y = nxt->y - cur->y;
        normalize(&b);
        b_perp.x = -b.y;
        b_perp.y = b.x;

        p0.x = cur->x + b_perp.x*w;
        p0.y = cur->y + b_perp.y*w;

        p1.x = cur->x - b_perp.x*w;
        p1.y = cur->y - b_perp.y*w;

        p2.x = nxt->x + b_perp.x*w;
        p2.y = nxt->y + b_perp.y*w;

        p3.x = nxt->x - b_perp.x*w;
        p3.y = nxt->y - b_perp.y*w;

        // first triangle
        glVertex2f(p0.x, p0.y);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        // second triangle
        glVertex2f(p2.x, p2.y);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p3.x, p3.y);

        // only do joins when we have a prv
        if(i == 0) continue;

        prv = &shape->points[i-1];
        a.x = prv->x - cur->x;
        a.y = prv->y - cur->y;
        normalize(&a);
        a_perp.x = a.y;
        a_perp.y = -a.x;

        float det = a.x*b.y - b.x*a.y;
        if(det <= 0) {
            a_perp.x = -a_perp.x;
            a_perp.y = -a_perp.y;
            b_perp.x = -b_perp.x;
            b_perp.y = -b_perp.y;
        }

        // TODO: do inner miter calculation

        size_t num_pts = 4;
        SDL_FPoint *round = malloc((1 + num_pts + 1)*sizeof(SDL_Point));
        for(j = 0; j <= num_pts+1; ++j) {
            float t = (float)j/(float)(num_pts+1);
            SDL_FPoint slerp;
            if(det > 0)
                slerp = slerp2d(&b_perp, &a_perp, 1.0f-t);
            else 
                slerp = slerp2d(&a_perp, &b_perp, t);
            round[j].x = cur->x + (slerp.x * w);
            round[j].y = cur->y + (slerp.y * w);
        }

        for(j = 0; j < num_pts+1; ++j)
        {
            glVertex2i(cur->x, cur->y);
            if(det > 0) {
                glVertex2f(round[j+1].x, round[j+1].y);
                glVertex2f(round[j+0].x, round[j+0].y);
            }
            else {
                glVertex2f(round[j+0].x, round[j+0].y);
                glVertex2f(round[j+1].x, round[j+1].y);
            }
        }
        free(round);
    }
    glEnd();
}