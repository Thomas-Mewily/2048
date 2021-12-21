#include "CombineBloc.h"

// Jeu du 2048
// Programmer par Thomas TAMAGNAUD & Reynalde SEGERIE

#pragma region Variables
int color_grid_red = 40;
long Frame = 0;
int bestScore = 0;

string path_last_level = "./last_level.txt";
string path_record_level = "./best_score.txt";
bool chargeUneSauvegarde = false;

string scenesNom[] = { "Menu Principal", "Credits", "En Jeu", "Game Over" };

level* niveauActuel;
scene  sceneActuel;
#pragma endregion

#pragma region Score

void getBestScore()
{
    FILE* f = fopen("./best_score_value.txt", "r");
    if (f == null)
    {
        return;
    }
    fscanf(f, "%i", &bestScore);
    fclose(f);
}

void saveBestScore(level* l)
{
    int score = l->score;
    getBestScore();
    if (score < bestScore)
    {
        return;
    }

    bestScore = score;

    FILE* f = fopen("./best_score_value.txt", "w");
    if (f != null)
    {
        fprintf(f, "%i", score);
        fclose(f);
    }
    sauvegarderLevel("./best_score.txt", l);
}
#pragma endregion

#pragma region Mouvement

char mouvementToChar(mouvement m)
{
    switch (m)
    {
        case MOUV_Haut:     return 'h';
        case MOUV_Bas:      return 'b';
        case MOUV_Gauche:   return 'g';
        case MOUV_Droite:   return 'd';
        default:            return '?';
    }
}

mouvement charToMouvement(char c)
{
    switch (c)
    {
        case 'h': return MOUV_Haut;
        case 'b': return MOUV_Bas;
        case 'g': return MOUV_Gauche;
        case 'd': return MOUV_Droite;
        default:  return MOUV_PasEncoreChoisi; // En cas d'erreur
    }
}


mouvement lireMouvementZQSD()
{
    printf("(p pour sauvegarder)\n<ZQSD> ou <GDHB> : ");
    char c = '?';
    scanf("\n%c", &c);
    switch (c)
    {
        case 'Z': case 'z': case 'H': case 'h': return MOUV_Haut;
        case 'S': case 's': case 'B': case 'b': return MOUV_Bas;
        case 'Q': case 'q': case 'G': case 'g': return MOUV_Gauche;
        case 'D': case 'd': return MOUV_Droite;
        case 'p': case 'P': quitteLeJeu = true;
    }
    return MOUV_PasEncoreChoisi;
}

mouvement lireMouvementConsoleZQSD(level* l)
{
    mouvement m = lireMouvementZQSD();
    return m;
}


#ifdef  UtiliseSDL
bool estPresser = false;
mouvement lireMouvementSDL(level* l)
{
    static int nb = 0; //return (nb++) % 4; // c'est avec ce que j'ai fait mon hight score
    SDL_PollEvent(&event);
    switch (event.type)
    {
        case SDL_QUIT: quitteLeJeu = true;  return MOUV_PasEncoreChoisi;
        case SDL_KEYDOWN:
        {
            if (estPresser == false)
            {
                estPresser = true;
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:   case SDLK_z:  case SDLK_z_azerty:   return MOUV_Haut;
                    case SDLK_DOWN: case SDLK_s:                        return MOUV_Bas;
                    case SDLK_LEFT: case SDLK_q:  case SDLK_q_azerty:   return MOUV_Gauche;
                    case SDLK_RIGHT:case SDLK_d:                        return MOUV_Droite;
                    case SDLK_ESCAPE: quitteLeJeu = true;               return MOUV_PasEncoreChoisi;
                }
            }
            switch (event.key.keysym.sym)
            {
                case SDLK_p:return (nb++) % 4;
            }
        }break;
        case SDL_KEYUP: estPresser = false; break;
    }
    return MOUV_PasEncoreChoisi;
}
#endif

mouvement lireMouvementEntree(level* l)
{
#ifdef  UtiliseSDL
    return lireMouvementSDL(l);
#else
    return lireMouvementConsoleZQSD(l);
#endif
}

mouvement lireMouvementReplay(level* l)
{
    if (feof(l->save) == false)
    {
        char c = '?';
        if (fscanf(l->save, "%c", &c) == 1)
        {
            mouvement m = charToMouvement(c);
            if (m != MOUV_PasEncoreChoisi)
            {
                return m;
            }
        }
    }
    // Fin du Replay. Le joueur peut continuer a jouer si le niveau n'est pas finis
    basculerSurLesEntreeDuJoueur(l);
    return MOUV_PasEncoreChoisi;
}

