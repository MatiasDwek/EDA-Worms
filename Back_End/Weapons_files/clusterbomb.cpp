#include "clusterbomb.h"
#include "../Communication_files/server-client.h"
#include <iostream>

ClusterBomb::ClusterBomb(const char* file_path, const char* file_path_worm)
{
    image = al_load_bitmap(file_path);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'v';
    type = FUSE;
    past_refresh = 0;
    explosion_sound = al_load_sample("./Sounds/explosion1.wav");
    fragments = al_load_bitmap("./Images/Weapons/clusterbomb/clustlet.png");
    number.x = 2;
    number.y = 0;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading Cluster Bomb image." << endl;
}

int ClusterBomb::getDamage(int power)
{
    return 35;
}

int ClusterBomb::getDiameter(int power)
{
    return 30;
}

float ClusterBomb::getSpeed(int power)
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
    return 15.0 * percentage_modifier;
}

float ClusterBomb::getExplosionBias()
{
    return 0;
}

int ClusterBomb::number_of_fragments(int power)
{
    return 5;
}

bool ClusterBomb::readyToShoot(void)
{
    //al_play_sample(release_sound, 1.0, 0.0, 1.0,ALLEGRO_PLAYMODE_ONCE, &sample_id);
    if (Shots.size() > 0)
        return true;
    else
        return false;
}
bool ClusterBomb::Shoot(Cmap* map, Cworm* worm)
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
        Point start_for_fragments;
        start_for_fragments.x = i->position.x;
        start_for_fragments.y = i->position.y;
        Shots.erase(i);
        i--;

        int angle = -45;
        int fragments = number_of_fragments(_power);

        for (int ii = 0; ii < fragments; ii++, angle -= 15)
            createShotW(angle, _power, start_for_fragments, 0, -1);
        Shoot_fragments(map, worm);
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

bool ClusterBomb::Shoot_fragments(Cmap* map, Cworm* worm)
{
    float t = 0;
    float j;
    float temp_x;
    float temp_y;
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
                        break;
                    }
                }

                if ((worm->orientation == RIGHT) || ((i->velocity.x < FLOAT_ERROR) && (i->velocity.x > -FLOAT_ERROR)))
                    al_draw_rotated_bitmap(fragments,
                                           //i->position.x - WORM_CENTER_FIX,
                                           //i->position.y - WORM_CENTER_FIX,
                                           al_get_bitmap_width(fragments) / 2.0,
                                           al_get_bitmap_height(fragments) / 2.0,
                                           i->position.x,
                                           i->position.y,
                                           PI - atan(-(i->velocity.y + GRAVITY * j)/(i->velocity.x + wind * j)),
                                           0);
                else
                    al_draw_rotated_bitmap(fragments,
                       //i->position.x - WORM_CENTER_FIX,
                       //i->position.y - WORM_CENTER_FIX,
                       al_get_bitmap_width(fragments) / 2.0,
                       al_get_bitmap_height(fragments) / 2.0,
                       i->position.x,
                       i->position.y,
                       2 * PI - atan(-(i->velocity.y + GRAVITY * j)/(i->velocity.x + wind * j)),
                       0);

                //al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 0);
            }
            else
            {
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

void ClusterBomb::createShotW(int angle, int power, Point start, int fuse, int ID)
{
    _power = power;

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
    Shots.push_back(temp_Shot);

}

void ClusterBomb::clearShots(void)
{
    al_destroy_timer(time_to_explosion);
    al_destroy_timer(iteration);
    number_of_shots = 1;
    past_refresh = 0;
    Shots.clear();
}

bool ClusterBomb::checkExplosion(void)
{
    return false;
}

