#ifndef __DEF_WEAPONS
#define __DEF_WEAPONS

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


#include "../../Front_End/Defines/graphicdefines.h"
#include "../Defines/gamedefines.h"
//#include

//#define MAX_WEAPON_NAME
#define WORM_CENTER_FIX 30

using namespace std;

//el mapa tiene worms. Los worms tienen armas. Las armas no tienen nada

#include <vector>
#include "../Defines/point.h"

#define WEAPON_GRAPHICS_TOLERANCE 50

enum weapon_types {NORMAL, FUSE, SHOTGUN, PD, FP, UZI};

class Cmap;
class Cworm;

 struct Shot
{
    int angle;      //en grados. -91<angle<91
    int power;   //fuerza del disparo
    int type;           //de que? : esto es para diferenciar los diferentes tipos de tiro, por ejemplo, con fuse, homing missile, land mine etc...
    int damage;
    Point start;
    Point position;     //de que?posicion actual? : posicion actual del proyectil
    Point destination;
    Velocity velocity;
    int64_t fuse; //Or operation time
    int diameter;
    float explosion_bias;
    int shooter_ID;
};

class Cweapon
{
public:
    ~Cweapon();
    virtual int getDamage(int power) = 0;
    virtual int getDiameter(int power) = 0;
    virtual float getSpeed(int power) = 0;
    virtual float getExplosionBias(void) = 0;
    virtual bool readyToShoot(void) = 0;
    virtual bool Shoot(Cmap* map, Cworm* worm) = 0; //cuando retorna falso? : no me acuerdo, capaz que nunca o capaz despues srive para algo
    virtual void createShotW(int angle, int power, Point Pos, int fuse, int ID) = 0;
    virtual bool checkExplosion(void) = 0;

    virtual void clearShots(void) = 0;



    vector<Shot> Shots;

    ALLEGRO_BITMAP* image;
    ALLEGRO_BITMAP* worm_with_gun;
    int number_of_shots;
    char weapon;
    char type;
    Point number;
    Point button_start;
    Point button_end;

    ALLEGRO_SAMPLE* explosion_sound;
    ALLEGRO_SAMPLE_ID sample_id;
};

class Bazooka : public Cweapon
{
public:
    Bazooka(const char* file_path, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point Pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);

    ALLEGRO_SAMPLE* release_sound;
};

Shot createShot(int angle, int power, int type, Point start, Point destination, float speed, int64_t fuse, int diameter, float explosion_bias, int damage, int shooter_ID);
bool shoot(vector<Shot>& Shots, Cmap* map, Cworm* worm);
bool isInsideMap(int x, int y, Cmap* map);
float getWindStrength(char charWindStrength);
bool impact(int x, int y, Cmap* map);
bool impactWorms(int x, int y, int shooter_ID, Cmap* map);
void drawShot(Shot shot, float x, float y, float vx, float vy, Cmap* map, Cworm* worm);
vector<Cworm*> checkWormsHit(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms, vector<Cworm>& enemyWorms);
bool wormsMoving();
Velocity getVelocity(Point impact_point, Point worm_position, int diameter);
void moveWormsHit(vector<Cworm*>& worms_hit, Cmap* map);


//class Cweapon //: public Cworm
//{
//public:
////    weapon(){};
//    Cweapon();
//     virtual list <Shot>* fire(); //no le gusta el =0
//     virtual int getHitPoints(int strength);
//};
//etc ettc. Faltan muchas cosas
//class Bazooka:public Cweapon
//{
//
//};

#endif
