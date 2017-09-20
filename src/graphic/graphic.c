#include <string.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include "graphic.h"
#include "graphic_tools.h"
#include "../model/minHeap.h"

SDL_Renderer *ren = NULL;
OSM_Data *data = NULL;
OSM_Way **_ways_by_prio = NULL;

/* Paramètres de la vue */
int SCREEN_W = 0; // Largeur de l'écran
int SCREEN_H = 0; // Hauteur de l'écran
int REF_X = 0; // Position X (centre d'affichage)
int REF_Y = 0; // Position Y (centre d'affichage)
double INTERVAL_X = 0; // Interval X en metres
double INTERVAL_Y = 0; // Interval Y en metres
double INIT_SCALE = 1; // Echelle initiale
double SCALE = 1; // Echelle courante
double MOVE = 50; // Valeur de déplacement

/* Création du render */
void CreateRenderer(SDL_Window *pWindow){
    ren = SDL_CreateRenderer(pWindow, 0, SDL_RENDERER_ACCELERATED);
}

/* Destruction du render */
void DestroyRenderer(){
  free(_ways_by_prio);
  freeDico();
  SDL_DestroyRenderer(ren);
}

/* Initialisation de la vue et des paramètres */
void OSM_Rendering(SDL_Window *pWindow, int w, int h, OSM_Data *_data){
  data = _data;
  SCREEN_W = w;
  SCREEN_H = h;
  REF_X = SCREEN_W / 2;
  REF_Y = SCREEN_H / 2;

  // Calcul du ratio permetant une couverture complète de la fenêtre
  INTERVAL_X = lon2x_m(data->bounds->maxlon) - lon2x_m(data->bounds->minlon);
  INTERVAL_Y = lat2y_m(data->bounds->maxlat) - lat2y_m(data->bounds->minlat);

  double ratio_X = SCREEN_W / INTERVAL_X;
  double ratio_Y = SCREEN_H / INTERVAL_Y;

  if(ratio_X > ratio_Y) INIT_SCALE = ratio_X;
  else INIT_SCALE = ratio_Y;

  INIT_SCALE = ceil(INIT_SCALE * 10)/10;
  SCALE = INIT_SCALE;

  /* Ouverture du fichier de styles */
  openStyleSheet("styles.txt");

  // Création du tas de priorités min
  minHeap priority_heap = initMinHeap(data->nb_way);
  CreateHeapPriority(&priority_heap, data);

  // Remplissage de la liste dans l'ordre de priorité
  _ways_by_prio = malloc(data->nb_way * sizeof(OSM_Way*));

  for(int i=0; i<data->nb_way; i++){
    _ways_by_prio[i] = getHead(&priority_heap);
    deleteNode(&priority_heap);
  }
  deleteMinHeap(&priority_heap);

  /* Création du renderer */
  CreateRenderer(pWindow);
  RefreshView();
}

void RefreshView(){
  SDL_SetRenderDrawColor(ren, 232, 229, 223, 255);
  SDL_RenderClear(ren); // Clear la fenêtre


  // Affichage des membres OUTTER des relations
  for(int i=0; i < data->nb_relation; i++){
    drawRelationOuter(ren, &data->relations[i]);
  }

  // Affichage des ways en fonction de leur priorité
  for(int i=0; i<data->nb_way; i++){
    drawWay(ren, _ways_by_prio[i]);
  }

  // Affichage des membres INNER des relations
  for(int i=0; i < data->nb_relation; i++){
    drawRelationInner(ren, &data->relations[i]);
  }

  // Affichage des nodes
  for(int i=0; i<data->nb_node; i++){
    drawNode(ren, &data->nodes[i]);
  }

  // Affichage texte ------------------------------
  //SDL_Color black = {0, 0, 0}; 
  //drawTexte(ren, 200, 200, 100, 50, "fonts/times.ttf", 80, "texte", &black);
  //  ----------------------------------------------*/

  SDL_RenderPresent(ren); // Affiche les modifications
}


