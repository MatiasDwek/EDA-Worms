#ifndef GRAPHICDEFINES
#define GRAPHICDEFINES

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_image.h>
#include <cmath>

//general
#define WIDTH 1920
#define HEIGHT 861 //842
#define FONT_SIZE 15
#define FONT_SIZE_FOR_TIME 45
#define FRAMERATE 0.02
#define TRANSPARENT_ 0x00 //TRANSPARENT esta definido x allegro
#define AL_RED 255,0,0
#define AL_BLUE 0,0,255
typedef enum {RED = 'r', BLUE = 'b'}Color;

#define ENTORNO_DELTA 1   //tiene dimension uno. x ej si quiero ver si el worm cruza una recta, en realidad
                            //me fijo si pasa x un rectangulo de ancho ENTORNO_DELTA
#define WATER_HEIGHT 10
#define GRAVITY (0.24)      //en px/frame^2
#define PI 3.14159265


//worms
#define W_HEIGHT 26 //alto del worm
#define W_WIDTH 20  //alto del worm
#define W_HEIGHT_PNG 60 //ANCHO DE LOS PNG DE LOS WORMS
#define W_WIDTH_PNG 60
    /*no las uso por el momento:*/
#define W_START_X 21//posicion desde donde empieza el worm (cando esta parado)respecto de su .png
#define W_START_Y 16
    //walk
    #define WALKING_UNIT 27 //menos que esto no se puede caminar
    #define W_AV 3 //numero de avances de los worms x unidad avanzada (en los frames bajados en c/u el worm avanza 9px)
    #define WWALK_FRAMES 15
    #define WWALK_MAX_DELTA 8 //el maximo cambio vertical es de 8 pixeles para continuar caminando
    #define SHIFT_NUMBERS (j==1 || j==2 || j==3 || j==4 || j==5 || j==6 || j==7)
    //jump
    #define WJUMP_SPEED (4.5)
    #define WJUMP_ANGLE 60
    #define WJUMP_FRAMES 10
    #define WJUMP_SUBDIVISIONES 5



//weapons
#define WBAZOOKA_FRAMES 7



struct Gif
{
    ALLEGRO_BITMAP *wwalkL[WWALK_FRAMES]; //tiene los frames del desplazamiento del worm
    ALLEGRO_BITMAP *wwalkR[WWALK_FRAMES];
    ALLEGRO_BITMAP *wjumpL[WJUMP_FRAMES]; //tiene los frames del salto del worm
    ALLEGRO_BITMAP *wjumpR[WJUMP_FRAMES];
    ALLEGRO_BITMAP *wUnloadBazookaL[WBAZOOKA_FRAMES];
    ALLEGRO_BITMAP *wUnloadBazookaR[WBAZOOKA_FRAMES];
    ALLEGRO_BITMAP *wAirR;
    ALLEGRO_BITMAP *wAirL;
};

#endif // GRAPHICDEFINES
