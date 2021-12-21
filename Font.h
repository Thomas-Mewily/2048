#pragma once
#include "Include.h"

#ifdef UtiliseSDL
#define font_rectangle_size 40
#define font_spacing_x 16
#define font_spacing_y 18

int font_mesureX(string msg);

void font_write(string msg, int posX, int posY);
void font_write_char(char c, int posX, int posY);
void font_write_center(string msg, float center /* [0;1] 0.5 = centree, 0 : gauche, 1 : droite*/, int posX, int posY);

void font_load();
void font_unload();
#endif