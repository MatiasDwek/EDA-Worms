#ifndef __DEF_MAP
#define __DEF_MAP

#include "../../Front_End/Defines/graphicdefines.h"
#include "../Defines/gamedefines.h"

#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>

using namespace std;
#include <vector>
#include <string>

#include "../Worms_files/worms.h" //el mapa tiene worms. Los worms tienen armas. Las armas no tienen nada

#define WIND_BAR_FIX 736
#define MAX_NUM_WORMS 6
#define LETTER_LEN 2.5

struct MapData
{
    int wormQuantity;
    vector<Point> blueTeamXY; //coordenadas de inicio de cada equipo para el mapa seleccionado
    vector<Orientation> blueTeamOri;
    vector<Point> redTeamXY;
    vector<Orientation> redTeamOri;
    char map_name[50];
    Color myTeamColor; //esto lo llena la maquina de estados ppal, segun quien comience el juego
    Color enemyColor;
};


class Cmap
{
public:
    vector<Cworm> myWorms; //es importante que sea red o blue?
    Color myWormsColor;
    vector<Cworm> enemyWorms;
    Color enemyWormsColor;
    Cmap(ALLEGRO_DISPLAY *display, MapData& mapData, string mapa_, string background_, char WindStrength_);
    ~Cmap();
    ALLEGRO_COLOR alGetAvgColor(ALLEGRO_BITMAP *map);
    ALLEGRO_BITMAP *mapa;
    ALLEGRO_BITMAP *background;
    ALLEGRO_BITMAP *water;
    ALLEGRO_BITMAP *bottom;
    ALLEGRO_BITMAP* top_my_turn;
    ALLEGRO_BITMAP* top_enemy_turn;
    ALLEGRO_BITMAP* wind_bar;
    ALLEGRO_BITMAP* button;
    ALLEGRO_BITMAP* button_pressed;
    ALLEGRO_BITMAP* button_selected;
    ALLEGRO_BITMAP* weapon_bar;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_FONT *font;
    ALLEGRO_FONT* font_for_time;
    ALLEGRO_FONT* font_for_time_small;
    ALLEGRO_FONT* font_for_fuse;
    ALLEGRO_MOUSE_STATE mouse_state;
    ALLEGRO_TIMER* turn_time;  ///Lo destruyo al final de runGame
    ALLEGRO_TIMER* total_time;  ///Lo destruyo al final de runGame
    char turn;
    Gif gif; //tiene todos los frames de wormWalking, etc etc
    MapData mapData;
    char WindStrength;
    ALLEGRO_COLOR avg_color;
    Point missile_destination;

    double resizeWidth;
    double resizeHeight;



    //funciones miembro:
    void showWorms();
    void fillMapData(string mapName);
    void createWorms(Bazooka* _Bazooka);
    bool deleteUnhealthyWorms();
    bool deleteDrownedWorms();
    void displayWater();

};



#endif
