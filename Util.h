#pragma once
#include "Include.h"
 
extern bool quitteLeJeu;

int mini(int a, int b);
int maxi(int a, int b);
int rng(int min, int max);

// smart malloc :)
void* smalloc(size_t size);

void quitter(int codeErreur);
void quitteSiBesoin();

void HSVtoRGB(float H, float S, float V, int* R, int* G, int* B);

int nbDigit(int number);
int faireTendre(int x, int cible);