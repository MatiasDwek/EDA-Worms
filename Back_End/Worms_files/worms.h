#ifndef __DEF_WORMS
#define __DEF_WORMS

#include "../../Front_End/Defines/graphicdefines.h"
#include "../Defines/gamedefines.h"

#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>

#include "../Weapons_files/weapons.h" //el mapa tiene worms. Los worms tienen armas. Las armas no tienen nada
#include "../Defines/point.h"
#include <vector>
#include <string>

#define WORM_WIDTH 16
#define WORM_HEIGHT 28

#define WORM_HALF_WIDTH 8
#define WORM_HALF_HEIGHT 14

using namespace std;

class Cmap;

typedef enum {LEFT, RIGHT}Orientation;
typedef enum {CAIDA_LIBRE, WALK, DO_NOTHING} Waction;
typedef enum {AIR, GROUND}Wstate;

class Cworm//:Cmap
{
    //El worm se lo modela como un cuadrado de 30x30px.
    //pos tiene la posición de la esquina superior izquierda (misma convencion que allegro)

public:
    Cworm(ALLEGRO_DISPLAY *display,/*armaPorDefecto,*/ Point pos, Orientation ori,ALLEGRO_BITMAP *mapa,
          ALLEGRO_BITMAP* background, Gif* gif, int wormID, Cmap* pMap, Bazooka* _Bazooka);
    Cworm(const Cworm &worm);
    ALLEGRO_DISPLAY *display;
    //Cweapon weapon;
    Point pos;
    Orientation orientation; //LEFT O RIGHT
    ALLEGRO_BITMAP *mapa;       //el worm debe tener la info del mapa para hacer walking()
    ALLEGRO_BITMAP *background; //copia del mapa orignial. Se va actualizando por los demás
    ALLEGRO_BITMAP *wwalkL[]; //tiene los frames de el desplazamiento del worm
    ALLEGRO_BITMAP *wwalkR[]; //tiene los frames de el desplazamiento del worm
    Gif* pGif;
    Cmap* pCmap;
    Cweapon* pCweapon;
    Color color;
    int ID; //ID del worm. arranca desde 0. Todas las ID's son distintas, sin importar si es equipo distinto
    string name;
    Wstate state;

    void walk(void);
        // Se hace un al_lock_bitmap() para agilizar la lectura de pixeles. El mapa no es modificable cuando esta lockeado.
            //RECORDAR CUANDO SE QUIERA MODIFICAR EL MAPA (cuando se dispara un arma por ej) , HACER
            //void al_unlock_bitmap(ALLEGRO_BITMAP *bitmap), Y LUEGO RE LOCKEARLO AL FINAL DE LA FUNCION.
    void shotWorm(double initSpeed, int angle); //"dispara" y grafica al worm con esa velocidad inicial y angulo entra en juego la orientation
                                            //sirve para caidaslibres, cuando xplota algo cerca de un worm  y sale volando, etcetc
    void shotWormNormal(double speed, int angle);
    void jump();
    void jumpLindo();
    void updateWormFrame(int j);
    void updateWormFrameMoving(int j);
    int realPosX(); //posiciones "verdaderas" de los worms(donde hay pixeles de worm dentro del png de worm)
    int realPosY();

    Velocity velocity;

    int health;

    Point impact_point;
    int impact_time;
    void drawHealthBar();
    int calculateFallDamage(double verticalSpeed);
    Point getJumpDestination();
    Point getShotDestination(double speed, int angle);
    Point getWalkDestination(void); //para que las PCs esten sincronizadas
    Waction nextAction; //proxima accion a realizar. Se modifica con las funciones que calculan los puntos y cosas de jump, etc.
    void showLoadBazooka();
    void showUnloadBazooka();
    void showBazookaFrame(int j);
    void showAlmostEverything();
    void showWormInAir();
    void showAllWormsButMyself();



private:
//funciones privadas
    bool isInsideMap();
    int getPhaseX();
    int getPhaseY(int j);

    bool noEstaChocandoseVert(int shift);
    bool canWalk();
    void showWwalkFrames();
    bool canJump();

    void showAllWormsBut(vector<int> IDs);
    void shotThisWorms(vector<int> IDs, vector<int> angles, int speed); //luego de una explosion todos tiene la misma initSpeed



// variables privadas
    Waction lastAction;


};




#endif