/* Choisie la fonction d'affichage appropriée en fonction de la clé */ 
void drawWay(SDL_Renderer *ren, OSM_Way *way){
  char *key = "";
  char *value = "";
  STYLE_ENTRY *style = NULL;

  if(way->nb_tag > 0){
    for(int i=0; i < way->nb_tag; i++){
      key = way->tags[i].k;
      value = way->tags[i].v;
      style = getStyleOf(key, value);
      if(style != NULL) break;
      //else printf("[%s:%s]\n", key, value);
    }
  }
	
  // Si le style existe alors on affiche
  if(style != NULL){
    if(strcmp(key, "highway") == 0 || strcmp(key, "railway") == 0 || strcmp(key, "waterway") == 0|| strcmp(key, "barrier") == 0){
      if(strcmp(value, "riverbank") == 0)
        draw_closedWay(ren, way, style);
      else if(containTag(&way->tags[0], way->nb_tag, "area", "yes") == 1)
        draw_closedWay(ren, way, style);
      else
        draw_openedWay(ren, way, style);
    }
    else if(strcmp(key, "building") == 0 || strcmp(key, "natural") == 0 || strcmp(key, "landuse") == 0 || strcmp(key, "leisure") == 0){
      if(strcmp(value, "coastline") == 0){
        STYLE_ENTRY *water = getStyleOf("natural", "water");
        RGBA_COLOR *rgb_IN = &water->color_IN;
        short vx[4] = {-10, SCREEN_W+10, SCREEN_W+10, -10};
        short vy[4] = {-10, -10, SCREEN_H+10, SCREEN_H+10};
        filledPolygonRGBA(ren, vx, vy, 4, rgb_IN->r, rgb_IN->g, rgb_IN->b, rgb_IN->a);
        draw_closedWay(ren, way, style);
      }
      else
        draw_closedWay(ren, way, style);
    }
    else if(strcmp(key, "amenity") == 0){
      if(strcmp(value, "parking") == 0 || strcmp(value, "school") == 0 || strcmp(value, "college") == 0) 
        draw_closedWay(ren, way, style);
      if(strcmp(value, "fountain") == 0)
        draw_closedWay(ren, way, getStyleOf("natural", "water"));
    }   
  }
}


/* Affichage d'une way ouverte */
void draw_openedWay(SDL_Renderer *ren, OSM_Way *way, STYLE_ENTRY *style){
  if(style != NULL){
    int weigth = style->weigth;
  	RGBA_COLOR *rgb_IN = &style->color_IN;

    double latitude = 0;
    double longitude = 0;
    int x,y, x_suiv, y_suiv;

  	// Draw shape
    for(int i=0; i < (way->nb_node)-1 ; i++){

      latitude = way->nodes[i]->lat;
      longitude = way->nodes[i]->lon;
      x = lon2x(longitude);
      y = lat2y(latitude); 

      latitude = way->nodes[i+1]->lat;
      longitude = way->nodes[i+1]->lon;
      x_suiv = lon2x(longitude);
      y_suiv = lat2y(latitude);

      int onScreen = lineIsOnScreen(x, y, x_suiv, y_suiv, SCREEN_W, SCREEN_H);
      // Si la portion de route est dans la fenêtre on l'affiche
      if(onScreen){        
        if(weigth >= 2)
          filledCircleRGBA(ren, x, y, ((weigth * SCALE)/2), rgb_IN->r, rgb_IN->g, rgb_IN->b, rgb_IN->a);

        if(weigth >= 2)
          thickLineRGBA(ren, x, y, x_suiv, y_suiv, (weigth * SCALE), rgb_IN->r, rgb_IN->g, rgb_IN->b, rgb_IN->a);
        else
          aalineRGBA(ren, x, y, x_suiv, y_suiv, rgb_IN->r, rgb_IN->g, rgb_IN->b, rgb_IN->a);
      }
    }
    //printf("* draw_openedWay <%lu> [%s:%s] *\n", way->id, style->key, style->value);
  }
}

/* Affichage d'une way fermée */
void draw_closedWay(SDL_Renderer *ren, OSM_Way *way, STYLE_ENTRY *style){

  if(style != NULL){
  	RGBA_COLOR *rgb_IN = &style->color_IN;
    RGBA_COLOR *rgb_OUT = &style->color_OUT;

  	int nb_nodes = (way->nb_node);
  	short vx[nb_nodes];
  	short vy[nb_nodes];

  	for(int i=0; i < nb_nodes; i++){
      OSM_Node *nd = way->nodes[i];

      vx[i] = lon2x(nd->lon);
      vy[i] = lat2y(nd->lat);   
  	}

    int onScreen = polyIsOnScreen(vx, vy, nb_nodes, SCREEN_W, SCREEN_H);

    if(onScreen){
      filledPolygonRGBA(ren, vx, vy, nb_nodes, rgb_IN->r, rgb_IN->g, rgb_IN->b, rgb_IN->a);
      _aapolygonRGBA(ren, vx, vy, nb_nodes, rgb_OUT->r, rgb_OUT->g, rgb_OUT->b, rgb_OUT->a);
    }
    //printf("* draw_closedWay <%lu> [%s:%s] *\n", way->id, style->key, style->value);
  }
}

