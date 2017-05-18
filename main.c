#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


#include <time.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 780

//Option de build dans les linker setting mettre les libSDL2main.a libSDL2.dll.a
//Dans search directory
//Option de build compiler mettre les include
//Option de build linker mettre les lib

//mettre dans build linker -lmingw32

typedef struct game{
     SDL_Window *g_pWindow;
     SDL_Renderer *g_pRenderer;
     SDL_Texture *g_texture;
     SDL_Surface *g_surface;
}game;

typedef struct gameState{
    int g_bRunning;
}gameState;



typedef struct coord{
    int x;
    int y;
}coord;

typedef struct balle_jeu{
    coord milieu;
    int direction;
    int rayon;
}balle_jeu;


int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame, SDL_Rect *j1, SDL_Rect *j2, int *score1, int *score2, balle_jeu *balle);
void handleEvents(gameState *state, SDL_Rect *j1, SDL_Rect *j2, game *myGame);
void delay(unsigned int frameLimit);
void destroy(game *myGame);

void dplcmt_raquette();
void dplcmt_balle(balle_jeu *balle, SDL_Rect j1, SDL_Rect j2, int *score1, int *score2);

void affiche_tout(game *myGame, int score1, int score2, SDL_Rect j1, SDL_Rect j2, balle_jeu balle);
void affiche_balle(game *myGame,int x0, int y0, int radius);
void affiche_score(game *myGame, int score1, int score2);
void affiche_raquette();

int main(int argc, char *argv[])
{
     balle_jeu balle;
     game myGame;
     gameState state;

     SDL_Rect j1;
     SDL_Rect j2;
     int score1;
     int score2;

    srand(time(NULL));

    //Pour les 60 fps
    unsigned int frameLimit = SDL_GetTicks() + 16;

    if(init("SDL_Pong",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN,&myGame, &j1, &j2, &score1, &score2, &balle)){
        state.g_bRunning=1;
    }else{
        return 1;//something's wrong
    }


    //Create texture for drawing in texture or load picture
    myGame.g_texture=SDL_CreateTexture(myGame.g_pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,SCREEN_WIDTH,SCREEN_HEIGHT);

    while(state.g_bRunning){

            handleEvents(&state, &j1, &j2, &myGame);
            dplcmt_balle(&balle,j1,j2,&score1,&score2);

            printf("j1 :%i\n j2 : %i\n\n",score1,score2);

            SDL_SetRenderDrawColor(myGame.g_pRenderer,0,0,0,255);
            SDL_RenderClear(myGame.g_pRenderer);

            affiche_tout(&myGame,score1,score2,j1,j2,balle);


        // Gestion des 60 fps (1000ms/60 = 16.6 -> 16
            delay(frameLimit);
            frameLimit = SDL_GetTicks() + 16;

    }
    destroy(&myGame);

//    TTF_Quit();
    SDL_Quit();

  return 0;
}

/*
BUT : Tout initialiser
ENTREE : La fenêtre et ses caractéristiques, les raquettes, les scores, la balle
SORTIE : Tous les éléments initialisés

*/

int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame, SDL_Rect *j1, SDL_Rect *j2, int *score1, int *score2, balle_jeu *balle){

    //initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING)>=0)
    {
            //if succeeded create our window
            myGame->g_pWindow=SDL_CreateWindow(title,xpos,ypos,height,width,flags);
            //if succeeded create window, create our render
            if(myGame->g_pWindow!=NULL){
                myGame->g_pRenderer=SDL_CreateRenderer(myGame->g_pWindow,-1,SDL_RENDERER_ACCELERATED);
            }

    }else{
        return 0;
    }

    //initialize ttf


//    mFont->g_font=TTF_OpenFont("./assets/fonts/sinaNovaReg/SinaNovaReg.ttf",65);

    //init position raquettes
    j1->x=20;
    j1->y=20;
    j1->h=100;
    j1->w=20;

    j2->x=SCREEN_WIDTH-40;
    j2->y=20;
    j2->h=100;
    j2->w=20;

    //init scores
    score1=0;
    score2=0;

    //init balle
    balle->milieu.x=SCREEN_WIDTH/2;
    balle->milieu.y=SCREEN_HEIGHT/2;
    balle->direction=rand()%4+1;
    balle->rayon=10;

    return 1;
}