mouvement lireMouvementReplayAvecInput(level* l)
{
    //return rand() % 4;
    entrePourQuitterLeJeuUniquement();
    return lireMouvementReplay(l);
}

mouvement attendreEtLireMouvementReplay(level* l)
{
#ifdef  UtiliseSDL
    estPresser = false;
    mouvement m = lireMouvementSDL(l);
    int delai = 700;
    switch (m)
    {
        case MOUV_Haut:   delai = 0; break;
        case MOUV_Bas:    delai = 50; break;
        case MOUV_Droite: delai = 350; break;
        case MOUV_Gauche: delai = 100000000; break;
    }

    if (SDL_GetTicks() - l->SdlTickDebutTour >= delai)
    {
        return lireMouvementReplay(l);
    }
    else { entrePourQuitterLeJeuUniquement(); }
    return MOUV_PasEncoreChoisi;
#else
    return lireMouvementReplay(l);
#endif
}
#pragma endregion

#pragma region Chargement / Dechargement Niveau 

level* nouveauLevel(int tailleX, int tailleY)
{
    level* l = smalloc(sizeof(level));
    tailleX = maxi(2, tailleX);
    tailleY = maxi(2, tailleY);
    l->tailleX = tailleX;
    l->tailleY = tailleY;
    

    l->seed = time(null);

    basculerSurLesEntreeDuJoueur(l);
    levelSauvegardeConfigurationInitiale(l);

    initLevel(l);
    return l;
}

level* levelDepuisFichier(FILE* f, bool* reussi)
{
    *reussi = true;
    level* l = smalloc(sizeof(level));


#ifdef  UtiliseSDL
    fseek(f, 0, SEEK_SET);
    printf("fichier :\n");
    while (feof(f) == false)
    {
        char c = fgetc(f);
        printf("%c", c);
    }
    printf("\n");
#endif 

    fseek(f, 0, SEEK_SET);

    if (fscanf(f, "seed %i\ntailleX %i\ntailleY %i\nreplay ", &l->seed, &l->tailleX, &l->tailleY) != 3)
    {
        free(l);
        fclose(f);
        *reussi = false;
        return null;
    }

    l->tailleX = maxi(2, l->tailleX);
    l->tailleY = maxi(2, l->tailleY);

    visionner(l);
    l->seed = l->seed;
    l->save = f;

    initLevel(l);
    return l;
}

level* levelDepuisChemin(string s, bool* reussi)
{
    *reussi = true;
    FILE* f = fopen(s, "r+");
    if (f == null)
    {
        *reussi = false;
        return null;
    }
    return levelDepuisFichier(f, reussi);
}

level* defaultLevel() { return nouveauLevel(4, 4); }

level* chargerSauvegarde(string path)
{
    bool reussi;
    level* l = levelDepuisChemin(path_last_level, &reussi);

    if (reussi)
    {
        chargeUneSauvegarde = true;
        l->LireMouvement = &lireMouvementReplayAvecInput;
        while (chargeUneSauvegarde)
        {
            tourDeJeu(l);
        }

        repeat(x, l->tailleX)
        {
            repeat(y, l->tailleY)
            {
                l->blocs[x][y].posXAffichage = x;
                l->blocs[x][y].posYAffichage = y;
            }
        }
        return l;
    }

    remove(path); // Le fichier du niveau est corrompu
    return defaultLevel();
}

void sauvegarderLevel(string path, level* l)
{
    FILE* f = fopen(path, "w");
    if (f != null)
    {
        // Sauvegarde du fichier save du niveau en tant que meileur score
        long pos = ftell(l->save);

        fseek(l->save, 0, SEEK_SET);
        fseek(f, 0, SEEK_SET);

        while (feof(l->save) == false)
        {
            char c = '?';
            if (fscanf(l->save, "%c", &c) != 1) { break; }
            fprintf(f, "%c", c);
        }
        fseek(l->save, pos, SEEK_SET);
        fclose(f);
    }
}


