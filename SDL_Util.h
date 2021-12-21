#pragma once
#include "Include.h"

#ifdef UtiliseSDL
extern int screenX;
extern int screenY;
extern SDL_Surface* screen;
extern SDL_Event event;

typedef Uint32 color;
extern color color_white;

extern int camX;
extern int camY;

void screen_load();
void screen_unload();
void FillRect(SDL_Rect* r, color c);
void FillCircle(SDL_Rect* r, color c);
void FillCircularSquare(SDL_Rect* r, color c, float pow);

SDL_Rect newRect(int x, int y, int w, int h);

void SDL_Afficher();

#define fontX 3
#define fontY 5

void writeDigit(int digit, float posX, float posY, float nbPixelY, color c);
void writeNumber(int value, float posX, float posY, float nbPixelY, color c);
int randNoSideEffect();
int randSignNoSideEffect();
#endif