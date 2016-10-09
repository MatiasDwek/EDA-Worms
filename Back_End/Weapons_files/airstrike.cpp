#include <iostream>
#include <cmath>
#include <vector>
#include "weapons.h"
#include "airstrike.h"
#include "../Map_files/map.h"
#include "../Communication_files/server-client.h"

using namespace std;

AirStrike::AirStrike(const char* file_path, const char* file_path_worm, Cmap* _map)
{
    image = al_load_bitmap(file_path);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'a';
    type = NORMAL;
    explosion_sound = al_load_sample("./Sounds/explosion1.wav");
    release_sound = al_load_sample("./Sounds/airstrike.wav");
    marker_b = al_load_bitmap("./Images/Weapons/airstrike/markerb.png");
    marker_r = al_load_bitmap("./Images/Weapons/airstrike/markerr.png");
    map = _map;
    number.x = 2;
    number.y = 1;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading AirStrike image." << endl;
}

bool AirStrike::Shoot(Cmap* map, Cworm* worm)
{
    float t = 0;
    float j;
    float temp_x;
    float temp_y;
    bool already_stopped = false;
    vector<Cworm*> worms_hit;
    vector<Cworm*> temp_worms_hit;
    vector<Shot>::iterator i;
    float wind = getWindStrength(map->WindStrength);

    do
    {
        map->showWorms();

        for (i = Shots.begin(); i != Shots.end(); i++)
        {
            i->position.x = i->start.x
                            + i->velocity.x * t
                            + wind * t * (t + 1) / 2;
            i->position.y = i->start.y
                            + i->velocity.y * t
                            + GRAVITY * t * (t + 1) / 2;

            if (isInsideMap(i->position.x, i->position.y, map))
            {
                for (j = t; j < (t + 1); j = j + 0.01)
                {
                    temp_x = i->start.x
                            + i->velocity.x * j
                            + wind * j * (j + 1) / 2;
                    temp_y = i->start.y
                            + i->velocity.y * j
                            + GRAVITY * j * (j + 1) / 2;

                    if (impact(temp_x, temp_y, map) || impactWorms(temp_x, temp_y, i->shooter_ID, map))
                    {
                        drawShot(*i, temp_x, temp_y, i->velocity.x + wind * j, i->velocity.y + GRAVITY * j, map, worm);

                        temp_worms_hit = checkWormsHit(*i, temp_x,
                                                       temp_y,
                                                       i->velocity.x + wind * j,
                                                       i->velocity.y + GRAVITY * j,
                                                       map->myWorms,
                                                       map->enemyWorms);
                        worms_hit.insert(worms_hit.end(), temp_worms_hit.begin(), temp_worms_hit.end());


                        Shots.erase(i);
                        i--;
                        if (already_stopped == false)
                        {
                            already_stopped = true;
                            al_stop_sample(&sample_id);
                        }

                        break;
                    }
                }

                    al_draw_rotated_bitmap(worm->pCweapon->image,
                                           al_get_bitmap_width(worm->pCweapon->image) / 2.0,
                                           al_get_bitmap_height(worm->pCweapon->image) / 2.0,
                                           i->position.x,
                                           i->position.y,
                                           PI - atan(-(i->velocity.y + GRAVITY * j)/(i->velocity.x + wind * j)),
                                           i->velocity.x + wind * t < 0 ? ALLEGRO_FLIP_HORIZONTAL : 0);
            }
            else
            {
                if (already_stopped == false)
                {
                    already_stopped = true;
                    al_stop_sample(&sample_id);
                }
                Shots.erase(i);
                i--;
            }
        }
        t += 1;
        moveWormsHit(worms_hit, map);
        al_flip_display();
        al_rest(FRAMERATE);
    } while(!Shots.empty() || !worms_hit.empty());

    return true;
}

int AirStrike::getDamage(int power)
{
    return power * 3 + 21;
}

int AirStrike::getDiameter(int power)
{
    switch (power)
    {
    case 1:
        return 47;
        break;
    case 2:
    case 3:
    case 4:
        return 61;
        break;
    case 5:
        return 73;
        break;
    default:
        return 1;
    }
}

float AirStrike::getSpeed(int power)
{
    return 0;
}

float AirStrike::getExplosionBias()
{
    return 0;
}

int AirStrike::getNumberMissiles(int power)
{
    switch (power)
    {
    case 1:
    case 2:
        return 4;
        break;
    case 3:
    case 4:
        return 5;
        break;
    case 5:
        return 6;
        break;
    default:
        return 4;
        break;
    }
}

bool AirStrike::readyToShoot(void)
{
    if (Shots.empty() == false)
        return true;
    else
        return false;
}

void AirStrike::createShotW(int angle, int power, Point start, int fuse, int ID)
{
    al_play_sample(release_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &sample_id);
    al_rest(.5);
    Shot temp_Shot;
    int temp_left = start.x;
    int temp_right = start.x;
    temp_Shot.angle = angle;
    temp_Shot.power = power;
    //temp_Shot.type = type;

    temp_Shot.start.x = start.x;
    temp_Shot.start.y = 1;
    //temp_Shot.destination = destination;
    temp_Shot.fuse = fuse;
    temp_Shot.diameter = getDiameter(power);
    temp_Shot.explosion_bias = getExplosionBias();
    temp_Shot.damage = getDamage(power);

    temp_Shot.velocity.x = getSpeed(power) * cos(angle * PI / 180.0);
    temp_Shot.velocity.y = -getSpeed(power) * sin(angle * PI / 180.0);

    temp_Shot.shooter_ID = ID;
    number_of_missile = getNumberMissiles(power);
    for (int i = 0; i < number_of_missile; i++)
    {
        Shots.push_back(temp_Shot);
        if (i % 2)
        {
            temp_left -= 40;
            temp_Shot.start.x = temp_left;
        }
        else
        {
            temp_right += 40;
            temp_Shot.start.x = temp_right;
        }

    }

}

void AirStrike::clearShots(void)
{
    Shots.clear();
}

bool AirStrike::checkExplosion(void)
{
    return false;
}

