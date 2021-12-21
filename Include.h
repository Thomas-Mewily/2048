#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "UtiliseSDL.h"

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <float.h>

#ifdef  UtiliseSDL
#include <SDL/SDL.h>
//#include "SDL/include/SDL.h"
#endif 

// Enjoy my macros :) - Thomas T / Mewily
// Toutes ces macros sont en minuscule car il s'agit de nouveaux mots cles (extension du langage)

// ================= Constante : =========
#define true  1
#define false 0
#define null NULL
#define pi 3.14159265

// ================= Typedef : =========
typedef char* string;
// Pas un typedef car bool est deja defini en C++
#define bool int

// ================= Boucles : =========

// Tant Que :
#define until(cond) while(!(cond))

// Repeter Indefiniment
#define forever while(true)

// Boucle for de [0, _nbTime[
#define repeat(_varName, _nbTime) for(int _varName = 0; _varName < (_nbTime); _varName++)

// boucle for de ]_nbTime, 0]
#define repeat_reverse(_varName, _nbTime) for(int _varName = (_nbTime)-1; _varName >= 0; _varName--)

// Ok celle la est est pas hyper utile
#define twice(instruction) instruction instruction

#include "Util.h"
#include "Font.h"

#ifdef  UtiliseSDL
#include "SDL_Util.h"
#endif 