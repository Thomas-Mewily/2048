#pragma once 
#include "Include.h"

// ================= Macros : =========

// La touche q n'a pas la meme valeur sur les clavier qwerty et azerty, et la sdl utilise un clavier qwerty
#define SDLK_q_azerty 97
#define SDLK_z_azerty 119

#define TuileVideId -1

// ================= Structures et Enumerations : =========

typedef enum
{
    MOUV_PasEncoreChoisi = -1,
    MOUV_Haut            = 0,
    MOUV_Bas             = 1,
    MOUV_Gauche          = 2,
    MOUV_Droite          = 3,
} mouvement;

typedef enum
{
    MENU_ActionPasEncoreChoisi  = -1,
    MENU_ReprendrePartie        = 0,
    MENU_NouvellePartie         = 1,
    MENU_RegarderDernierePartie = 3, // meme valeur que MENU_RegarderRecord car même bouton
    MENU_RegarderRecord         = 3,
    MENU_Credits                = 4,
    MENU_Ok                     = 5, // Le joueur a pris connaissance du menu (game over)
} menuAction;

typedef enum
{
    SCENE_MenuPrincipal = 0,
    SCENE_Credits       = 1,
    SCENE_EnJeu         = 2,
    SCENE_GameOver      = 3,
} scene;

typedef struct 
{
    int puissance;            // valeur = 1 << puissance
    int dernierTourCombiner;  // par default : -1. -2 : vient juste d'apparaitre

    float posXAffichage;      // Index x par defaut
    float posYAffichage;      // Index y par defaut

    float colorR;
    float colorG;
    float colorB;
} bloc;

struct level_struct
{
    bloc** blocs;
    int score;

    int tailleX;
    int tailleY;

    int seed;
    int tour;

    mouvement(*LireMouvement)(struct level_struct* l);
    int SdlTickDebutTour;
    int SdlTickChargementNiveau;
    int SdlTickAvantGameOver; // vaut SDL_GetTick() jusqu'au game over (pour arreter le chrono)

    FILE* save;
};
typedef struct level_struct level;

// ================= Score : =========

void getBestScore();
void saveBestScore(level* l);

// ================= Mouvement : =========

char mouvementToChar(mouvement m);
mouvement charToMouvement(char c);

mouvement lireMouvementZQSD();
mouvement lireMouvementConsoleZQSD(level* l);

#ifdef  UtiliseSDL
mouvement lireMouvementSDL(level* l);
#endif
mouvement lireMouvementEntree(level* l);
mouvement lireMouvementReplay(level* l);
mouvement lireMouvementReplayAvecInput(level* l);
mouvement attendreEtLireMouvementReplay(level* l);
// ================= Chargement / Dechargement Niveau : =========

level* nouveauLevel(int tailleX, int tailleY);
level* levelDepuisFichier(FILE* f, bool* reussi);
level* levelDepuisChemin(string s, bool* reussi);
level* chargerSauvegarde(string path);
level* defaultLevel();
void sauvegarderLevel(string path, level* l);

void levelSauvegardeConfigurationInitiale(level* l);
void initLevel(level* l);

void dechargerLevelSaufFichier(level* l);
void dechargerLevel(level* l);

// ================= Logique du jeu : =========

// Retourne : EstGameOver
bool tourDeJeu(level* l);
void boucleDeJeu(level* l);

bool dansLeNiveau(level* l, int x, int y);
bool isGameOver(level* l);

int compterTuileVide(level* l);
bool placerBlocAleatoire(level* l, int puissance);

// Retourne le nombre d'action (bloc qui ont été bouger/combiner) :
int deplacerBloc(level* l, int x, int y, int xFinal, int yFinal, int blocValue);
int appliquerLeMouvementSurLeBloc(level* l, int x, int y, int mX, int mY);
int appliquerMouvement(level* l, mouvement m);

void entrePourQuitterLeJeuUniquement();
void basculerSurLesEntreeDuJoueur(level* l);
void visionner(level* l);
bool estEnTrainDeVisionner(level* l);

// ================= Affichage : =========

void printLevelConsole(level* l);
void printLevelInfo(level* l);
void printLevelGrille(level* l);
void printLevelSDL(level* l);

// ================= Scene : =========

menuAction LireEntreMenu(level* l);
void sceneInit();
void changeScene(scene s);

void sceneMenuPrincipal();
void sceneGameOver();
void sceneCredits();
void sceneEnJeu();

int main(int argc, char* argv[]);