#include "SDL_Util.h"

#ifdef UtiliseSDL

int screenX;
int screenY;
SDL_Surface* screen;
SDL_Event event;

typedef Uint32 color;
color color_white;

int camX = 0;
int camY = 0;

SDL_Surface* icon;

void screen_load()
{
    screenX = 1080 / 2;
    screenY = 1080 / 2;


    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("\nSDL_Init Error : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption("2048 ("__TIME__")", NULL);

    icon = SDL_LoadBMP("icon.bmp");

    SDL_WM_SetIcon(icon, NULL);

    screen = NULL;

    // SDL_RESIZABLE | SDL_FULLSCREEN
    if ((screen = SDL_SetVideoMode(screenX, screenY, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
    {
        printf("SDL_SetVideoMode Error %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    color_white = SDL_MapRGB(screen->format, 255, 255, 255);
}

void screen_unload()
{
    if(icon != null)
    {
        SDL_FreeSurface(icon);
    }
}

#define CirclePixelPrecision 1

void FillCircularSquare(SDL_Rect* r, color c, float pow)
{
    if (r != null)
    {
        SDL_Rect a = *r;
        a.w = CirclePixelPrecision;
        int nbTime = (r->w) / CirclePixelPrecision;

        repeat(i, nbTime)
        {
            float angle = i / (float)nbTime;
            float hcoef = powf((1 - powf((angle - 0.5) * 2, 2)), pow);
            a.h = hcoef * r->h;
            a.y = r->y + (1 - hcoef) * r->h * 0.5;

            SDL_FillRect(screen, &a, c);

            a.x += CirclePixelPrecision;
        }
    }
}


void FillCircle(SDL_Rect* r, color c)
{
    FillCircularSquare(r, c, 0.5);
}

void FillRect(SDL_Rect* r, color c)
{
    if (r != null)
    {
        r->x -= camX;
        r->y -= camY;
    }
    SDL_FillRect(screen, r, c);
}

SDL_Rect newRect(int x, int y, int w, int h)
{
    return (SDL_Rect) { .x = x, .y = y, .w = w, .h = h };
}

void SDL_Afficher()
{
    SDL_Flip(screen);
}



void writeDigit(int digit, float posX, float posY, float nbPixelY, color c)
{
    SDL_Rect r;

    // Booleen pour les ligne
    bool h  = false; // Haut
    bool m  = false; // Milieu
    bool b  = false; // Bas

    bool hg = false; // Haut Gauche
    bool bg = false; // Bas  Gauche

    bool hd = false; // Haut Droite
    bool bd = false; // Bas  Droite
    
    // Affichage 7 segments pour pouvoir scaler le nombre a l'affichage
    switch (digit)
    {
        case 0: h  = b  = hg = bg = hd = bd =       true; break;
        case 1: hd = bd =                           true; break;
        case 2: h  = b  = bg = hd = m  =            true; break;
        case 3: h  = b  = bd = hd = m  =            true; break;
        case 4: hg = m  = hd = bd =                 true; break;
        case 5: h  = b  = bd = hg = m  =            true; break;
        case 6: h  = b  = bd = hg = m  = bg =       true; break;
        case 7: hd = bd = h  =                      true; break;
        case 8: h  = b  = hg = bg = hd = bd = m =   true; break;
        case 9: h  = b  = hg = hd = bd = m  =       true; break;
    }

    float nbPixelX = nbPixelY * fontX / fontY;

    if (h)
    {
        r = newRect(posX, posY, nbPixelX, nbPixelY / fontY);
        FillRect(&r, c);
    }
    if (m)
    {
        r = newRect(posX + nbPixelX * 1 / fontX - 1, posY + nbPixelY * 2 / fontY, nbPixelX / fontX + 2, nbPixelY / fontY);
        FillRect(&r, c);
    }
    if (b)
    {
        r = newRect(posX, posY + nbPixelY * 4 / fontY, nbPixelX, nbPixelY / fontY);
        FillRect(&r, c);
    }
    if (hg)
    {
        r = newRect(posX, posY, nbPixelX / fontX, nbPixelY * 3 / fontY);
        FillRect(&r, c);
    }
    if (hd)
    {
        r = newRect(posX + nbPixelX * 2 / fontX, posY, nbPixelX / fontX, nbPixelY * 3 / fontY);
        FillRect(&r, c);
    }
    if (bg)
    {
        r = newRect(posX, posY + nbPixelY * 2 / fontY, nbPixelX / fontX, nbPixelY * 3 / fontY);
        FillRect(&r, c);
    }
    if (bd)
    {
        r = newRect(posX + nbPixelX * 2 / fontX, posY + nbPixelY * 2 / fontY, nbPixelX / fontX, nbPixelY * 3 / fontY);
        FillRect(&r, c);
    }
}

void writeNumber(int value, float posX, float posY, float nbPixelY, color c)
{
    float pxPerDigit = nbPixelY * 4 / 5;
    posX += nbDigit(value) * pxPerDigit / 2 - pxPerDigit;
    while (value != 0)
    {
        writeDigit(value % 10, posX, posY, nbPixelY, c);
        posX -= pxPerDigit;
        value /= 10;
    }
}

int randNoSideEffect()
{
    static int rng = 0;
    int tick = SDL_GetTicks();
    rng = abs(tick * 17 * (rng + 3));
    return rng;
}
int randSignNoSideEffect() { return randNoSideEffect() % 2 * 2 - 1; }
#endif