void levelSauvegardeConfigurationInitiale(level* l)
{
#define ERROR_LastLevelFile 2
    // Creation du fichier
    l->save = fopen(path_last_level, "w");

    if (l->save != NULL)
    {
        fclose(l->save);
    }

    l->save = fopen(path_last_level, "r+");

    if (l->save == null)
    {
        quitter(ERROR_LastLevelFile);
    }
    fprintf(l->save, "seed %i\ntailleX %i\ntailleY %i\n", l->seed, l->tailleX, l->tailleY);
    fprintf(l->save, "replay ");
}


void initLevel(level* l)
{
    l->tour = 0;
    l->score = 0;

    l->blocs = smalloc(l->tailleX * sizeof(bloc*));

#ifdef  UtiliseSDL
    l->SdlTickChargementNiveau = SDL_GetTicks();
#endif

    repeat(x, l->tailleX)
    {
        l->blocs[x] = smalloc(l->tailleY * sizeof(bloc));

        repeat(y, l->tailleY)
        {
            l->blocs[x][y].puissance = TuileVideId;
            l->blocs[x][y].posXAffichage = x;
            l->blocs[x][y].posYAffichage = y;
            l->blocs[x][y].dernierTourCombiner = -1;
            l->blocs[x][y].colorR = l->blocs[x][y].colorG = l->blocs[x][y].colorB = 0;
        }
    }
    srand(l->seed);

    twice(placerBlocAleatoire(l, 1););
}


void dechargerLevelSaufFichier(level* l)
{
    repeat(x, l->tailleX)
    {
        free(l->blocs[x]);
    }
    free(l->blocs);
    free(l);
}

void dechargerLevel(level* l)
{
    fclose(l->save);
    dechargerLevelSaufFichier(l);
}
#pragma endregion

#pragma region Logique du jeu

// Retourne : EstGameOver
bool tourDeJeu(level* l)
{
    l->tour++;

#ifdef  UtiliseSDL
    l->SdlTickDebutTour = SDL_GetTicks();
#endif 

    printLevelConsole(l);

    do
    {
        mouvement m = MOUV_PasEncoreChoisi;
        do
        {
#ifdef  UtiliseSDL
            l->SdlTickAvantGameOver = SDL_GetTicks();
            if (chargeUneSauvegarde == false)
            {
                printLevelSDL(l);
                SDL_Afficher();
                SDL_Delay(1000 / 120);
            }
#endif 

            m = l->LireMouvement(l);

            if (m == MOUV_PasEncoreChoisi && chargeUneSauvegarde)
            {
                chargeUneSauvegarde = false;
                return false;
            }

            if (quitteLeJeu)
            {
                dechargerLevel(l);
                quitteSiBesoin();
            }
        } while (m == MOUV_PasEncoreChoisi || m < 0 || m >= 4);

        int nbAction = appliquerMouvement(l, m);
        if (nbAction != 0)
        {
            if (estEnTrainDeVisionner(l) == false && chargeUneSauvegarde == false)
            {
                fprintf(l->save, "%c", mouvementToChar(m));
            }
            break;
        }
        else
        {
            // Si il n'y a aucune action, alors il y a aucun effet secondaire dans le jeu
            chargeUneSauvegarde = false;
            if (estEnTrainDeVisionner(l))
            {
                basculerSurLesEntreeDuJoueur(l);
            }

#ifdef  UtiliseSDL
            camX += randNoSideEffect() % 2 * 16 - 8;
            camY += randNoSideEffect() % 2 * 16 - 8;
#endif 
            printf("Impossible de deplacer les blocs dans la direction souhaiter\n");
        }
    }
    forever;

    if (l->score > bestScore && chargeUneSauvegarde == false)
    {
        saveBestScore(l);
    }

    // Place un bloc aleatoire d'une valeur de 2 ou 4
    if (placerBlocAleatoire(l, rand() % 2 + 1) == false || isGameOver(l))
    {
        return true;
    }
    return false;
}

void boucleDeJeu(level* l)
{
#ifdef  UtiliseSDL
    l->SdlTickChargementNiveau = SDL_GetTicks();
    l->SdlTickAvantGameOver = l->SdlTickChargementNiveau;
#endif 
    printf("Bienvenue au jeu du 2^11 :\n");

    if (isGameOver(l)) { return; }
    forever
    {
        bool finDuJeu = tourDeJeu(l);
        if (finDuJeu) { break; }
    }
}


bool dansLeNiveau(level* l, int x, int y) { return x >= 0 && x < l->tailleX&& y >= 0 && y < l->tailleY; }

