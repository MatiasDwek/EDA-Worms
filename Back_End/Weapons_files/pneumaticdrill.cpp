#include "pneumaticdrill.h"
#include "../Map_files/map.h"
#include "../Defines/point.h"
#include "../Communication_files/server-client.h"
#include <iostream>


PneumaticDrill::PneumaticDrill(const char* projectilePath, const char* file_path_worm)
{
    image = al_load_bitmap(projectilePath);
    worm_with_gun = al_load_bitmap(file_path_worm);
    number_of_shots = 1;
    weapon = 'p';
    type = PD;
    explosion_sound = al_load_sample("./Sounds/drill.wav");
    number.x = 3;
    number.y = 1;
    button_start.x = 413 + 75 * number.x;
    button_start.y = 711 + 75 * number.y;
    button_end.x = 413 + 75 * (number.x + 1);
    button_end.y = 711 + 75 * (number.y + 1);
    if (image == NULL)
        cout << "Error loading pneumatic drill image." << endl;
}

int PneumaticDrill::getDamage(int power)
{
    return 10;
}
int PneumaticDrill::getDiameter(int power)
{
    return 40;
}
float PneumaticDrill::getSpeed(int power)
{
    return 4.5;
}
float PneumaticDrill::getExplosionBias(void)
{
    return 0;
}
bool PneumaticDrill::readyToShoot(void)
{
    if (Shots.size() == 1)
        return true;
    else
        return false;
}

bool PneumaticDrill::Shoot(Cmap* map, Cworm* worm)
{
    int t = 0;
    float j;
    float temp_x;
    float temp_y;
    vector<Cworm*> worms_hit;
    vector<Cworm*> temp_worms_hit;
    vector <Cworm*> shooterFalling;
    bool shooterStartFalling = false;
    vector<Shot>::iterator i;



    al_play_sample(worm->pCweapon->explosion_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &sample_id);

    worms_hit.push_back(worm);

    do
    {
    if (shooterFalling.empty())
        worm->showAllWormsButMyself();
    else
        map->showWorms();
    for (i = Shots.begin(); i != Shots.end(); i++)
    {
        i->position.x = i->start.x + i->velocity.x*t;
        i->position.y = i->start.y + i->velocity.y*t;

        if (isInsideMap(i->position.x, i->position.y, map))
        {
            for (j = t; j < (t + 1); j = j + 0.1)  //avanzo de apoquito
            {
                temp_x = i->start.x + i->velocity.x * j;
                temp_y = i->start.y + i->velocity.y * j; //no lo afecta la gravedad GRAVITY * j * (j + 1) / 2;

                temp_worms_hit = checkWormsHit(*i, temp_x, temp_y,i->velocity.x,i->velocity.y + GRAVITY * j
                                                 ,map->myWorms, map->enemyWorms, worms_hit);
                worms_hit.insert(worms_hit.end(), temp_worms_hit.begin(), temp_worms_hit.end());
            }
            if (t%5==1 && !Shots.empty()) //drawShot consume mucho tiempo. X eso solo la llamo una de cada 4veces
                drawShot(*i, temp_x, temp_y, i->velocity.x, i->velocity.y + GRAVITY * j, map, worm);
                if (shooterFalling.empty())
            worm->showAllWormsButMyself();
            al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX + random_btwn(-3, 3), i->position.y - WORM_CENTER_FIX + random_btwn(-3, 3), worm->orientation == RIGHT ? ALLEGRO_FLIP_HORIZONTAL : 0);

            map->displayWater(); // tapo al worm que tiene el drill con AGUA y BOTTOM
            //al_draw_bitmap(map->bottom,0, al_get_bitmap_height(map->mapa),0);


            //al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 0);
        }
        else
        {
            Shots.erase(i);
            i--;
            break;
        }
    }
    t += 1;
    if (t>=PD_FRAMES && !shooterStartFalling) //si ya termino el  PD
        if (!(impact(worm->pos.x + WORM_CENTER_FIX - WORM_HALF_WIDTH, worm->pos.y + WORM_CENTER_FIX + WORM_HALF_HEIGHT, map) //izq
            || impact(worm->pos.x + WORM_CENTER_FIX + WORM_HALF_WIDTH, worm->pos.y+ WORM_CENTER_FIX + WORM_HALF_HEIGHT, map))) //der)
        {       //si el shooter luego de efectuar el pneumatic drill, queda en el aire, debe caer
            if (!Shots.empty())
            {
                vector<Cworm>::iterator i;
                for (i=map->myWorms.begin(); i!=map->myWorms.end(); i++)
                    if (Shots[0].shooter_ID == i->ID)
                        shooterFalling.push_back(&(*i));
                for (i=map->enemyWorms.begin(); i!=map->enemyWorms.end(); i++)
                    if (Shots[0].shooter_ID == i->ID)
                        shooterFalling.push_back(&(*i));
            }

            //shooter vacio!!! ->
            if (!shooterFalling.empty())
            {
                shooterFalling[0]->velocity.x =0; shooterFalling[0]->velocity.y =0; //caida libre
                shooterFalling[0]->impact_time = 0;
                shooterFalling[0]->impact_point.x = Shots[0].position.x- WORM_CENTER_FIX;
                shooterFalling[0]->impact_point.y = Shots[0].position.y- WORM_CENTER_FIX;
                shooterFalling[0]->state = AIR;
                shooterStartFalling = true;
            }



        }
    //moveWormsHit(shooterFalling, map);

    al_flip_display();
    al_rest(FRAMERATE);
    } while( (t<PD_FRAMES) );
    do
    {
        map->showWorms();
        al_draw_bitmap(worm->pCweapon->image, i->position.x - WORM_CENTER_FIX, i->position.y - WORM_CENTER_FIX, 0);
        moveWormsHit(worms_hit, map);//Punch
        moveWormsHit(shooterFalling, map);
        al_flip_display();
        al_rest(FRAMERATE);
    }while (!worms_hit.empty());
    map->showWorms();

    al_stop_sample(&sample_id);
    return true;
}
Velocity PneumaticDrill::getVelocity(Point posWvictim, Point posWattacker, int power)
{
    Velocity vel = {0,0};
    return vel;
}

















