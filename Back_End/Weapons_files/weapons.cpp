#include <iostream>
#include <cmath>
#include <vector>
#include "weapons.h"
#include "../Map_files/map.h"
#include "../Communication_files/server-client.h"

using namespace std;

Cweapon::~Cweapon()
{
    al_destroy_bitmap(image);
    al_destroy_bitmap(worm_with_gun);
}

bool shoot(vector<Shot>& Shots, Cmap* map, Cworm* worm)
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
                    al_draw_rotated_bitmap(worm->pCweapon->image,
                                           //i->position.x - WORM_CENTER_FIX,
                                           //i->position.y - WORM_CENTER_FIX,
                                           al_get_bitmap_width(worm->pCweapon->image) / 2.0,
                                           al_get_bitmap_height(worm->pCweapon->image) / 2.0,
                                           i->position.x,
                                           i->position.y,
                                           PI - atan(-(i->velocity.y + GRAVITY * j)/(i->velocity.x + wind * j)),
                                           0);
                else
                    al_draw_rotated_bitmap(worm->pCweapon->image,
                       //i->position.x - WORM_CENTER_FIX,
                       //i->position.y - WORM_CENTER_FIX,
                       al_get_bitmap_width(worm->pCweapon->image) / 2.0,
                       al_get_bitmap_height(worm->pCweapon->image) / 2.0,
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

Shot createShot(int angle, int power, int type, Point start, Point destination, float speed, int64_t fuse, int diameter, float explosion_bias, int damage, int shooter_ID)
{
    Shot temp_Shot;
    temp_Shot.angle = angle;
    temp_Shot.power = power;
    temp_Shot.type = type;
    temp_Shot.start.x = start.x + WORM_CENTER_FIX;
    temp_Shot.start.y = start.y + WORM_CENTER_FIX;
    temp_Shot.destination = destination;
    temp_Shot.fuse = fuse;
    temp_Shot.diameter = diameter;
    temp_Shot.explosion_bias = explosion_bias;
    temp_Shot.damage = damage;

    temp_Shot.velocity.x = speed * cos(angle * PI / 180.0);
    temp_Shot.velocity.y = -speed * sin(angle * PI / 180.0);

    temp_Shot.shooter_ID = shooter_ID;

    return temp_Shot;
}

bool isInsideMap(int x, int y, Cmap* map)
{
    bool c1 = (((0 - WEAPON_GRAPHICS_TOLERANCE) < x) && (x < (WIDTH + WEAPON_GRAPHICS_TOLERANCE)));
    bool c2 = (y < (HEIGHT - al_get_bitmap_height(map->bottom))); //((0 - WEAPON_GRAPHICS_TOLERANCE) < y) &&

    //printf("c11 = %d\n", (0 - WEAPON_GRAPHICS_TOLERANCE) < x);
    //printf("c12 = %d\n", x < (WIDTH + WEAPON_GRAPHICS_TOLERANCE));

    return (c1 && c2);
}

bool impact(int x, int y, Cmap* map) //impacto con el Scenario? Rta: true o false.
{
    unsigned char r, g, b, a;
    al_unmap_rgba(al_get_pixel(map->mapa, x, y), &r, &g, &b, &a);
    if (a != TRANSPARENT_)
        return true;
    else
        return false;
}

bool impactWorms(int x, int y, int shooter_ID, Cmap* map)
{
    vector<Cworm>::iterator i;

    for (i = map->myWorms.begin(); i != map->myWorms.end(); i++)
    {
        if ((i->realPosX() < x) && (x < i->realPosX() + WORM_WIDTH)
            && (i->realPosY() < y) && (y < i->realPosY() + WORM_HEIGHT)
            && i->ID != shooter_ID)
            return true;
    }
    for (i = map->enemyWorms.begin(); i != map->enemyWorms.end(); i++)
    {
        if ((i->realPosX() < x) && (x < i->realPosX() + WORM_WIDTH)
            && (i->realPosY() < y) && (y < i->realPosY() + WORM_HEIGHT)
            && i->ID != shooter_ID)
            return true;
    }
    return false;
}

float getWindStrength(char charWindStrength)
{
    float floatWindStrength;
    switch (charWindStrength)
    {
    case 0:
        floatWindStrength = 0;
        break;
    case 'A':
    case 'a':
        floatWindStrength = 0.119 * GRAVITY;
        break;
    case 'B':
    case 'b':
        floatWindStrength = 0.238 * GRAVITY;
        break;
    case 'C':
    case 'c':
        floatWindStrength = 0.357 * GRAVITY;
        break;
    case 'D':
    case 'd':
        floatWindStrength = 0.476 * GRAVITY;
        break;
    case 'E':
    case 'e':
        floatWindStrength = 0.595 * GRAVITY;
        break;
    case 'F':
    case 'f':
        floatWindStrength = 0.714 * GRAVITY;
        break;
    case 'G':
    case 'g':
        floatWindStrength = 0.833 * GRAVITY;
        break;
    case 'H':
    case 'h':
        floatWindStrength = 0.952 * GRAVITY;
        break;
    case 'I':
    case 'i':
        floatWindStrength = 1.071 * GRAVITY;
        break;
    case 'J':
    case 'j':
        floatWindStrength = 1.190 * GRAVITY;
        break;
    }
    if (charWindStrength >= ASCII_LOW_LET_FIX)
        floatWindStrength = -floatWindStrength;
    return floatWindStrength;
}

void drawShot(Shot shot, float x, float y, float vx, float vy, Cmap* map, Cworm* worm) //dibuja el impacto sobre el Scenario
{
    if (worm->pCweapon->weapon == 'b')
        al_stop_sample(&worm->pCweapon->sample_id);
    if ((worm->pCweapon->type == NORMAL) || (worm->pCweapon->type == FUSE))
        al_play_sample(worm->pCweapon->explosion_sound, 1.0, 0.0, 1.0,ALLEGRO_PLAYMODE_ONCE,NULL);

    al_unlock_bitmap(map->mapa);
    al_set_target_bitmap(map->mapa);

    //al_set_blender(ALLEGRO_DEST_MINUS_SRC, ALLEGRO_ZERO, ALLEGRO_ZERO);
    //al_draw_circle(x + vx * shot.explosion_bias, y + vy * shot.explosion_bias, shot.diameter / 2.0, al_map_rgb(0, 0, 0), 5);

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_draw_filled_ellipse(x + vx * shot.explosion_bias, y + vy * shot.explosion_bias, shot.diameter / 2.0, shot.diameter / 2.0, al_map_rgba(0, 0, 0, 0));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);


    al_set_target_bitmap(al_get_backbuffer(map->display));
    al_lock_bitmap(map->mapa, al_get_bitmap_format(map->mapa), ALLEGRO_LOCK_READWRITE);
}