/*
BUT : Lire les touches au clavier et mettre à jour les coordonnées des raquettes suite à l'appui.
ENTREE : La fenêtre de jeu, l'état du jeu, les coordonnees des raquettes
SORTIE : Les coordonnées de la raquette mises à jour suite à l'appui d'une touche

*/

void handleEvents(gameState *state, SDL_Rect *j1, SDL_Rect *j2, game *myGame){

    SDL_Event event;

    if(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
              state->g_bRunning=0;break;
        case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
                            {
                                case SDLK_z:
                                    if (j1->y>=20){
                                        j1->y=j1->y-20;
                                    }
                                    break;
                                case SDLK_s:
                                    if (j1->y<SCREEN_HEIGHT-100){
                                        j1->y=j1->y+20;
                                    }
                                     break;
                                case SDLK_UP:
                                     if (j2->y>=20){
                                        j2->y=j2->y-20;
                                    }
                                     break;
                                case SDLK_DOWN:
                                    if (j2->y<SCREEN_HEIGHT-100){
                                        j2->y=j2->y+20;
                                    }
                                     break;
                            }
                            break;
        case SDL_KEYUP:;break;

        default:break;

        }
    }


}

/*
BUT : Afficher tous les éléments du jeu : raquettes, score et balle
ENTREE : Les scores, les attributs des rectangles des joueurs, les informations de la balle
SORTIE : L'affichage de tous les éléments

*/

void affiche_tout(game *myGame,int score1, int score2, SDL_Rect j1, SDL_Rect j2, balle_jeu balle) {

        affiche_score(myGame, score1, score2);

        //Affichage raquettes
        SDL_SetRenderDrawColor(myGame->g_pRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(myGame->g_pRenderer,&j1);
        SDL_RenderFillRect(myGame->g_pRenderer,&j2);

        affiche_balle(myGame,balle.milieu.x,balle.milieu.y,balle.rayon);

        //Affichage RENDERER
        SDL_RenderPresent(myGame->g_pRenderer);
}

/*
BUT : Gérer l'intervalle d'affichage entre 2 images du jeu
ENTREE : La valeur donnée par le main
SORTIE : Non

*/
void delay(unsigned int frameLimit){
    // Gestion des 60 fps (images/seconde)
    unsigned int ticks = SDL_GetTicks();

    if (frameLimit < ticks)
    {
        return;
    }

    if (frameLimit > ticks + 16)
    {
        SDL_Delay(16);
    }

    else
    {
        SDL_Delay(frameLimit - ticks);
    }
}

/*
BUT : Libérer la mémoire en fin de programme
ENTREE : La fenêtre de jeu, la police
SORTIE : La mémoire libérée

*/

void destroy(game *myGame){


   // if(mFont->g_font!=NULL){
  //      TTF_CloseFont(mFont->g_font); /* Doit être avant TTF_Quit() */
   //     mFont->g_font=NULL;
  //  }

    //Destroy texture
    if(myGame->g_texture!=NULL)
            SDL_DestroyTexture(myGame->g_texture);


    //Destroy render
    if(myGame->g_pRenderer!=NULL)
        SDL_DestroyRenderer(myGame->g_pRenderer);


    //Destroy window
    if(myGame->g_pWindow!=NULL)
        SDL_DestroyWindow(myGame->g_pWindow);



}

/*
BUT : Dessiner une balle vide
ENTREE : L'instance du jeu, les coordonnees du milieu, le rayon
SORTIE : L'affichage de la balle

*/

void affiche_balle(game *myGame,int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0+x,y0+y);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0+y,y0+x);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0-y,y0+x);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0-x,y0+y);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0-x,y0-y);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0-y,y0-x);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0+y,y0-x);
        SDL_RenderDrawPoint(myGame->g_pRenderer,x0+x,y0-y);

        y=y+1;
        if (err <= 0)
        {
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void affiche_score(game *myGame, int score1, int score2){
    SDL_Color fontColor={255,255,255};
    char buffer[30];
    sprintf(buffer,"%i - %i",score1,score2);

    //myGame->g_surface=TTF_RenderText_Blended(mFont.g_font, buffer, fontColor);//Charge la police

        if(myGame->g_surface){
                SDL_Rect rectangle;
                rectangle.x=(SCREEN_WIDTH/2)-100;//debut x
                rectangle.y=20;//debut y
                rectangle.w=100; //Largeur
                rectangle.h=50; //Hauteur

                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation de la texture pour la chaine
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                 if(myGame->g_texture){

                        SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,NULL,&rectangle); // Copie du sprite grâce au SDL_Renderer
                 }
                 else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }

        }else{
            fprintf(stdout,"Échec de creation surface pour chaine (%s)\n",SDL_GetError());
        }

}

