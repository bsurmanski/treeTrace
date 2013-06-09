/**
 * main.c
 * treeTrace
 * December 5, 2011
 * Brandon Surmanski
 */

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>

#include "list.h"
#include "noise.h"

#define PI 3.141592654

typedef struct {
    float x;
    float y;
    float angle;
    int life;
} Tracer;

List *tracer_list;

SDL_Surface *s;

void *getPixelAt(SDL_Surface *s, int x, int y)
{
    return s->pixels + x * s->format->BytesPerPixel + y * s->pitch;
}

void setPixel(SDL_Surface *s, int x, int y, int val)
{
    if(x >= 0 && y >= 0 && x < s->w && y < s->h)
        memcpy(getPixelAt(s,x,y), &val, s->format->BytesPerPixel);
}

Tracer *create_tracer(Tracer *parent)
{
    Tracer *t = malloc(sizeof(Tracer));
    if(parent){
    t->x = parent->x;
    t->y = parent->y;
    t->angle = parent->angle  + 1.0f - (rand()%500)/250.0f;
    t->life = parent->life * (rand()%200)/200;
    } else {
        t->x = s->w / 2;
        t->y = s->h * 7/8;
        t->angle = 0;
        t->life = s->h * 6/8;
    }
    return t;
}

void tracer_update(Tracer *t)
{
    t->y-= cos(t->angle);//+noise_perlin1(t->x/10);
    t->x+= sin(t->angle)+noise_perlin1(t->y/10);
    t->life--;
    float i=0, j=0;
    //for(i = -t->life/16.0f * cos(t->angle), j = -t->life/16.0f * sin(t->angle);
    //        i<t->life/16.0f * cos(t->angle) && j<t->life/16.0f * sin(t->angle);
    //        i+=sin(t->angle), j+=cos(t->angle)){
    //for(i=-t->life/16;i<t->life/16; i++)
        setPixel(s, (int)t->x + i, (int)t->y + j, 0xFFFFFFFF);
    //}
    if(rand() % 100 >= 85 && t->y < s->h*5/8)//generate child
    {
        Tracer *new = create_tracer(t);
        //if(rand() % 100 >= 50) //child deflects parent branch
        //    t->angle = 2*PI - new->angle;
        list_append(tracer_list, new);
    }
}

void update(void)
{
    Node *n = list_first_node(tracer_list);
    while(n){
        Tracer *t = (Tracer*)node_value(n);
        tracer_update(t);
        if(t->life <= 0){
            //draw leaf
            //setPixel(s, t->x, t->y, 0xFF00FF00);
            //remove node
            Node *next = node_next(n);
            list_remove(tracer_list, n);
            n = next;
        }else {
            n = node_next(n);
        }
    }
}

void draw(void)
{

}

int handleEvents()
{
    SDL_PumpEvents();
    uint8_t *keyState = SDL_GetKeyState(0);
    if(keyState[SDLK_SPACE])
        return 1;
    return 0;
}

void run(void)
{
    srand(time(0));
    int quit = 0;
    tracer_list = list_create(sizeof(Tracer));
    list_append(tracer_list, create_tracer(0));
    while(!quit){
        quit = handleEvents();
        update();
        draw();
        SDL_Flip(s);
        SDL_Delay(16);
    }
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    s = SDL_SetVideoMode(320, 240, 0, SDL_SWSURFACE);
    run();
}
