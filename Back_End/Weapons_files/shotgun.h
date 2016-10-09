#ifndef SHOTGUN_H
#define SHOTGUN_H
#include "weapons.h"

class Shotgun : public Cweapon
{
public:
    Shotgun(const char* file_path, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);

    ALLEGRO_SAMPLE* reloading_sound;
};



#endif // SHOTGUN_H
