#ifndef _GRAPHIC_H_ 
#define _GRAPHIC_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../graphic/graphic_tools.h"

#define ICON_SIZE 15 // pixels de coté

// Gestion du rendu
void CreateRenderer();
void DestroyRenderer();
void OSM_Rendering(SDL_Window *pWindow, int w, int h, OSM_Data *data);
void RefreshView();

// Affichage des différents éléments
void drawWay(SDL_Renderer *ren, OSM_Way *way);
void draw_openedWay(SDL_Renderer *ren, OSM_Way *way, STYLE_ENTRY *style);
void draw_closedWay(SDL_Renderer *ren, OSM_Way *way, STYLE_ENTRY *style);
void drawNode(SDL_Renderer *ren, OSM_Node *node);
void drawRelationOuter(SDL_Renderer *ren, OSM_Relation *rel);
void drawRelationInner(SDL_Renderer *ren, OSM_Relation *rel);
void drawTexte(SDL_Renderer *ren, int x, int y, int w, int h, char *font, int size, char *texte, SDL_Color *color);
void drawImage(SDL_Renderer *ren, char* file_img, int x, int y);
void drawOSM_ABR(ABR_Node *tree);
void _aapolygonRGBA(SDL_Renderer *renderer, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Fonctions de positionnement
int lat2y(double lat);
int lon2x(double lon);

// Fonctions de zooom
void upScale();
void downScale();

// Fonctions de déplacement
void moveUP();
void moveDOWN();
void moveRIGTH();
void moveLEFT();

#endif
