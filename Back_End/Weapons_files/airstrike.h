#ifndef AIRSTRIKE_H
#define AIRSTRIKE_H
#include "weapons.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class AirStrike : public Cweapon
{
public:
    AirStrike(const char* file_path, const char* file_path_worm, Cmap* _map);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);
    int getNumberMissiles(int power);

    ALLEGRO_SAMPLE* release_sound;
    ALLEGRO_BITMAP* marker_b;
    ALLEGRO_BITMAP* marker_r;
    int number_of_missile;
    Cmap* map;
};

#endif
