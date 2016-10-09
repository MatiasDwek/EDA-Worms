#include "uzi.h"
#include "../Map_files/map.h"
#include <iostream>

Uzi::Uzi(const char* file_path, const char* file_path_worm)
{
    image = al_load_bitmap(file_path);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'u';
    type = UZI;
    explosion_sound = al_load_sample("./Sounds/uzifire.wav");
    worm_holding_uzi = al_load_bitmap("./Images/Weapons/uzi/wuzif.png");
    number.x = 5;
    number.y = 0;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading Uzi image." << endl;
}

int Uzi::getDamage(int power)
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
        return 1;
    }
}

int Uzi::getDiameter(int power)
{
    return 11;
}

float Uzi::getSpeed(int power)
{
    return 0;
}

float Uzi::getExplosionBias()
{
    return 0;
}

bool Uzi::readyToShoot(void)
{
    if (Shots.size() > 0)
        return true;
    else
        return false;
}

bool Uzi::Shoot(Cmap* map, Cworm* worm)
{
    number_of_shots--;

    float t = 0;
    float j;
    float temp_x;
    float temp_y;
    bool finish;
    vector<Cworm*> worms_hit;
    vector<Cworm*> temp_worms_hit;
    vector<Shot>::iterator i;
    float wind = getWindStrength(map->WindStrength);
    al_play_sample(explosion_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &sample_id);

        for (int ii = 0; ii < 10; ii++)
        {
            finish = false;
            do
            {
                for (i = Shots.begin(); i != Shots.end(); i++)
                {
                    i->position.x = i->start.x
                                    + i->velocity.x * t;
                    i->position.y = i->start.y
                                    + i->velocity.y * t;

                    if (isInsideMap(i->position.x, i->position.y, map))
                    {
                        for (j = t; j < (t + 1); j = j + 0.5)
                        {
                            temp_x = i->start.x
                                    + i->velocity.x * j;
                            temp_y = i->start.y
                                    + i->velocity.y * j;

                            //al_draw_filled_circle(temp_x, temp_y, 2, al_map_rgb(0, 0, 0));
                            //al_flip_display();

                            if (impact(temp_x, temp_y, map) || impactWorms(temp_x, temp_y, i->shooter_ID, map))
                            {
                                drawShot(*i, temp_x, temp_y, i->velocity.x, i->velocity.y, map, worm);

                                temp_worms_hit = checkWormsHit(*i, temp_x,
                                                               temp_y,
                                                               i->velocity.x + wind * j,
                                                               i->velocity.y + GRAVITY * j,
                                                               map->myWorms,
                                                               map->enemyWorms);
                                worms_hit.insert(worms_hit.end(), temp_worms_hit.begin(), temp_worms_hit.end());

                                finish = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        finish = true;
                        break;
                    }
                    t += 4;
                }
            } while (!finish);
            moveWormsHit(worms_hit, map);
            map->showWorms();
            al_draw_bitmap(worm_holding_uzi, worm->pos.x, worm->pos.y, worm->orientation == RIGHT ? ALLEGRO_FLIP_HORIZONTAL : 0);
            al_flip_display();
            al_rest(FRAMERATE * 6);
        }
        al_stop_sample(&sample_id);
        return true;
}

void Uzi::createShotW(int angle, int power, Point start, int fuse, int ID)
{
    Shot temp_Shot;
    temp_Shot.angle = angle;
    temp_Shot.power = power;
    //temp_Shot.type = type;
    temp_Shot.start.x = start.x + WORM_CENTER_FIX;
    temp_Shot.start.y = start.y + WORM_CENTER_FIX;
    //temp_Shot.destination = destination;
    temp_Shot.diameter = getDiameter(power);
    temp_Shot.explosion_bias = getExplosionBias();
    temp_Shot.damage = getDamage(power);

    temp_Shot.velocity.x = cos(angle * PI / 180.0);
    temp_Shot.velocity.y = -sin(angle * PI / 180.0);

    temp_Shot.shooter_ID = ID;
    Shots.push_back(temp_Shot);
}

void Uzi::clearShots(void)
{
    number_of_shots = 1;
    Shots.clear();
}

bool Uzi::checkExplosion(void)
{
    return false;
}