vector<Cworm*> checkWormsHit(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms, vector<Cworm>& enemyWorms)
{
   Point impact_point = {(int) x, (int) y};
    Point worm_real_pos;
    vector<Cworm*> hit_worms;
    vector<Cworm>::iterator i;

    for (i = myWorms.begin(); i != myWorms.end(); i++)
    {
         worm_real_pos.x = i->pos.x + WORM_CENTER_FIX;
         worm_real_pos.y = i->pos.y + WORM_CENTER_FIX;

//         al_draw_filled_circle(worm_real_pos.x, worm_real_pos.y, 3, al_map_rgb(255, 255, 255));
//         al_draw_filled_circle(impact_point.x, impact_point.y, 3, al_map_rgb(0, 255, 0));
//         al_flip_display();

        if (getDistanceBetweenPoints(worm_real_pos, impact_point) <= shot.diameter / 2.0)
        {
            i->health -= shot.damage;
            if (i->health < 0)
                i->health = 0;
            i->velocity = getVelocity(impact_point, worm_real_pos, shot.diameter);
            i->impact_time = 0;
            i->impact_point = i->pos;
            i->state = AIR;
            hit_worms.push_back(&(*i));
        }
    }
    for (i = enemyWorms.begin(); i != enemyWorms.end(); i++)
    {
        worm_real_pos.x = i->pos.x + WORM_CENTER_FIX;
        worm_real_pos.y = i->pos.y + WORM_CENTER_FIX;

        if (getDistanceBetweenPoints(worm_real_pos, impact_point) <= shot.diameter / 2.0)
        {
            i->health -= shot.damage;
            if (i->health < 0)
                i->health = 0;
            i->velocity = getVelocity(impact_point, worm_real_pos, shot.diameter);
            i->impact_time = 0;
            i->impact_point = i->pos;
            i->state = AIR;
            hit_worms.push_back(&(*i));
        }
    }

    return hit_worms;
}

bool wormsMoving()
{
    return true;
}