bool isGameOver(level* l)
{
    if (compterTuileVide(l) != 0) { return false; }

    repeat(y, l->tailleY)
    {
        repeat(x, l->tailleX - 1)
        {
            if (l->blocs[x][y].puissance == l->blocs[x + 1][y].puissance) { return false; }
        }
    }

    repeat(y, l->tailleY - 1)
    {
        repeat(x, l->tailleX)
        {
            if (l->blocs[x][y].puissance == l->blocs[x][y + 1].puissance) { return false; }
        }
    }
    return true;
}

int compterTuileVide(level* l)
{
    int count = 0;
    repeat(x, l->tailleX)
    {
        repeat(y, l->tailleY)
        {
            count += l->blocs[x][y].puissance == TuileVideId ? 1 : 0;
        }
    }
    return count;
}

bool placerBlocAleatoire(level* l, int puissance)
{
    if (compterTuileVide(l) == 0) { return false; }

    forever
    {
        int x = rand() % l->tailleX;
        int y = rand() % l->tailleY;
        if (l->blocs[x][y].puissance == TuileVideId)
        {
            l->blocs[x][y].puissance = puissance;
            l->blocs[x][y].posXAffichage = x;
            l->blocs[x][y].posYAffichage = y;
            l->blocs[x][y].dernierTourCombiner = -2; // -2 pour l'animation d'apparition
            l->blocs[x][y].colorR = l->blocs[x][y].colorG = l->blocs[x][y].colorB = 0;
            return true;
        }
    }
}

int deplacerBloc(level* l, int x, int y, int xFinal, int yFinal, int puissance)
{
    if (x == xFinal && y == yFinal)
    {
        // Pas la peine de bouger le bloc
        return 0;
    }
    l->blocs[x][y].puissance = TuileVideId;
    l->blocs[xFinal][yFinal].puissance = puissance;
    l->blocs[xFinal][yFinal].posXAffichage = l->blocs[x][y].posXAffichage;
    l->blocs[xFinal][yFinal].posYAffichage = l->blocs[x][y].posYAffichage;
    l->blocs[xFinal][yFinal].colorR = l->blocs[x][y].colorR;
    l->blocs[xFinal][yFinal].colorG = l->blocs[x][y].colorG;
    l->blocs[xFinal][yFinal].colorB = l->blocs[x][y].colorB;
    return 1;
}

int appliquerLeMouvementSurLeBloc(level* l, int x, int y, int mX, int mY)
{
    if (l->blocs[x][y].puissance == TuileVideId) { return 0; }

    if (l->blocs[x][y].dernierTourCombiner == -2) { l->blocs[x][y].dernierTourCombiner = -1; }

    int xFinal = x;
    int yFinal = y;

    int nbAction = 0;

    forever
    {
        if (dansLeNiveau(l, xFinal + mX, yFinal + mY) == false)
        {
            // bouge le bloc contre le bord de l'ecran
            nbAction += deplacerBloc(l, x, y, xFinal, yFinal, l->blocs[x][y].puissance);
            break;
        }

        int blocSuivantPuissance = l->blocs[xFinal + mX][yFinal + mY].puissance;
        if (blocSuivantPuissance != TuileVideId)
        {
            // Un bloc peut etre combiner au plus une seul fois pendant un tour
            if (blocSuivantPuissance == l->blocs[x][y].puissance && l->blocs[xFinal + mX][yFinal + mY].dernierTourCombiner != l->tour)
            {
                // fusionne les 2 blocs
                l->score += 1 << (l->blocs[x][y].puissance + 1);
                nbAction += deplacerBloc(l, x, y, xFinal + mX, yFinal + mY, l->blocs[x][y].puissance + 1);
                l->blocs[xFinal + mX][yFinal + mY].dernierTourCombiner = l->tour;
                break;
            }
            else
            {
                // bouge le bloc contre un autre bloc d'une valeur differente
                nbAction += deplacerBloc(l, x, y, xFinal, yFinal, l->blocs[x][y].puissance);
                break;
            }
        }
        xFinal += mX;
        yFinal += mY;
    }
    return nbAction;
}