/* Affichage des membres OUTER d'une relation */
void drawRelationOuter(SDL_Renderer *ren, OSM_Relation *rel){
  char *key = "";
  char *value = "";
  STYLE_ENTRY *relation_style = NULL;
  STYLE_ENTRY *outer_style = NULL;
  int nb_outer = 0;

  if(relationIsComplete(rel)){
    // Si c'est une relation de type multipolygon
    if(containTag(&rel->tags[0], rel->nb_tag, "type", "multipolygon") == 1 || containTag(&rel->tags[0], rel->nb_tag, "type", "boundary") == 1){
      //printf("%s\n", "--------------------------------------");
      //printf(" RELATION <%lu> MEMBERS <%d>\n", rel->id, rel->nb_member);

      // On récupère le style de la relation si il existe
      for(int i=0; i < rel->nb_tag; i++){
        key = rel->tags[i].k;
        value = rel->tags[i].v;

        relation_style = getStyleOf(key, value);
        if(relation_style != NULL){
          break;
        }
      }

      // Comptage du nombre de membres OUTER et recherche d'un style
      for(int i=0; i < rel->nb_member; i++){
        if(rel->members[i].type == OSM_MEMBER_WAY_REF_STRUCT){
          OSM_Way *way = rel->members[i].ref;

          if(strcmp(rel->members[i].role, "outer") == 0){
            // Si il n'y a pas de styles encore défini pour les OUTER
            if(outer_style == NULL){
              // Recherche d'un tag de style
              for(int j=0; j < way->nb_tag; j++){
                outer_style = getStyleOf(way->tags[j].k, way->tags[j].v);
                if(outer_style != NULL) break;
              }
            }
            nb_outer++;
          }
        }
      }

      //if(relation_style != NULL) printf("STYLE Relation = [%s:%s]\n", relation_style->key, relation_style->value);
      //if(outer_style != NULL) printf("STYLE Outer = [%s:%s]\n", outer_style->key, outer_style->value);

      /* AFFICHAGE DES MEMBRES OUTER */

      // Si la relation à plusieurs membres OUTER
      if(nb_outer > 1){
        if(relation_style != NULL){
          // Affichage de tout les membres avec le style de la relation
          for(int i=0; i < rel->nb_member; i++){
            if(rel->members[i].type == OSM_MEMBER_WAY_REF_STRUCT){
              OSM_Way *way = rel->members[i].ref;

              if(strcmp(rel->members[i].role, "outer") == 0)
                draw_closedWay(ren, way, relation_style);
            }
          }
        }
        else{
          if(outer_style != NULL){
            // Affichage de tout les membres avec le premier style OUTER trouvé
            for(int i=0; i < rel->nb_member; i++){
              if(rel->members[i].ref != NULL){
                OSM_Way *way = rel->members[i].ref;

                if(strcmp(rel->members[i].role, "outer") == 0)
                  draw_closedWay(ren, way, outer_style);
              }
            }
          }
        }
      }
      else if(nb_outer == 1){ // Si la relation à un seul membre OUTER
        if(relation_style != NULL){
          for(int i=0; i < rel->nb_member; i++){
            if(rel->members[i].type == OSM_MEMBER_WAY_REF_STRUCT){
              OSM_Way *way = rel->members[i].ref;

              if(strcmp(rel->members[i].role, "outer") == 0)
                draw_closedWay(ren, way, relation_style);
            }
          }
        }
        else{
          for(int i=0; i < rel->nb_member; i++){
            if(rel->members[i].type == OSM_MEMBER_WAY_REF_STRUCT){
              OSM_Way *way = rel->members[i].ref;

              if(strcmp(rel->members[i].role, "outer") == 0)
                drawWay(ren, way);
            }
          }
        }
      }
      //printf("%s\n", "--------------------------------------");
    }
  }
}

/* Affichage des membres INNER d'une relation */
void drawRelationInner(SDL_Renderer *ren, OSM_Relation *rel){
  STYLE_ENTRY *outer_style = NULL;

  if(relationIsComplete(rel)){
    // Si c'est une relation de type multipolygon
    if(containTag(&rel->tags[0], rel->nb_tag, "type", "multipolygon") == 1 || containTag(&rel->tags[0], rel->nb_tag, "type", "boundary") == 1){

      // Recherche d'un style OUTER
      for(int i=0; i < rel->nb_member; i++){
        if(rel->members[i].type == OSM_MEMBER_WAY_REF_STRUCT){
          OSM_Way *way = rel->members[i].ref;

          if(strcmp(rel->members[i].role, "outer") == 0){
            // Si il n'y a pas de styles encore défini pour les OUTER
            if(outer_style == NULL){
              // Recherche d'un tag de style
              for(int j=0; j < way->nb_tag; j++){
                outer_style = getStyleOf(way->tags[j].k, way->tags[j].v);
                if(outer_style != NULL) break;
              }
            }
          }
        }
      }

      // AFFICHAGE DES MEMBRES INNER
      for(int i=0; i < rel->nb_member; i++){
        if(rel->members[i].ref != NULL){
          OSM_Way *way = rel->members[i].ref;

          if(strcmp(rel->members[i].role, "inner") == 0){
            if(outer_style != NULL){
              if(strcmp(outer_style->key, "building") == 0)
                draw_closedWay(ren, way, getStyleOf("landuse", "residential"));
              else
                drawWay(ren, way);
            }
            else{
              drawWay(ren, way);
            }
          }
        }
      }

    }
  }
}