void PneumaticDrill::createShotW(int angle, int power, Point pos, int fuse, int ID)
{
    Shot shot;
    shot.angle = -90;
    shot.damage = getDamage(power); // el daño va cambiando segun a cuantos worms les pegue
    shot.diameter = getDiameter(power);
    shot.explosion_bias = getExplosionBias();
    shot.fuse = 0;
    shot.power = power;
    shot.shooter_ID = ID;
    shot.velocity.y = PD_SPEED;
    shot.velocity.x = 0;
    shot.start.x = pos.x + WORM_CENTER_FIX;
    shot.start.y = pos.y + WORM_CENTER_FIX;
    Shots.push_back(shot);
}

vector<Cworm*> PneumaticDrill::checkWormsHit(Shot shot, float x, float y, float vx, float vy, vector<Cworm>& myWorms,
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
                        i->velocity = getVelocity(i->pos,st, shot.power);
                        i->health -= shot.damage;
                        if (i->health < 0)
                            i->health = 0;
                        i->impact_time = 0;
                        i->impact_point = i->pos;
                        i->state = AIR;
                        hit_worms.push_back(&(*i));
                    }
//                if (prevHit.size()==0)
//                {
//                    Point st = shot.start; st.x -= WORM_CENTER_FIX; st.y -= WORM_CENTER_FIX;
//                    i->velocity = getVelocity(i->pos,st, shot.power);
//                    i->health -= shot.damage;
//                    if (i->health < 0)
//                        i->health = 0;
//                    i->impact_time = 0;
//                    i->impact_point = i->pos;
//                    i->state = AIR;
//                    hit_worms.push_back(&(*i));
//                }
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
                        i->velocity = getVelocity(i->pos,st, shot.power);
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
                    i->velocity = getVelocity(i->pos,st, shot.power);
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

bool PneumaticDrill::checkExplosion(void)
{
    return false;
}

void PneumaticDrill::clearShots(void)
{
    number_of_shots = 1;
    Shots.clear();
}
