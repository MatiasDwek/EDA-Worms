#include "firepunch.h"
#include "../Communication_files/server-client.h"
#include <iostream>


FirePunch::FirePunch(const char* projectilePath, const char* file_path_worm)
{
    image = al_load_bitmap(projectilePath);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'f';
    type = FP;
    explosion_sound = al_load_sample("./Sounds/firepunchimpact.wav");
    number.x = 4;
    number.y = 1;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading FirePunch image." << endl;
}

int FirePunch::getDamage(int power)
{
    return 21+3*power;
}
int FirePunch::getDiameter(int power)
{
    return 40;
}
float FirePunch::getSpeed(int power)
{
    return 4.5;
}
float FirePunch::getExplosionBias(void)
{
    return 0;
}
bool FirePunch::readyToShoot(void)
{
       if (Shots.size() == 1)
        return true;
    else
        return false;
}
bool FirePunch::Shoot(Cmap* map, Cworm* worm)
{
    int t = 0;
    float j;
    float temp_x;
    float temp_y;
    vector<Cworm*> worms_hit;
    vector<Cworm*> temp_worms_hit;
    vector<Shot>::iterator i;

    do
    {
    if (!Shots.empty())
        worm->showAllWormsButMyself();
    else
        map->showWorms();
    for (i = Shots.begin(); i != Shots.end(); i++)
    {
        i->position.x = i->start.x + i->velocity.x*t;
        i->position.y = i->start.y + i->velocity.y*t + GRAVITY * t * (t + 1) / 2;

        if (isInsideMap(i->position.x, i->position.y, map))
        {
            for (j = t; j < (t + 1); j = j + 0.1)  //avanzo de apoquito
            {
                temp_x = i->start.x + i->velocity.x * j;
                temp_y = i->start.y + i->velocity.y * j + GRAVITY * j * (j + 1) / 2;

                if (i->velocity.y+GRAVITY*j<=0)
                {
                    temp_worms_hit = checkWormsHitFP(*i, temp_x, temp_y,i->velocity.x,i->velocity.y + GRAVITY * j
                                                     ,map->myWorms, map->enemyWorms, worms_hit);
                    worms_hit.insert(worms_hit.end(), temp_worms_hit.begin(), temp_worms_hit.end());
                    if (!worms_hit.empty())
                        al_play_sample(explosion_sound, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE,NULL);
                }
                if (i->start.y - i->position.y < 0.1 && i->velocity.y+GRAVITY*j>0)
                {
                    Shots.erase(i);
                    i--;
                    break;
                }
            }
            if (t%4 == 1 && !Shots.empty()) //drawShot consume mucho tiempo. X eso solo la llamo una de cada 4veces
                drawShot(*i, temp_x, temp_y, i->velocity.x, i->velocity.y + GRAVITY * j, map, worm);
            al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, worm->orientation == RIGHT ? ALLEGRO_FLIP_HORIZONTAL : 0);
        }
        else
        {
            Shots.erase(i);
            i--;
            break;
        }
    }
    t += 1;
    moveWormsHit(worms_hit, map);
    al_flip_display();
    al_rest(FRAMERATE);
    } while(!Shots.empty() || !worms_hit.empty());
    map->showWorms();
    return true;
}
Velocity getVelocityFP(Point posWvictim, Point posWattacker, int power)
{
    float coef;
    if (1<=power && power<=5)
        coef = 0.7 + power*0.1;
    else
        coef = 5.0; //para ver si hay algun error
    Velocity vel = {coef*W_HIT_F_SPEED*cos(W_HIT_F_ANGLE*PI/180), -coef*W_HIT_F_SPEED*sin(W_HIT_F_ANGLE*PI/180)};
    if (posWattacker.x>posWvictim.x)
        vel.x = -vel.x;
    return vel;
}

