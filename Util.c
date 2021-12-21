#include "Util.h"

bool quitteLeJeu = false;

int mini(int a, int b) { return a < b ? a : b; }
int maxi(int a, int b) { return a > b ? a : b; }
int rng(int min, int max) { return rand() % (max - min + 1) + min; }

// Securized Malloc : Malloc Securise, plus besoin de regarder 
// si le malloc est null en appelant cette fonction
void* smalloc(size_t size)
{
    void* p = malloc(size);
    if (p == null)
    {
        printf("Un malloc de %zu octets s'est mal passé, mais voilà une blague pour oublier le crash :\nQuel est le combre pour un diabetique en informatique ?\n D'aimer le sucre syntaxique !", size);
        // Free automatiquement les autres mallocs
        quitter(EXIT_FAILURE);
    }
    return p;
}

void quitter(int codeErreur)
{
    printf("Quitte avec le code d'erreur : %i\n", codeErreur);
#ifdef  UtiliseSDL
    screen_unload();
    font_unload();
    SDL_Quit();
#endif
    exit(codeErreur);
}

void quitteSiBesoin()
{
    if(quitteLeJeu)
    {
        quitter(EXIT_SUCCESS);
    }
}


//Thank to https://www.codespeedy.com/hsv-to-rgb-in-cpp/
// H : [0; 360]
// S : [0; 100]
// V : [0; 100]
void HSVtoRGB(float H, float S, float V, int* R, int* G, int* B) 
{
    if (H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0)  { return; }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float f = fmodf(H / 60.0, 2);
    float X = C * (1 - fabsf(f - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    *R = (r + m) * 255;
    *G = (g + m) * 255;
    *B = (b + m) * 255;
}

int nbDigit(int number)
{
    int count = 0;
    do
    {
        count += 1;
        number /= 10;
    } while (number != 0);
    return count;
}

int faireTendre(int x, int cible)
{
    if (x == cible) { return cible; }
    return x < cible ? x + 1 : x - 1;
}