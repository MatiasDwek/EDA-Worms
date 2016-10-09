#ifndef FIREPUNCH_H
#define FIREPUNCH_H
#include "weapons.h"
#define W_HIT_F_SPEED (6.0)
#define W_HIT_F_ANGLE 50
#define FP_SPEED (4.5)


class FirePunch : public Cweapon
{
    public:
    FirePunch(const char* projectilePath, const char* file_path_worm);
    virtual int getDamage(int power);
    virtual int getDiameter(int power);
    virtual float getSpeed(int power);
    virtual float getExplosionBias(void);
    virtual bool readyToShoot(void);
    virtual bool Shoot(Cmap* map, Cworm* worm);
    virtual void createShotW(int angle, int power, Point pos, int fuse, int ID);
    virtual void clearShots(void);
    virtual bool checkExplosion(void);
};
Velocity getVelocityFP(Point posWvictim, Point posWattacker, int power);
vector<Cworm*> checkWormsHitFP(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms, vector<Cworm>& enemyWorms, vector<Cworm*> prevHit);


#endif //FIREPUNCH_H
