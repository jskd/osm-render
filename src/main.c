#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>

#include "parser/osm_parser.h"

#include "graphic/graphic.h"
#include "model/OSM_ABR.h"

#define SCREEN_W 950
#define SCREEN_H 620

#define FLAGS_IMG (IMG_INIT_JPG|IMG_INIT_PNG)

void Init_SDL();
void CreateWindow(int w, int h);
void Quit_SDL();
void EventsLoop();

SDL_Window* pWindow = NULL;
SDL_Event evenements;
int terminer = 0;

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "%s\n", "Usage: make run map=file ");
        exit(1);
    }

    if(access(argv[1], F_OK ) != 0){
        fprintf(stderr, "ERREUR : Le fichier n'existe pas.\n");
        exit(1);
    }

    OSM_Data* osmData;
    getOSM_data(argv[1], &osmData);

  // ################# AFFICHAGE ##################

    /* Initialisation */
    Init_SDL();
    /* Création de la fenêtre */
    CreateWindow(SCREEN_W, SCREEN_H);
	/* Rendu OSM */
	OSM_Rendering(pWindow, SCREEN_W, SCREEN_H, osmData);
	/* Boucle d'évenements */
	EventsLoop();
	/* Fermeture de la SDL */
	Quit_SDL();
	/* Free data */	
	freeOSM_data(osmData);

	exit(0);
}

void Init_SDL(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0 ){
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        exit(1);
    }
    if((IMG_Init(FLAGS_IMG)&FLAGS_IMG) != FLAGS_IMG) {
        fprintf(stdout,"Échec de l'initialisation de la SDL_image (%s)\n",IMG_GetError());
        exit(1); 
    }
    if (TTF_Init() != 0){
        fprintf(stdout,"Échec de l'initialisation de la SDL_ttf (%s)\n",SDL_GetError());
        exit(1); 
    }
}

void CreateWindow(int w, int h){
    pWindow = SDL_CreateWindow("OSM_Render",SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
}


void Quit_SDL(){
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}

void EventsLoop(){
    if( pWindow ){
        while(!terminer){
            SDL_WaitEvent(&evenements);
            switch(evenements.type){
                case SDL_WINDOWEVENT:
                    if(evenements.window.event == SDL_WINDOWEVENT_CLOSE)
                        terminer = 1;
                break;
                case SDL_KEYUP:
                    if(evenements.key.keysym.sym == SDLK_EQUALS){
                        upScale();
                    }
                    if(evenements.key.keysym.sym == SDLK_MINUS){
                        downScale();
                    }
                    if(evenements.key.keysym.sym == SDLK_UP){
                        moveUP();
                    }
                    if(evenements.key.keysym.sym == SDLK_DOWN){
                        moveDOWN();
                    }
                    if(evenements.key.keysym.sym == SDLK_RIGHT){
                        moveRIGTH();
                    }
                    if(evenements.key.keysym.sym == SDLK_LEFT){
                        moveLEFT();
                    }
                break;
            }
    
        }

        DestroyRenderer();
        SDL_DestroyWindow(pWindow);
    }
    else{
        fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
        SDL_Quit();
        exit(1);
    }   
}
