#include "Font.h"

#ifdef UtiliseSDL
SDL_Surface* font;

int font_mesureX(string msg)
{
    return strlen(msg) * font_spacing_x;
}

// '\\' = revenir a la ligne
void font_write(string msg, int posX, int posY)
{
    int i = 0;
    int x = posX;
    int y = posY;

    while (msg[i] != '\0')
    {
        font_write_char(msg[i], x, y);
        x += font_spacing_x;
        if (x + font_spacing_x > screenX || msg[i] == '\\')
        {
            x = posX;
            y += font_spacing_y;
        }
        i++;
    }
}

void font_write_center(string msg, float center, int posX, int posY)
{
    font_write(msg, posX - font_mesureX(msg) * center- font_spacing_x/2, posY);
}

void font_write_char(char c, int posX, int posY)
{
    if (c > ' ' && c != '\\')
    {
        SDL_Rect mask = newRect((c % 16) * font_rectangle_size, (c / 16) * font_rectangle_size, font_rectangle_size, font_rectangle_size);
        SDL_Rect pos = newRect(posX, posY, font_rectangle_size, font_rectangle_size);
        SDL_BlitSurface(font, &mask, screen, &pos);
    }
}

void font_load()
{
    printf("Debut du chargement de la police d'ecriture\n");
    font = SDL_LoadBMP("font.bmp");
    SDL_SetColorKey(font, SDL_SRCCOLORKEY, SDL_MapRGB(font->format, 0, 148, 255));
    printf("Fin\n");
}

void font_unload()
{
    if (font != null)
    {
        SDL_FreeSurface(font);
    }
}
#endif