void moveWormsHit(vector<Cworm*>& worms_hit, Cmap* map)
{
    float j;
    float temp_x, temp_y;
    bool flag;
    vector<Cworm*>::iterator i;
    bool cUp, cDown;

    for (i = worms_hit.begin(); i != worms_hit.end(); i++)
    {
        flag = true;

        (*i)->pos.x = (*i)->impact_point.x + (*i)->velocity.x * (*i)->impact_time;
        (*i)->pos.y = (*i)->impact_point.y + (*i)->velocity.y * (*i)->impact_time + GRAVITY * (*i)->impact_time * ((*i)->impact_time + 1) / 2.0;

        for (j = (*i)->impact_time; j < ((*i)->impact_time + 1); j = j + 0.10)
        {
            temp_x = (*i)->impact_point.x + (*i)->velocity.x * j;
            temp_y = (*i)->impact_point.y + (*i)->velocity.y * j + GRAVITY * j * (j + 1) / 2.0;

            cUp =   impact(temp_x + WORM_CENTER_FIX - WORM_HALF_WIDTH, temp_y + WORM_CENTER_FIX - WORM_HALF_HEIGHT, map) //izq
                    || impact(temp_x + WORM_CENTER_FIX + WORM_HALF_WIDTH, temp_y + WORM_CENTER_FIX - WORM_HALF_HEIGHT, map); //der
            cUp &= ((*i)->velocity.y +GRAVITY*j)<0; //si va para arriba y se choca arriba TRUE
            cDown =   impact(temp_x + WORM_CENTER_FIX - WORM_HALF_WIDTH, temp_y + WORM_CENTER_FIX + WORM_HALF_HEIGHT, map) //izq
                    || impact(temp_x + WORM_CENTER_FIX + WORM_HALF_WIDTH, temp_y + WORM_CENTER_FIX + WORM_HALF_HEIGHT, map); //der
            cDown &= ((*i)->velocity.y +GRAVITY*j)>=0; //si va para abajo y se choca abajo TRUE
            if (cUp || cDown || !isInsideMap(temp_x + WORM_CENTER_FIX, temp_y + WORM_CENTER_FIX, map))
            {   //condiciones para dejar de graficar al worm en el aire
                (*i)->pos.x = temp_x;
                (*i)->pos.y = temp_y;

                (*i)->health -= (*i)->calculateFallDamage((*i)->velocity.y + GRAVITY * j);

                (*i)->state = GROUND;
                worms_hit.erase(i);
                i--;
                flag = false;
                break;
            }

        }

        if (flag)
            (*i)->impact_time++;
        //al_flip_display();
    }
}

Velocity getVelocity(Point impact_point, Point worm_position, int diameter)
{
    Velocity velocity = {diameter * cos(getAngleBetweenPoints( worm_position,impact_point)) / 10.0,
                         diameter * sin(getAngleBetweenPoints( worm_position,impact_point)) / 10.0};
    return velocity;
}


Bazooka::Bazooka(const char* file_path, const char* file_path_worm)
{
    image = al_load_bitmap(file_path);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'b';
    type = NORMAL;
    explosion_sound = al_load_sample("./Sounds/explosion1.wav");
    release_sound = al_load_sample("./Sounds/rocketrelease.wav");
    number.x = 0;
    number.y = 0;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading bazooka image." << endl;
}

int Bazooka::getDamage(int power)
{
    return (power*5 + 35);
}

int Bazooka::getDiameter(int power)
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

float Bazooka::getSpeed(int power)
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

float Bazooka::getExplosionBias()
{
    return 0;
}

bool Bazooka::readyToShoot(void)
{
    if (Shots.size() == 1)
        return true;
    else
        return false;
}

bool Bazooka::Shoot(Cmap* map, Cworm* worm)
{
    al_play_sample(release_sound, 1.0, 0.0, 1.0,ALLEGRO_PLAYMODE_ONCE, &sample_id);
    shoot(Shots, map, worm);
    return true;
}

void Bazooka::createShotW(int angle, int power, Point start, int fuse, int ID)
{
    Shot temp_Shot;
    temp_Shot.angle = angle;
    temp_Shot.power = power;
    //temp_Shot.type = type;
    temp_Shot.start.x = start.x + WORM_CENTER_FIX;
    temp_Shot.start.y = start.y + WORM_CENTER_FIX;
    //temp_Shot.destination = destination;
    temp_Shot.fuse = fuse;
    temp_Shot.diameter = getDiameter(power);
    temp_Shot.explosion_bias = getExplosionBias();
    temp_Shot.damage = getDamage(power);

    temp_Shot.velocity.x = getSpeed(power) * cos(angle * PI / 180.0);
    temp_Shot.velocity.y = -getSpeed(power) * sin(angle * PI / 180.0);

    temp_Shot.shooter_ID = ID;
    Shots.push_back(temp_Shot); //un solo disparo para la Bazooka
}

void Bazooka::clearShots(void)
{
    Shots.clear();
}

bool Bazooka::checkExplosion(void)
{
    return false;
}
