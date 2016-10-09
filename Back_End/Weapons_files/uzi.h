#ifndef UZI_H
#define UZI_H
#include "weapons.h"

class Uzi : public Cweapon
{
public:
    Uzi(const char* file_path, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);

    ALLEGRO_BITMAP* worm_holding_uzi;
};



#endif // UZI_H