// Retourne le nombre d'action (bloc qui ont ete bouger/combiner)
int appliquerMouvement(level* l, mouvement m)
{
    int nbAction = 0;

    // Vecteur m
    int mX = m == MOUV_Droite ? 1 : (m == MOUV_Gauche ? -1 : 0);
    int mY = m == MOUV_Bas    ? 1 : (m == MOUV_Haut   ? -1 : 0);

    // Les Index dependent de la direction du mouvement
    // afin de gerer correctement la priorite
    int xDebut = mX < 0 ? 0          : l->tailleX - 1;
    int xFin   = mX < 0 ? l->tailleX :  -1;

    int yDebut = mY < 0 ? 0          : l->tailleY - 1;
    int yFin   = mY < 0 ? l->tailleY :    -1;

    // Les boucles gerent aussi la priorite du mouvement
    for (int x = xDebut; x != xFin; x = faireTendre(x, xFin))
    {
        for (int y = yDebut; y != yFin; y = faireTendre(y, yFin))
        {
            nbAction += appliquerLeMouvementSurLeBloc(l, x, y, mX, mY);
        }
    }

    return nbAction;
}


void entrePourQuitterLeJeuUniquement()
{
#ifdef  UtiliseSDL
    SDL_PollEvent(&event);
    switch (event.type)
    {
        case SDL_QUIT: quitteLeJeu = true;  break;
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE: quitteLeJeu = true;  break;
            }
        }break;
    }
#endif
}

void basculerSurLesEntreeDuJoueur(level* l)
{
    l->LireMouvement = &lireMouvementEntree;
}

void visionner(level* l)
{
    l->LireMouvement = &attendreEtLireMouvementReplay;
}

bool estEnTrainDeVisionner(level* l)
{
    return l->LireMouvement == &attendreEtLireMouvementReplay;
}

#pragma endregion

#pragma region Affichage

