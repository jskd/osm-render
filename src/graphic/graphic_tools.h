#ifndef _GRAPHIC_TOOLS_H_ 
#define _GRAPHIC_TOOLS_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../model/osm_types.h"
#include "../model/OSM_ABR.h"
#include "../model/minHeap.h"

#define ARGSEP " :\t\n"
#define DICO_SIZE 1024

#define deg2rad(d) (((d)*M_PI)/180)
#define rad2deg(d) (((d)*180)/M_PI)
#define earth_radius 6378137

typedef struct{
	int r;
	int g;
	int b;
	int a;
} RGBA_COLOR;

typedef struct{
	char *key;
	char *value;
	int weigth;
	RGBA_COLOR color_IN;
	RGBA_COLOR color_OUT;
	char *file_img;
	int priority;
} STYLE_ENTRY;

// Gestion des styles
void openStyleSheet(char *file);
STYLE_ENTRY* getStyleOf(char *key, char *value);
void CreateHeapPriority(minHeap *hp, OSM_Data* data);
int tokenize_command(char* argl, char** argv);
void freeDico();

// Fonctions de tests
int polyIsOnScreen(Sint16 *vx, Sint16 *vy, int size, int SCREEN_W, int SCREEN_H);
int lineIsOnScreen(int x1, int y1, int x2, int y2, int SCREEN_W, int SCREEN_H);
int pointIsOnScreen(int x, int y, int SCREEN_W, int SCREEN_H);
int containTag(OSM_Tag *tags, int nb_tag, char *key, char *value);
int relationIsComplete(OSM_Relation *rel);

// Mercator functions
double y2lat_m(double y);
double x2lon_m(double x);
double lat2y_m(double lat);
double lon2x_m(double lon);

#endif