void dplcmt_balle(balle_jeu *balle, SDL_Rect j1, SDL_Rect j2, int *score1, int *score2){

        //deplacement balle
         if (balle->direction==1){
            balle->milieu.x=balle->milieu.x-2;
            balle->milieu.y=balle->milieu.y-2;
          }else if (balle->direction==2){
            balle->milieu.x=balle->milieu.x+2;
            balle->milieu.y=balle->milieu.y-2;
          }else if (balle->direction==3){
            balle->milieu.x=balle->milieu.x+2;
            balle->milieu.y=balle->milieu.y+2;
          }else if (balle->direction==4){
            balle->milieu.x=balle->milieu.x-2;
            balle->milieu.y=balle->milieu.y+2;
          }

        //rebond raquette 1
          if ((balle->milieu.x==40+balle->rayon) && ((balle->milieu.y<=j1.y+j1.h) && (balle->milieu.y>=j1.y)) && (balle->direction==1)){
            balle->direction=2;
            }else if ((balle->milieu.x==40+balle->rayon) && ((balle->milieu.y<=j1.y+j1.h) && (balle->milieu.y>=j1.y)) && (balle->direction==4)){
                balle->direction=3;
        //rebond raquette 2
          }else if ((balle->milieu.x==SCREEN_WIDTH-40-balle->rayon) && ((balle->milieu.y<=j2.y+j2.h) && (balle->milieu.y>=j2.y)) && (balle->direction==3)){
            balle->direction=4;
          }else if ((balle->milieu.x==SCREEN_WIDTH-40-balle->rayon) && ((balle->milieu.y<=j2.y+j2.h) && (balle->milieu.y>=j2.y)) && (balle->direction==2)) {
            balle->direction=1;
          }

          //rebond bord haut
          if (balle->milieu.y==balle->rayon){
            if (balle->direction==1){
              balle->direction=4;
            }else{
              balle->direction=3;
            }
          }

        //rebond bord bas
          if (balle->milieu.y==SCREEN_HEIGHT-balle->rayon){
            if (balle->direction==4){
              balle->direction=1;
            }else{
              balle->direction=2;
            }
          }

          // ajout points
          if (balle->milieu.x==10) {
            balle->milieu.x=SCREEN_WIDTH/2;
            balle->milieu.y=SCREEN_HEIGHT/2;
            balle->direction=rand()%4+1;
            *score2+=1;
         }

          if (balle->milieu.x==SCREEN_WIDTH-10) {
            balle->milieu.x=SCREEN_WIDTH/2;
            balle->milieu.y=SCREEN_HEIGHT/2;
            balle->direction=rand()%4+1;
            *score1+=1;
        }

}

/*
BUT : Réaliser un pong avec la SDL 2
ENTREE :
SORTIE :

*/