void FirePunch::createShotW(int angle, int power, Point pos, int fuse, int ID)
{
    Shot shot;
    shot.angle = 90;
    shot.damage = getDamage(power);
    shot.diameter = getDiameter(power);
    shot.explosion_bias = getExplosionBias();
    shot.fuse = 0;
    shot.power = power;
    shot.shooter_ID = ID;
    shot.velocity.y = -FP_SPEED;
    shot.velocity.x = 0;
    shot.start.x = pos.x + WORM_CENTER_FIX;
    shot.start.y = pos.y + WORM_CENTER_FIX;

    Shots.push_back(shot);
}

vector<Cworm*> checkWormsHitFP(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms,
                                vector<Cworm>& enemyWorms, vector<Cworm*> prevHit)
{
    Point impact_point = {(int) x, (int) y};
    Point worm_real_pos;
    vector<Cworm*> hit_worms;
    vector<Cworm>::iterator i;
    vector<Cworm*>::iterator j;
    bool isHere;
    for (i = myWorms.begin(); i != myWorms.end(); i++)
    {
         worm_real_pos.x = i->pos.x + WORM_CENTER_FIX;
         worm_real_pos.y = i->pos.y + WORM_CENTER_FIX;
        if (i->ID != shot.shooter_ID)
            if (getDistanceBetweenPoints(worm_real_pos, impact_point) <= shot.diameter / 2.0)
            {
                isHere = false;
                for (j = prevHit.begin(); j!= prevHit.end(); j++)
                    if ((*j)->ID == i->ID)
                    {
                        isHere = true;
                        break;
                    }
                    if (!isHere)
                    {
                        Point st = shot.start; st.x -= WORM_CENTER_FIX; st.y -= WORM_CENTER_FIX;
                        i->velocity = getVelocityFP(i->pos,st, shot.power);
                        i->health -= shot.damage;
                        if (i->health < 0)
                            i->health = 0;
                        i->impact_time = 0;
                        i->impact_point = i->pos;
                        i->state = AIR;
                        hit_worms.push_back(&(*i));
                    }
                if (prevHit.size()==0)
                {
                    Point st = shot.start; st.x -= WORM_CENTER_FIX; st.y -= WORM_CENTER_FIX;
                    i->velocity = getVelocityFP(i->pos,st, shot.power);
                    i->health -= shot.damage;
                    if (i->health < 0)
                        i->health = 0;
                    i->impact_time = 0;
                    i->impact_point = i->pos;
                    i->state = AIR;
                    hit_worms.push_back(&(*i));
                }
            }
    }
    for (i = enemyWorms.begin(); i != enemyWorms.end(); i++)
    {
        worm_real_pos.x = i->pos.x + WORM_CENTER_FIX;
        worm_real_pos.y = i->pos.y + WORM_CENTER_FIX;

        if (i->ID != shot.shooter_ID)
            if (getDistanceBetweenPoints(worm_real_pos, impact_point) <= shot.diameter / 2.0)
            {
                isHere = false;
                for (j = prevHit.begin(); j!= prevHit.end(); j++)
                    if ((*j)->ID == i->ID)
                    {
                        isHere = true;
                        break;
                    }
                    if (!isHere)
                    {
                        Point st = shot.start; st.x -= WORM_CENTER_FIX; st.y -= WORM_CENTER_FIX;
                        i->velocity = getVelocityFP(i->pos,st, shot.power);
                        i->health -= shot.damage;
                        if (i->health < 0)
                            i->health = 0;
                        i->impact_time = 0;
                        i->impact_point = i->pos;
                        i->state = AIR;
                        hit_worms.push_back(&(*i));
                    }
                if (prevHit.size()==0)
                {
                    Point st = shot.start; st.x -= WORM_CENTER_FIX; st.y -= WORM_CENTER_FIX;
                    i->velocity = getVelocityFP(i->pos,st, shot.power);
                    i->health -= shot.damage;
                    if (i->health < 0)
                        i->health = 0;
                    i->impact_time = 0;
                    i->impact_point = i->pos;
                    i->state = AIR;
                    hit_worms.push_back(&(*i));
                }
            }
    }

    return hit_worms;
}

void FirePunch::clearShots(void)
{
    Shots.clear();
}

bool FirePunch::checkExplosion(void)
{
    return false;
}
