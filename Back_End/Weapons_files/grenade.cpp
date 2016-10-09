#include "grenade.h"
#include "../Map_files/map.h"
#include <iostream>

Grenade::Grenade(const char* file_path, const char* file_path_worm)
{
    image = al_load_bitmap(file_path);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'g';
    type = FUSE;
    past_refresh = 0;
    explosion_sound = al_load_sample("./Sounds/explosion3.wav");
    number.x = 1;
    number.y = 0;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading Grenade image." << endl;
}

int Grenade::getDamage(int power)
{
    return 35 + power * 5;
}

int Grenade::getDiameter(int power)
{
    switch (power)
    {
    case 1:
    case 2:
        return 85;
        break;
    case 3:
        return 97;
        break;
    case 4:
        return 111;
        break;
    case 5:
        return 123;
        break;
    default:
        return 1;
    }
}

float Grenade::getSpeed(int power)
{
    float percentage_modifier;
    switch (power)
    {
    case 1:
        percentage_modifier = 0.8;
        break;
    case 2:
        percentage_modifier = 0.9;
        break;
    case 3:
        percentage_modifier = 1.0;
        break;
    case 4:
        percentage_modifier = 1.1;
        break;
    case 5:
        percentage_modifier = 1.2;
        break;
    }
    return 24.12 * percentage_modifier;
}

float Grenade::getExplosionBias()
{
    return 0;
}

bool Grenade::readyToShoot(void)
{
    if (Shots.size() > 0)
        return true;
    else
        return false;
}


bool Grenade::Shoot(Cmap* map, Cworm* worm)
{
    al_start_timer(iteration);
    char buffer_for_time[5];
    char buffer_for_time2[3];
    memset(buffer_for_time, 0, 5);
    memset(buffer_for_time2, 0, 3);
    float t;
    float j;
    float temp_x;
    float temp_y;
    vector<Shot>::iterator i = Shots.begin();
    float wind = getWindStrength(map->WindStrength);
    number_of_shots = 0;
    t = al_get_timer_count(iteration);


    if ((al_get_timer_count(time_to_explosion) >= (i->fuse * 10)) && (!Shots.empty()))
    {
        vector<Cworm*> worms_hit;
        drawShot(*i, i->position.x, i->position.y, i->velocity.x + wind * t, i->velocity.y + GRAVITY * t, map, worm);
        al_flip_display();
        worms_hit = checkWormsHit(*i, i->position.x,
                                  i->position.y,
                                  i->velocity.x + wind * t,
                                  i->velocity.y + GRAVITY * t,
                                  map->myWorms,
                                  map->enemyWorms);
        while (!worms_hit.empty())
        {
            map->showWorms();
            al_rest(FRAMERATE);
            moveWormsHit(worms_hit, map);
        }
        Shots.erase(i);
        i--;
        return true;
    }
    else if (al_get_timer_count(iteration) > past_refresh)
    {
        past_refresh = al_get_timer_count(iteration);
        if (!(impact(i->position.x, i->position.y, map) || impactWorms(i->position.x, i->position.y, i->shooter_ID, map)))
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
                            i->position.x = temp_x;
                            i->position.y = temp_y;
                            break;
                        }
                    }
                }
                else
                {
                    Shots.erase(i);
                    i--;
                    return true;
                }
        }
        buffer_for_time2[0] = '.';
        itoa(i->fuse * 10 - al_get_timer_count(time_to_explosion) - (int) ((i->fuse * 10 - al_get_timer_count(time_to_explosion)) / 10) * 10, buffer_for_time2 + 1, 10);
        strcat(itoa((int) ((i->fuse * 10 - al_get_timer_count(time_to_explosion)) / 10), buffer_for_time, 10), buffer_for_time2);
        al_draw_text(map->font_for_fuse, al_map_rgb(255, 255, 0), i->position.x + 15, i->position.y - 20, 0, buffer_for_time);
        al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 0);
        //al_draw_filled_circle(i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 5, al_map_rgb(0, 0, 0));
        //al_draw_filled_circle(i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 2, al_map_rgb(255, 255, 255));
        //al_flip_display();
    }
    return true;
}

void Grenade::createShotW(int angle, int power, Point start, int fuse, int ID)
{
    Shot temp_Shot;
    temp_Shot.angle = angle;
    temp_Shot.power = power;
    //temp_Shot.type = type;
    temp_Shot.start.x = start.x + WORM_CENTER_FIX;
    temp_Shot.start.y = start.y + WORM_CENTER_FIX;
    temp_Shot.fuse = fuse;
    time_to_explosion = al_create_timer(0.1);
    al_start_timer(time_to_explosion);
    iteration = al_create_timer(FRAMERATE);
    //temp_Shot.destination = destination;
    temp_Shot.diameter = getDiameter(power);
    temp_Shot.explosion_bias = getExplosionBias();
    temp_Shot.damage = getDamage(power);

    temp_Shot.velocity.x = getSpeed(power) * cos(angle * PI / 180.0);
    temp_Shot.velocity.y = -getSpeed(power) * sin(angle * PI / 180.0);

    temp_Shot.shooter_ID = ID;
    Shots.push_back(temp_Shot); //un solo disparo para la Bazooka
}

void Grenade::clearShots(void)
{
    al_destroy_timer(time_to_explosion);
    al_destroy_timer(iteration);
    number_of_shots = 1;
    past_refresh = 0;
    Shots.clear();
}

bool Grenade::checkExplosion(void)
{
    return false;
}