/* Affichage d'une surface texte */
void drawTexte(SDL_Renderer *ren, int x, int y, int w, int h, char *font, int size, char *texte, SDL_Color *color){
    TTF_Font* font_ttf = TTF_OpenFont(font, size);

    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font_ttf, texte, *color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage); 
    SDL_Rect Message_rect;
    Message_rect.x = x; 
    Message_rect.y = y;
    Message_rect.w = w; 
    Message_rect.h = h; 
    SDL_RenderCopy(ren, Message, NULL, &Message_rect);

    TTF_CloseFont(font_ttf);
}

/* Affiche une image */
void drawImage(SDL_Renderer *ren, char* file_img, int x, int y){
  /* Chargement de l'image et affichage */
  if(strcmp(file_img, "") != 0){
    SDL_Surface *img = IMG_Load(file_img);
    if(!img) { printf("IMG_Load: %s\n", IMG_GetError());}
    if(img != NULL){
      SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, img);
      SDL_Rect dest = { x - (ICON_SIZE/2), y - (ICON_SIZE/2), ICON_SIZE, ICON_SIZE};
      SDL_RenderCopy(ren,texture,NULL,&dest);
      SDL_DestroyTexture(texture);
      SDL_FreeSurface(img);
    }
  }
}


/* Affichage d'un node */
void drawNode(SDL_Renderer *ren, OSM_Node *node){
  STYLE_ENTRY *style = NULL;
  char *key = "";
  char *value = "";

  int longitude = lon2x(node->lon);
  int latitude = lat2y(node->lat);

  int onScreen = pointIsOnScreen(longitude, latitude, SCREEN_W, SCREEN_H);
  if(onScreen){
    if(node->nb_tag > 0){
      for(int i=0; i<node->nb_tag; i++){
        key = node->tags[i].k;
        value = node->tags[i].v;
        style = getStyleOf(key, value);
        if(style != NULL) break;
      }

      if(style != NULL){
        drawImage(ren, style->file_img, longitude, latitude);
      }
    }
  }
  //filledCircleRGBA(ren, lon2x(node->lon), lat2y(node->lat), 2, 50, 50, 50, 255);
}

/* Parcours l'ABR et l'affiche */
void drawOSM_ABR(ABR_Node *tree){
	if(!tree) return;
  if(tree->left)  drawOSM_ABR(tree->left);
  drawNode(ren, (OSM_Node *)&tree->nd);
  if(tree->right) drawOSM_ABR(tree->right);
}

/* Affichage d'un polygone avec anti-aliasing */
void _aapolygonRGBA(SDL_Renderer *renderer, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
  for(int i=0; i < n-1; i++){
    aalineRGBA(renderer, vx[i], vy[i], vx[i+1], vy[i+1], r, g, b, a);
  }
  aalineRGBA(renderer, vx[n-1], vy[n-1], vx[0], vy[0], r, g, b, a);
}

/* Retourne le pixel X en fonction de la longitude */
int lon2x(double lon){
  return (REF_X + (lon2x_m(lon) - lon2x_m(data->bounds->minlon) - (INTERVAL_X / 2)) * SCALE);
}

/* Retourne le pixel Y en fonction de la latitude */
int lat2y(double lat){
  return SCREEN_H - (REF_Y + (lat2y_m(lat) - lat2y_m(data->bounds->minlat) - (INTERVAL_Y / 2)) * SCALE);
}

/* Augmente l'échelle */
void upScale(){
  if(SCALE + 0.1 < 10) SCALE = SCALE + 0.1;
  RefreshView();
}

/* Diminue l'échelle */
void downScale(){
  if(SCALE - 0.1 >= 0.1) SCALE = SCALE - 0.1;
  RefreshView();
}

/* Déplacement vers le NORD */
void moveUP(){
  REF_Y -= MOVE;
  RefreshView();
}

/* Déplacement vers le SUD */
void moveDOWN(){
  REF_Y += MOVE;
  RefreshView();
}

/* Déplacement vers l'OUEST */
void moveRIGTH(){
  REF_X -= MOVE;
  RefreshView();
}

/* Déplacement vers le l'EST */
void moveLEFT(){
  REF_X += MOVE;
  RefreshView();
}