void printLevelConsole(level* l)
{
    printf("\nscore: %i, tour: %i, (seed: %i)\n", l->score, l->tour, l->seed);
    repeat(y, l->tailleY)
    {
        repeat(x, l->tailleX)
        {
            if (l->blocs[x][y].puissance == TuileVideId)
            {
                printf("   . ");

            }
            else
            {
                //printf("%4i ", l->bloc[x][y].puissance);
                printf("%4i ", 1 << l->blocs[x][y].puissance);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void printLevelInfo(level* l)
{
#ifdef  UtiliseSDL
    char info[64];
    sprintf(info, "Tour: %i", l->tour);
    font_write(info, 17, 9);

    sprintf(info, "Score: %i", l->score);
    font_write_center(info, 1, screenX - 17 - font_spacing_x, 9);

    sprintf(info, "` :% i", bestScore);
    font_write_center(info, 1, screenX - 17 - font_spacing_x, screenY - 17 - 26);

    int totalMs = l->SdlTickAvantGameOver - l->SdlTickChargementNiveau;
    int min = totalMs / (60 * 1000);
    totalMs -= min * 60 * 1000;
    int s = totalMs / 1000;
    totalMs -= s * 1000;

    if (totalMs != 0)
    {
        while (totalMs / 100 != 0) { totalMs /= 10; }
    }

    if (estEnTrainDeVisionner(l) == false)
    {
        sprintf(info, " _ %i'%i\"%.1i", min, s, totalMs);
        font_write_center(info, 0, 17, screenY - 17 - 26);
    }
    else
    {
        font_write_center("| replay ( ~  avancer)", 0, 17, screenY - 17 - 26);
    }
#endif
}

void printLevelGrille(level* l)
{
#ifdef  UtiliseSDL
    Frame++;

    if (randNoSideEffect() % 4 == 0) { camX *= -1; }
    if (randNoSideEffect() % 4 == 0) { camY *= -1; }

    if (Frame % 3 == 0)
    {
        camX = faireTendre(camX, 0);
        camY = faireTendre(camY, 0);
    }

    if (camX == 0 && camY == 0)
    {
        color_grid_red = maxi(40, color_grid_red - 2);

    }
    else
    {
        color_grid_red = mini(255, color_grid_red + 4);
    }

    FillRect(null, SDL_MapRGB(screen->format, 0, 0, 0));

    int pixelParCase = 100;
    if (pixelParCase * l->tailleX > 90 * screenX / 100)
    {
        pixelParCase = 80 * screenX / 100 / l->tailleX;
    }
    if (pixelParCase * l->tailleY > 90 * screenY / 100)
    {
        pixelParCase = 80 * screenY / 100 / l->tailleY;
    }

    int pixelParBloc = pixelParCase * 90 / 100;
    int differenceCaseBloc = pixelParCase - pixelParBloc;

    SDL_Rect r = newRect(0, 0, pixelParCase * l->tailleX, pixelParCase * l->tailleY);
    int offsetX = (screenX - r.w) / 2;
    int offsetY = (screenX - r.w) / 2;
    r.x = offsetX - differenceCaseBloc / 2;
    r.y = offsetY - differenceCaseBloc / 2;
    r.w += differenceCaseBloc;
    r.h += differenceCaseBloc;

    color color_grid = SDL_MapRGB(screen->format, color_grid_red, 40, 40);
    FillRect(&r, color_grid);

    int tick = SDL_GetTicks();

    // Background :
    repeat(x, l->tailleX)
    {
        repeat(y, l->tailleY)
        {
            r = newRect(x * pixelParCase + offsetX + differenceCaseBloc / 2, y * pixelParCase + offsetY + differenceCaseBloc / 2, pixelParBloc, pixelParBloc);
            FillRect(&r, SDL_MapRGB(screen->format, 0, 0, 0));
        }
    }

    // Bloc :
    repeat(x, l->tailleX)
    {
        repeat(y, l->tailleY)
        {
            bloc* b = &l->blocs[x][y];

            if (b->puissance == TuileVideId)
            {
                continue;
            }

            float posX = b->posXAffichage;
            float posY = b->posYAffichage;

            float coef = 15;
            // Moyenne ponderee de la position :
            b->posXAffichage = (posX * coef + x) / (coef + 1);
            b->posYAffichage = (posY * coef + y) / (coef + 1);

            // Petit truc pour mettre la position d'affichage du bloc à la meme position dans le tableau si les 2 valeurs sont similaires
            if (fabsf(b->posXAffichage - x) < 0.01) { b->posXAffichage = x; }
            if (fabsf(b->posYAffichage - y) < 0.01) { b->posYAffichage = y; }

            int pixels = 90;
            if (b->dernierTourCombiner == l->tour - 1)
            {
                pixels = pixels + 20 * sinf((tick - l->SdlTickDebutTour) / 1000.0 * 2 * pi);
            }
            else if (b->dernierTourCombiner == -2)
            {
                pixels = pixels - 20;
            }

            pixelParBloc = pixelParCase * pixels / 100;
            differenceCaseBloc = pixelParCase - pixelParBloc;

            r = newRect(posX * pixelParCase + offsetX + differenceCaseBloc / 2, posY * pixelParCase + offsetY + differenceCaseBloc / 2, pixelParBloc, pixelParBloc);

            int redTarget = 0;
            int greenTarget = 0;
            int blueTarget = 0;

            HSVtoRGB(fmodf((b->puissance - 1) * 31 + 270, 360), 100, 80 + (b->dernierTourCombiner == -2 ? -20 : 0), &redTarget, &greenTarget, &blueTarget);

            coef = 31;
            b->colorR = (b->colorR * coef + redTarget)   / (coef + 1);
            b->colorG = (b->colorG * coef + greenTarget) / (coef + 1);
            b->colorB = (b->colorB * coef + blueTarget)  / (coef + 1);

            color c = SDL_MapRGB(screen->format, b->colorR, b->colorG, b->colorB);

            /*
            if (b->dernierTourCombiner == l->tour - 1)
            {
                FillCircularSquare(&r, c, fabsf(0.5f*sinf((tick - l->SdlTickDebutTour) / 10000.0 * 2 * pi)));
            }
            else
            {
                FillRect(&r, c);
            }*/
            //FillCircle(&r, c);

            FillRect(&r, c);

            int valeur = 1 << b->puissance;
            int pixelTextValeur = pixelParBloc * (5 * 5) / 90;
            int offset = (pixelParBloc - pixelTextValeur) / 2;

            c = SDL_MapRGB(screen->format, b->colorR / 2, b->colorG / 2, b->colorB / 2);
            writeNumber(valeur, r.x + pixelParBloc / 2, r.y + offset, pixelTextValeur, c);
        }
    }
#endif
}

void printLevelSDL(level* l)
{
#ifdef  UtiliseSDL
    printLevelGrille(l);
    printLevelInfo(l);
#endif 
}
#pragma endregion

#pragma region Scene

menuAction LireEntreMenu(level* l)
{
#ifdef  UtiliseSDL
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT: quitteLeJeu = true; quitteSiBesoin();
        case SDL_KEYDOWN:
        {
            if (estPresser == false)
            {
                estPresser = true;

                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: quitteLeJeu = true; quitteSiBesoin();
                        return MOUV_PasEncoreChoisi;
                    case SDLK_UP:   case SDLK_z: case SDLK_z_azerty:
                    case SDLK_DOWN: case SDLK_s:
                    case SDLK_LEFT: case SDLK_q: case SDLK_q_azerty:
                    case SDLK_RIGHT:case SDLK_d:
                        return MENU_ReprendrePartie;
                    case SDLK_n:
                        return MENU_NouvellePartie;
                    case SDLK_r:
                        return MENU_RegarderDernierePartie;
                    case SDLK_c:
                        return MENU_Credits;
                    case SDLK_SPACE:
                    case SDLK_KP_ENTER:
                    case SDLK_RETURN:
                        return MENU_Ok;
                }
            }
        }break;
        case SDL_KEYUP: estPresser = false; break;
    }
#else
    printf("Pressez une touche: ");
    //printf("Entree Console Menu\n");
    char c = '?';
    scanf("\n%c", &c);
    switch (c)
    {
        case 'Z': case 'z': case 'H': case 'h':
        case 'S': case 's': case 'B': case 'b':
        case 'Q': case 'q': case 'G': case 'g':
        case 'D': case 'd':
            return MENU_ReprendrePartie;
        case 'N': case 'n':
            return MENU_NouvellePartie;
        case 'R': case 'r':
            return MENU_RegarderDernierePartie;
        case 'C': case 'c':
            return MENU_Credits;
        case ' ': case '\0': case 'o': case '0':
            return MENU_Ok;
    }
#endif
    return MENU_ActionPasEncoreChoisi;
}


void sceneInit()
{
    printf(__TIME__"\n");

    getBestScore();

#ifdef UtiliseSDL
    screen_load();
    font_load();

    FillRect(null, SDL_MapRGB(screen->format, 0, 0, 0));
    font_write_center("Chargement de l'ancienne partie...", 0.5, screenX / 2, screenY / 2);

    SDL_Afficher();
#endif

    sceneActuel = SCENE_MenuPrincipal;
    niveauActuel = chargerSauvegarde(path_last_level);
}

void changeScene(scene s)
{
    printf("================= %s =========\n", scenesNom[s]);
    sceneActuel = s;
}

void sceneMenuPrincipal()
{
    menuAction action = MENU_ActionPasEncoreChoisi;

    while (action == MENU_ActionPasEncoreChoisi)
    {
        printLevelGrille(niveauActuel);

#ifdef  UtiliseSDL
        int line = 1;
        font_write_center("=== 2048 ===", 0.5, screenX / 2, 17 + 9);
        font_write_center("~  ou <ESFD> : Jouer", 0.5, screenX / 2, 430);
        font_write_center("<N> : Nouvelle Partie", 0.5, screenX / 2, 460);

        font_write_center("<R>: Voir Record", 0, 9, 510);
        font_write_center("<C>: Credits", 1, screenX, 510);
        //font_write_center("<C> : Credits", 0.5, screenX / 2, 370 + 30 * line++);

        char meilleur_score[64];
        sprintf(meilleur_score, "` :% i", bestScore);
        font_write_center(meilleur_score, 0, 17, 9);

        SDL_Afficher();
        SDL_Delay(1000 / 120);
#else
        printf("\n\n        Le Jeu du\n\n");
        printf("  --+   +-+   + +   +-+\n");
        printf("    |   | |   | |   | |\n");
        printf("  +-+   | |   +-+   +-+\n");
        printf("  |     | |     |   | |\n");
        printf("  +--   +-+     |   +-+\n\n");
        printf("<ZQSD> ou <HBGD> : Jouer\n");
        printf("<N> : Nouvelle Partie\n");
        printf("<C> : Credits\n\n");
        printf("<R> : Voir le Record\n\n");
        printf("Meilleur Score : %i\n", bestScore);
#endif 
        action = LireEntreMenu(niveauActuel);
    }

    switch (action)
    {
        case MENU_ReprendrePartie: changeScene(SCENE_EnJeu);   break;
        case MENU_Credits:         changeScene(SCENE_Credits); break;
        case MENU_NouvellePartie:
        {
            changeScene(SCENE_EnJeu);
            dechargerLevel(niveauActuel);
            niveauActuel = defaultLevel();
        }break;
        case MENU_RegarderRecord:
        {
            changeScene(SCENE_EnJeu);
            dechargerLevel(niveauActuel);
            int reussi;
            niveauActuel = levelDepuisChemin(path_record_level, &reussi);
            if (reussi)
            {
                visionner(niveauActuel);
            }
        }break;
    }
}

void sceneGameOver()
{
    menuAction action = MENU_ActionPasEncoreChoisi;
    while (action == MENU_ActionPasEncoreChoisi)
    {
        printLevelSDL(niveauActuel);

#ifdef  UtiliseSDL
        font_write_center("Game Over !", 0.5, screenX / 2, 32);
        font_write_center("<Espace> ou <Entree> : Continuer", 0.5, screenX / 2, 430);
        font_write_center("<R> : Revoir le Replay", 0.5, screenX / 2, 460);
        SDL_Afficher();
        SDL_Delay(1000 / 120);
#else
        printf("<O> : Continuer\n");
        printf("<R> : Revoir le Replay\n");
#endif
        action = LireEntreMenu(niveauActuel);
    }

    switch (action)
    {
        case MENU_Ok:
        {
            dechargerLevel(niveauActuel);
            niveauActuel = NULL;
            changeScene(SCENE_MenuPrincipal);
        }
        break;
        case MENU_RegarderDernierePartie:
        {
            bool reussi;
            level* l = levelDepuisFichier(niveauActuel->save, &reussi);
            dechargerLevelSaufFichier(niveauActuel);
            niveauActuel = l;
            changeScene(SCENE_EnJeu);
        }
        break;
    }
}

void sceneCredits()
{
    menuAction action = MENU_ActionPasEncoreChoisi;
    while (action == MENU_ActionPasEncoreChoisi)
    {
#ifdef  UtiliseSDL
        FillRect(null, SDL_MapRGB(screen->format, 0, 0, 0));

        int ligne = 0;
        font_write_center("============= Credit =============", 0.5, screenX / 2, 30 * ligne++);
        font_write_center("Un jeu de :", 0.5, screenX / 2, 30 * ligne++);

        ligne++;
        font_write_center("Thomas    &    Reynalde", 0.5, screenX / 2, 30 * ligne++);
        font_write_center("  TAMAGNAUD       SEGERIE    ", 0.5, screenX / 2, 30 * ligne++);

        ligne += 2;
        font_write_center("Base sur le jeu du 2048.", 0, 17, 30 * ligne++);
        ligne++;
        font_write_center("Programmation :", 0, 17, 30 * ligne++);
        font_write_center("   Langage C avec la SDL 1.2", 0, 17, 30 * ligne++);
        ligne++;
        font_write_center("Police d'Ecriture:", 0, 17, 30 * ligne++);
        font_write_center("   Amstrad CPC Extented", 0, 17, 30 * ligne++);
        ligne++;
        font_write_center("26 Novembre 2021", 0.5, screenX / 2, 30 * ligne++);

        font_write_center("<Espace> ou <Entree> : Continuer", 0.5, screenX / 2, screenY - 36);
        SDL_Afficher();
        SDL_Delay(1000 / 120);
#else
        printf("\n   Un jeu de :\n");
        printf("Thomas TAMAGNAUD & Reynalde SEGERIE\n\n\n");
        printf("Base sur le jeu du 2048.\n\n");
        printf("Programmation :\n");
        printf("   Langage C (version Console sans la SDL 1.2)\n\n");
        printf("Police d'Ecriture :\n");
        printf("   Amstrad CPC Extented\n\n");
        printf("26 Novembre 2021\n\n");
        printf("<O> : Continuer\n");
#endif 
        action = LireEntreMenu(niveauActuel);
    }

    switch (action)
    {
        case MENU_Ok:
        {
            changeScene(SCENE_MenuPrincipal);
        }
        break;
    }
}

void sceneEnJeu()
{
    boucleDeJeu(niveauActuel);
    changeScene(SCENE_GameOver);
}
#pragma endregion

int main(int argc, char* argv[])
{
    sceneInit();

    forever
    {
        if (niveauActuel == null)
        {
            niveauActuel = defaultLevel();
        }

        switch (sceneActuel)
        {
            case SCENE_MenuPrincipal:  sceneMenuPrincipal();    break;
            case SCENE_EnJeu:          sceneEnJeu();            break;
            case SCENE_GameOver:       sceneGameOver();         break;
            case SCENE_Credits:        sceneCredits();          break;
        }

    }
    return EXIT_SUCCESS;
}

//gcc -c -lm -ldl programme/*.c $(sdl-config --flags --libs)
//gcc -o jeu *.o $(sdl-config --cflags --libs) -lm -ldl
//./jeu