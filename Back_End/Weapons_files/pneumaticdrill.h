#ifndef PNEUMATICDRILL_H
#define PNEUMATICDRILL_H
#include "weapons.h"
#define W_HIT_PD_SPEED (0.0)
#define PD_SPEED (0.628)
#define PD_FRAMES 250 //en frames


class PneumaticDrill : public Cweapon
{
    public:
    PneumaticDrill(const char* projectilePath, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    Velocity getVelocity(Point posWvictim, Point posWattacker, int power);
    vector<Cworm*> checkWormsHit(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms, vector<Cworm>& enemyWorms, vector<Cworm*> prevHit);
    virtual bool checkExplosion(void);
    virtual void clearShots(void);
};

#endif //PNEUMATICDRILL_H

