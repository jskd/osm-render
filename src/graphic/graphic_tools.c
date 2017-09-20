#include "graphic_tools.h"

STYLE_ENTRY _dico[DICO_SIZE] = {};
int NB_STYLES = 0;

/* Ouvre la feuille de style et construit le dico */
void openStyleSheet(char *file){
  FILE* f = fopen(file, "r");

  if(f != NULL){
    char buff[1024];

    while(fgets(buff, sizeof(buff), f) != NULL){
      NB_STYLES++;
    } rewind(f);

    int i = 0;
    char* argv[64];
    int priority = 0;

    while(fgets(buff, sizeof(buff), f) != NULL){
      tokenize_command(buff, argv);

      char *key = malloc(64 * sizeof(char));
      char *value = malloc(64 * sizeof(char));
      char *file_img = malloc(64 * sizeof(char));

      strcpy(key, argv[0]);
      strcpy(value, argv[1]);
      if(argv[9] != NULL) strcpy(file_img, argv[9]);
      else strcpy(file_img, "");

      _dico[i].key = key;
      _dico[i].value = value;
      _dico[i].weigth = atoi(argv[2]);
      RGBA_COLOR color_IN = {atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), 255};
      RGBA_COLOR color_OUT = {atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), 255};
      _dico[i].color_IN = color_IN;
      _dico[i].color_OUT = color_OUT;
      _dico[i].file_img = file_img;
      _dico[i].priority = priority;
      priority++;
      i++;
    }
  }
  else{
    fprintf(stderr, "%s\n", "Erreur lors de l'ouverture du fichier.");
  }
}

// Retourne le style en fonction de la clé
STYLE_ENTRY* getStyleOf(char *key, char *value){
	for(int i=0; i<NB_STYLES; i++){
		if(strcmp(key, _dico[i].key) == 0 && strcmp(value, _dico[i].value) == 0){
			return &_dico[i];
		}
	}
	return NULL;
}

/* Création d'un tas de priorité minimum */
void CreateHeapPriority(minHeap *hp, OSM_Data* data){
  int priorite = 0;
  char *tag = "";
  char *value = "";

  for(int i=0; i< data->nb_way; i++){
    priorite = 99;

    if(data->ways[i].nb_tag > 0){
      for(int j=0; j < data->ways[i].nb_tag; j++){

        tag = data->ways[i].tags[j].k;
        value = data->ways[i].tags[j].v;
        STYLE_ENTRY *style = getStyleOf(tag, value);

        if(style != NULL){
          priorite = style->priority;
          break;
        }
      }
    } insertNode(hp, priorite, &data->ways[i]);
  }
}

/* Sépare une ligne en arguments */
int tokenize_command(char* argl, char** argv) {
  int i;
  argv[0] = strtok(argl, ARGSEP);
  for (i = 0; argv[i] != NULL; ++i)
      argv[i+1] = strtok(NULL, ARGSEP);
  return i;
}

/* Libère le dictionnaire de styles */
void freeDico(){
  for(int i=0; i<DICO_SIZE; i++){
    free(_dico[i].key);
    free(_dico[i].value);
    free(_dico[i].file_img);
  }
}


/* Test la présence d'un polygone dans la fenêtre */
int polyIsOnScreen(Sint16 *vx, Sint16 *vy, int size, int SCREEN_W, int SCREEN_H){
  int nb_points_out = 0;
  for(int i=0; i<size; i++){
    if( (vx[i] < 0 || vx[i] > SCREEN_W) || (vy[i] < 0 || vy[i] > SCREEN_H))
      nb_points_out++;
  }

  if(nb_points_out == size)
    return 0;
  else
    return 1;
}

/* Test la présence d'une ligne dans la fenêtre */
int lineIsOnScreen(int x1, int y1, int x2, int y2, int SCREEN_W, int SCREEN_H){
  if(((x1 >= 0 || x1 <= SCREEN_W) && (x2 >= 0 || x2 <= SCREEN_W)) || ((y1 >= 0 || y1 <= SCREEN_H) && (y2 >= 0 || y2 <= SCREEN_H)))
    return 1;
  else return 0;
}

/* Test la présence d'un point dans la fenêtre */
int pointIsOnScreen(int x, int y, int SCREEN_W, int SCREEN_H){
  if( (x >= 0 || x <= SCREEN_W) && (y >= 0 || y <= SCREEN_H) )
    return 1;
  else return 0;
}

/* Test si le node contient le tag */
int containTag(OSM_Tag *tags, int nb_tag, char *key, char *value){
  if(nb_tag > 0){
    for(int i=0; i < nb_tag; i++){
      if(strcmp(key,tags[i].k) == 0 && strcmp(value,tags[i].v) == 0){
        return 1;
      }
    }
  }
  return 0;
}

/* Test si la relation ne contient pas de membres NULL*/
int relationIsComplete(OSM_Relation *rel){
  for(int i=0; i < rel->nb_member; i++){
    if(strcmp(rel->members[i].role, "outer") == 0 && rel->members[i].type & OSM_MEMBER_REF_ID_BIT)
      return 0;
  }
  return 1;
}

/* Formule de mercator */
double y2lat_m(double y) { return rad2deg(2 * atan(exp( (y / earth_radius ) )) - M_PI/2); }
double x2lon_m(double x) { return rad2deg(x / earth_radius); }
double lat2y_m(double lat) { return earth_radius * log(tan(M_PI/4+ deg2rad(lat)/2)); }
double lon2x_m(double lon) { return deg2rad(lon) * earth_radius; }