#ifndef CLUSTER_BOMB_H
#define CLUSTER_BOMB_H
#include "weapons.h"
#include <vector>

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class ClusterBomb : public Cweapon
{
public:
    ClusterBomb(const char* file_path, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);

    bool Shoot_fragments(Cmap* map, Cworm* worm);
    int number_of_fragments(int power);
    int _power;

    ALLEGRO_TIMER* iteration;
    ALLEGRO_TIMER* time_to_explosion;
    int64_t past_refresh;

    ALLEGRO_BITMAP* fragments;
};


#endif

