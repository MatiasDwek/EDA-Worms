#include "worms.h"
#include "../../Front_End/runGame.h"
#include <iostream>
#include "../Map_files/map.h"

Cworm::Cworm(ALLEGRO_DISPLAY *disp,/*armaPorDefecto,*/ Point pos_, Orientation ori,ALLEGRO_BITMAP *mapa_,
             ALLEGRO_BITMAP* background_, Gif* pGif_, int wormID, Cmap* pMap, Bazooka* _Bazooka)
{
    display = disp;
    pos = pos_;
    pos.x -= (W_WIDTH_PNG-W_WIDTH)/2+W_WIDTH; //ajuste x el .map tiene distinta referencia
    pos.x += 11; //ajuste a mano x diferencias
    pos.y -= (W_HEIGHT_PNG-W_HEIGHT)/2+W_HEIGHT;
    orientation = ori;
    mapa = mapa_;
    background = background_;
    health = START_VIDA;
    pCweapon = _Bazooka;
//    for (unsigned i=0; i<WWALK_FRAMES; i++)
//    {
//        wwalkL[i] = wL[i]; //copio los frames de caminar a la izq
//        wwalkR[i] = wR[i];
//    }
    pGif = pGif_;
    ID = wormID;
    pCmap =  pMap;
    //cout << pCmap;
    char asd[4];
    itoa(ID, asd, 10);
    name = "W";
    //pCweapon = NULL;
    name = name+asd;
    state = GROUND;
}

Cworm::Cworm(const Cworm &worm)
{
    this->display = worm.display;
    this->pos = worm.pos;
    this->orientation = worm.orientation;
    this->mapa = worm.mapa;
    this->background = worm.background;
    this->pGif = worm.pGif;
    this->health = worm.health;

    pCmap = worm.pCmap;
    health = worm.health;
    ID = worm.ID;
    color = worm.color;
    name = worm.name;
    lastAction = worm.lastAction;
    pCweapon = worm.pCweapon;

    velocity.x = 0;
    velocity.y = 0;
    state = worm.state;
}

void Cworm::walk(void)
{
    /*
    Esta funcion hace que el worm camine 9 pixeles, a razon de 26px/seg. Camina  ala izq o der
    segun su orientation.
    */
    lastAction = WALK;
    al_set_target_bitmap(al_get_backbuffer(display)); //me aseguro que escriba sobre el display
    if (!canWalk())
    {
        showWwalkFrames();
        return;
    }

    for (unsigned j=0; j<WWALK_FRAMES; j++)
    {
        pos.y += getPhaseY(j); //se hace adelante porque así se verá que el worm trepa x cada frame
        updateWormFrameMoving(j);
        al_flip_display();
//        if (SHIFT_NUMBERS)
        al_rest(FRAMERATE);
    }
    if (orientation == LEFT)
        pos.x -= getPhaseX();
    else if (orientation == RIGHT)
        pos.x += getPhaseX();

    if (noEstaChocandoseVert(W_HEIGHT))
    {
        //shotWorm(0.0,0); //caida libre
        nextAction = CAIDA_LIBRE;
    }

}
Point Cworm::getWalkDestination(void)
{
    /*
    Esta funcion hace que el worm camine 9 pixeles, a razon de 26px/seg. Camina  ala izq o der
    segun su orientation.
    */
    lastAction = WALK;
    //al_set_target_bitmap(al_get_backbuffer(display)); //me aseguro que escriba sobre el display
    const Point bckup = pos;
    Point posRet;
    if (!canWalk())
    {
        //showWwalkFrames();
        return pos;
    }

    for (unsigned j=0; j<WWALK_FRAMES; j++)
    {
        pos.y += getPhaseY(j); //se hace adelante porque así se verá que el worm trepa x cada frame
        //updateWormFrame(j);
        //al_flip_display();
//        if (SHIFT_NUMBERS)
        //al_rest(FRAMERATE);
    }
    if (orientation == LEFT)
        pos.x -= getPhaseX();
    else if (orientation == RIGHT)
        pos.x += getPhaseX();

    nextAction = DO_NOTHING;
    if (noEstaChocandoseVert(W_HEIGHT))
    {
        //shotWorm(0.0,0); //caida libre
        nextAction = CAIDA_LIBRE;
    }
    posRet = pos;
    pos = bckup;
    return posRet;
}

bool Cworm::canWalk()
{
    unsigned char r,g,b,a;
    int desfasaje =0;
    if (orientation == LEFT )
        desfasaje += 0;
    else if (orientation == RIGHT)
        desfasaje += W_WIDTH;

    int startX = realPosX();
    int startY = realPosY();
    //checkeo pos.x-desfazaje a lo largo de la altura del worm
    for(int f=0; f<=W_HEIGHT-WWALK_MAX_DELTA; f++) //sacandole WWALK_MAX_DELTA mejora considerablemente la velocidad
        for (int c=0; c<ENTORNO_DELTA; c++)
        {
//           al_draw_pixel(startX+desfasaje+c, startY+f, al_map_rgb(0,255,255));
            al_unmap_rgba(al_get_pixel(mapa, startX+desfasaje+c,startY+f),
                         &r,&g,&b,&a);
            if ( a!=TRANSPARENT_)
            {
                if (((int)W_HEIGHT-f)>=WWALK_MAX_DELTA)
                     return false;
            }
        }
//        al_flip_display();
//        al_rest(1.0);
    return true;
}
int Cworm::getPhaseX()
{
    if (isInsideMap())
        return WALKING_UNIT/W_AV;
    else
        return 0;
}
int Cworm::getPhaseY(int j)
{
    /*
    aparece j(iterator de wwalk[]) para que trepe entre frames de wwalk[], entonces se debe modificar
    el valor de inicio de x en x=x-j si va para la izq, sino x=x+j
    */
    if (!(SHIFT_NUMBERS))
        return 0;

    unsigned char r,g,b,a;
    int desfasaje =0;
    if (orientation == LEFT )
    {
        desfasaje += 5; j=j; //el 5 sale a ojo
    }
    else if (orientation == RIGHT)
    {
        desfasaje += W_WIDTH-5;  //falta ver si esto esta bien!! (lo mismo para canWalk()
        j = -j;
    }

    int startX = realPosX();
    int startY = realPosY()+W_HEIGHT-WWALK_MAX_DELTA; //arranco a mirar en la pos donde me quede en canWalk()
    //checkeo pos.x-desfazaje a lo largo de la altura del worm
    for(int f=0; f<WWALK_MAX_DELTA; f++) //sacandole WWALK_MAX_DELTA mejora considerablemente la velocidad
        for (int c=0; c<ENTORNO_DELTA; c++)
        {
//            al_draw_pixel(startX+desfasaje+c-j, startY+f, al_map_rgb(255,255,255));
            al_unmap_rgba(al_get_pixel(mapa, startX+desfasaje+c-j,startY+f), //aparece j(iterator de wwalk[]) que trepe entre frames de wwalk[]
                         &r,&g,&b,&a);
            if ( a!=TRANSPARENT_)
                return f-((int)WWALK_MAX_DELTA); //si va para arriba debe ser negativo
        }
//
//        al_flip_display();
//        al_rest(.5);
    return 0;
}
bool Cworm::isInsideMap()
{
    bool c1 = 0<pos.x && realPosX()<WIDTH-W_WIDTH;
    bool c2 = 0<pos.y && realPosY()<(HEIGHT-W_HEIGHT);
//    cout<<endl<<al_get_bitmap_height(background)<<endl;
//    cout<<al_get_bitmap_height(mapa)<<endl;
    return (c1&&c2);
}
int Cworm::realPosX()
{
    return pos.x+((int)(W_WIDTH_PNG-W_WIDTH))/2;
}
int Cworm::realPosY()
{
    return pos.y+((int)(W_HEIGHT_PNG-W_HEIGHT))/2;
}
bool Cworm::noEstaChocandoseVert(int shift)
{
    /*
    Se fija en un entorno de la parte inferior del worm si es que hay piso o no hay piso.
    El entorno es un rectangulo de W_WIDTH x ENTORNO_DELTA
    */
    //al_set_target_bitmap(al_get_backbuffer(display));
    shift--;
    unsigned char r,g,b,a;
    int startX = realPosX();
    int startY = realPosY()+shift; //parte inferior del worm
    for (int f=0; f<ENTORNO_DELTA; f++)
        for (int c=0; c<W_WIDTH; c++)
        {
            //al_draw_pixel(startX+c, startY+f, al_map_rgb(0,200,200)); ->para ver donde estoy leyendo
            al_unmap_rgba(al_get_pixel(mapa, startX+c,startY+f),&r,&g,&b,&a);
            if (a!=TRANSPARENT_)
                return false;
        }
    return true;
}
void Cworm::updateWormFrame(int j)
{
//    al_draw_bitmap(background, 0,0,0);
//    al_draw_bitmap(mapa, 0,0,0);
    if ((j != 0) || (pCweapon == NULL))
    {
        if (orientation == LEFT)
        {
            showAllWormsButMyself();
            al_draw_bitmap(pGif->wwalkL[j], pos.x, pos.y,0);
        }
        else if (orientation == RIGHT)
        {
            showAllWormsButMyself();
            al_draw_bitmap(pGif->wwalkR[j], pos.x, pos.y,0);
        }
    }
    else
    {
        showAllWormsButMyself();
        al_draw_bitmap(pCweapon->worm_with_gun, pos.x, pos.y, orientation == LEFT ? 0 : ALLEGRO_FLIP_HORIZONTAL);
    }

    showAlmostEverything(); //nombre, health, water, bottomBar, turn, timeLeft
}
void Cworm::updateWormFrameMoving(int j)
{
//    al_draw_bitmap(background, 0,0,0);
//    al_draw_bitmap(mapa, 0,0,0);
    if (orientation == LEFT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wwalkL[j], pos.x, pos.y,0);
    }
    else if (orientation == RIGHT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wwalkR[j], pos.x, pos.y,0);
    }
    showAlmostEverything(); //nombre, health, water, bottomBar, turn, timeLeft
}
void Cworm::showAllWormsButMyself()
{
    char temp_buffer_for_time[7];
    char temp_buffer_for_time2[7];
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;
    //return;
    //al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(background, 0,0,0);
    al_draw_bitmap(mapa, 0,0,0);
    //al_flip_display();
    vector<Cworm>::iterator i = (pCmap->myWorms).begin();

    //cout << "asasa" << endl;
    //cout << pCmap << endl;
    //cout << (pCmap->mapData).map_name;

    for( ; i!=(pCmap->myWorms).end(); i++)
    {
        if (ID!= i->ID)
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
            if (i->color == BLUE)
                al_draw_text(pCmap->font, al_map_rgb(AL_BLUE), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
            else
                al_draw_text(pCmap->font, al_map_rgb(AL_RED), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
            i->drawHealthBar();
        }
    }
    for(i=(pCmap->enemyWorms).begin(); i!=pCmap->enemyWorms.end(); i++)
    {
        if (ID!= i->ID)
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
            if (i->color == BLUE)
                al_draw_text(pCmap->font, al_map_rgb(AL_BLUE), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
            else
                al_draw_text(pCmap->font, al_map_rgb(AL_RED), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
            i->drawHealthBar();
        }
    }
    pCmap->displayWater();
    drawButtons(pCmap->mouse_state, *pCmap, *this, pCmap->resizeWidth, pCmap->resizeHeight);
//    al_draw_bitmap(pCmap->bottom,0, al_get_bitmap_height(mapa),0);
    if (pCmap->turn == MY_TURN)
        al_draw_bitmap(pCmap->top_my_turn, WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0, 0, 0);
    else
        al_draw_bitmap(pCmap->top_enemy_turn, WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_enemy_turn) / 2.0, 0, 0);

    if (TURN_TIME - al_get_timer_count(pCmap->turn_time) > 9)
        al_draw_text(pCmap->font_for_time, al_map_rgb(255, 255, 255), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 155, 96, 0, itoa(TURN_TIME - al_get_timer_count(pCmap->turn_time), temp_buffer_for_time, 10));
    else
        al_draw_text(pCmap->font_for_time, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 155, 96, 0,
                     strcat(temp_buffer_for_time2, itoa((TURN_TIME - al_get_timer_count(pCmap->turn_time)) >= 0 ? TURN_TIME - al_get_timer_count(pCmap->turn_time) : 0, temp_buffer_for_time, 10)));

    memset(temp_buffer_for_time, 0, 7);
    memset(temp_buffer_for_time2, 0, 7);
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;

    if (TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time) >= 600)
    {
        temp_buffer_for_time2[0] = 0;
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) / 60.0), temp_buffer_for_time, 10));
    }
    else
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) / 60), temp_buffer_for_time, 10));
    strcat(temp_buffer_for_time2, ":");
    if ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60 >= 10)
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60, temp_buffer_for_time, 10));
    else
    {
        strcat(temp_buffer_for_time2, "0");
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60, temp_buffer_for_time, 10));
    }
    ///Modificacion para la feria
    /*
    if (TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time) >= 300)
        al_draw_text(pCmap->font_for_time_small, al_map_rgb(49, 100, 36), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
    else
        al_draw_text(pCmap->font_for_time_small, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
*/



    if (getWindStrength(pCmap->WindStrength) >= 0)
        al_draw_bitmap_region(pCmap->wind_bar,
                              0,
                              0,
                              getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX,
                              36,
                              1496 + 8 + 178,
                              805,
                              0);
    else
        al_draw_bitmap_region(pCmap->wind_bar,
                              0,
                              0,
                              -getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX + 1,
                              36,
                              1502 + getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX - 1,
                              805,
                              ALLEGRO_FLIP_HORIZONTAL);

    if ((pCweapon->type == FUSE) && (!pCweapon->Shots.empty()))
            pCweapon->Shoot(pCmap, this);
    //al_flip_display();
}
void Cworm::showAllWormsBut(vector<int> IDs)
{
    //return;
    al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(background, 0,0,0);
    al_draw_bitmap(mapa, 0,0,0);
    //al_flip_display();
    vector<Cworm>::iterator i;
    vector<int>::iterator j;

    //cout << "asasa" << endl;
    //cout << pCmap << endl;
    //cout << (pCmap->mapData).map_name;

    for(i=(pCmap->myWorms).begin() ; i!=(pCmap->myWorms).end(); i++)
    {
        for (j = IDs.begin(); j!=IDs.end(); j++)
        {
            if ((*j)!= i->ID)
            {
                if (i->orientation == LEFT)
                    al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
                if (i->orientation == RIGHT)
                    al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
            }
        }

    }
    for(i=(pCmap->enemyWorms).begin() ; i!=(pCmap->enemyWorms).end(); i++)
    {
        for (j = IDs.begin(); j!=IDs.end(); j++)
        {
            if ((*j)!= i->ID)
            {
                if (i->orientation == LEFT)
                    al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
                if (i->orientation == RIGHT)
                    al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
            }
        }

    }
    //al_flip_display();
}

void Cworm::showWwalkFrames()
{
    al_set_target_bitmap(al_get_backbuffer(display)); //por las dudas.
    for (unsigned j=0; j<WWALK_FRAMES; j++)
    {
        updateWormFrameMoving(j);
        al_flip_display();
        al_rest(FRAMERATE);
    }
}
Point Cworm::getShotDestination(double speed, int angle)
{
    double parteNoIntdeT = 0.0;
    bool noMeChoco;
    const int x0 = pos.x, y0 = pos.y; //luego las uso para que pos vuelva a tener los valores de antes
    Point posRet;
    int i; //
    int t;
    double vx0, vy0;
    vx0 = (cos(((double)(angle*PI))/180)*speed);
    //NECESARIO PARA CUANDO HAYA EXPLOSIONES.
//    if (vx0<0)
//        orientation = LEFT;
//    else
//        orientation = RIGHT;
    vy0 = (sin(-((double)(angle*PI))/180)*speed); //el - aparece xq y>0 va para abajo

    t=0;
    //cout<<"\nEntrada a shotWorm()\n";
    do
    {
        i=0;
        do
        {
            parteNoIntdeT = ((double)i)/WJUMP_SUBDIVISIONES;
            //cout<<"(t+parteNoIntdeT):"<<(t+parteNoIntdeT)<<endl;
            pos.x = (int)(x0 + vx0*(t+parteNoIntdeT));
            pos.y = (int) (y0 + vy0*(t+parteNoIntdeT) + GRAVITY*(t+parteNoIntdeT)*((t+parteNoIntdeT)+1)/2);
            if ((GRAVITY*((t+parteNoIntdeT))+vy0)<0)
                noMeChoco = noEstaChocandoseVert(0); //va para arriba, entonces me fijo arriba
            else
                noMeChoco = noEstaChocandoseVert(W_HEIGHT); //va para abajo, entonces me fijo abajo
            i++;
            noMeChoco = noMeChoco&&isInsideMap();
        }while(i<WJUMP_SUBDIVISIONES && canWalk() && noMeChoco );//lo hace unas 5 veces, se fija si no se choca con nada en el intervalo entre frame y su siguiente
        if ( (!noMeChoco) )
        {
            lastAction = DO_NOTHING;
            posRet = pos; pos.x = x0; pos.y = y0;
            nextAction = CAIDA_LIBRE;
            return posRet;
        }
//            if (lastAction!=CAIDA_LIBRE)
//            {

//            }
//
//        else
//        {
//            posRet = pos; pos.x = x0; pos.y = y0;
//            nextAction = DO_NOTHING;
//            return posRet;
//        }

        pos.x = x0 + vx0*t;
        pos.y = y0 + vy0*t + GRAVITY*t*(t+1)/2;
        //cout<<"Pos.y:"<<pos.y<<endl;
        t++;
        noMeChoco = noMeChoco&&isInsideMap();

    }while((noMeChoco&&isInsideMap()&&canWalk()));
    if (!(realPosY()<HEIGHT-W_HEIGHT))
        pos.y = HEIGHT-W_HEIGHT_PNG+(W_HEIGHT_PNG-W_HEIGHT)/2-4; //parche xsi llega muy abajo. En la health real se muere x el agua
    if (noEstaChocandoseVert(W_HEIGHT)&&pos.y<(HEIGHT-W_HEIGHT_PNG+(W_HEIGHT_PNG-W_HEIGHT)/2-10))
    {
        //lastAction = ;
        //health -= calculateFallDamage(GRAVITY*t+vy0);
        posRet = pos; pos.x = x0; pos.y = y0;
        nextAction = CAIDA_LIBRE;
        return posRet;
    }

    //updateDamage(GRAVITY*t+vy0); //le paso la velocidad final en y
    posRet = pos; pos.x = x0; pos.y = y0;
    nextAction = DO_NOTHING;
    return posRet;
}
Point Cworm::getJumpDestination()
{
    if (canJump())
    {
        if (orientation == RIGHT)
            return getShotDestination(WJUMP_SPEED,WJUMP_ANGLE);
        else
            return getShotDestination(WJUMP_SPEED,180-WJUMP_ANGLE);
    }
    else
    {
        nextAction = DO_NOTHING;
        return pos;
    }

}

void Cworm::shotWormNormal(double speed, int angle)
{
    /*
    "dispara" y grafica al worm con esa velocidad inicial y angulo(de 0 a 360°, de forma convencional)
    . NO entra en juego la orientation, esta se modifica al ppio
    Actualiza el daño. Nunca es afectado por el viento.
    Las unidades de los parametros recibidos son px/frame y grados respectivamente.
    */
    int t = 0;
    double parteNoIntdeT = 0.0;
    bool noMeChoco;
    int x0 = pos.x, y0 = pos.y, i;
    double vx0, vy0;
    vx0 = (cos(((double)(angle*PI))/180)*speed);
    //NECESARIO PARA CUANDO HAYA EXPLOSIONES.
//    if (vx0<0)
//        orientation = LEFT;
//    else
//        orientation = RIGHT;
    vy0 = (sin(-((double)(angle*PI))/180)*speed); //el - aparece xq y>0 va para abajo

    t=0;
    //cout<<"\nEntrada a shotWorm()\n";
    do
    {
        i=0;
        do
        {
            parteNoIntdeT = ((double)i)/WJUMP_SUBDIVISIONES;
            //cout<<"(t+parteNoIntdeT):"<<(t+parteNoIntdeT)<<endl;
            pos.x = (int)(x0 + vx0*(t+parteNoIntdeT));
            pos.y = (int) (y0 + vy0*(t+parteNoIntdeT) + GRAVITY*(t+parteNoIntdeT)*((t+parteNoIntdeT)+1)/2);
            if ((GRAVITY*((t+parteNoIntdeT))+vy0)<0)
                noMeChoco = noEstaChocandoseVert(0); //va para arriba, entonces me fijo arriba
            else
                noMeChoco = noEstaChocandoseVert(W_HEIGHT); //va para abajo, entonces me fijo abajo
            i++;
            noMeChoco = noMeChoco&&isInsideMap();
        }while(i<WJUMP_SUBDIVISIONES && canWalk() && noMeChoco );//lo hace unas 5 veces, se fija si no se choca con nada en el intervalo entre frame y su siguiente
        if ( !noMeChoco )
        {
            updateWormFrameMoving(0);
            al_flip_display();
            al_rest(FRAMERATE); //
            health -= calculateFallDamage(GRAVITY*t+vy0);
            return;
        }

        pos.x = x0 + vx0*t;
        pos.y = y0 + vy0*t + GRAVITY*t*(t+1)/2;
        //cout<<"Pos.y:"<<pos.y<<endl;
        updateWormFrameMoving(0);
        al_flip_display();
        al_rest(FRAMERATE); //

        t++;
        noMeChoco = noMeChoco&&isInsideMap();

    }while((noMeChoco&&isInsideMap()&&canWalk()));
    if (!(realPosY()<HEIGHT-W_HEIGHT))
        pos.y = HEIGHT-W_HEIGHT_PNG+(W_HEIGHT_PNG-W_HEIGHT)/2-4; //parche xsi llega muy abajo. En la health real se muere x el agua
    if (noEstaChocandoseVert(W_HEIGHT)&&lastAction!=CAIDA_LIBRE)
    {
        lastAction = CAIDA_LIBRE;
        health -= calculateFallDamage(GRAVITY*t+vy0);
        nextAction = CAIDA_LIBRE;
        //shotWorm(0.0,0); //caida libre. Se choco con un obstaculo durante el salto. Debe caer
        return;
    }

    //updateDamage(GRAVITY*t+vy0); //le paso la velocidad final en y
    health -= calculateFallDamage(GRAVITY*t+vy0);
}

void Cworm::shotWorm(double speed, int angle)
{
    /*
    "dispara" y grafica al worm con esa velocidad inicial y angulo(de 0 a 360°, de forma convencional)
    . NO entra en juego la orientation, esta se modifica al ppio
    Actualiza el daño. Nunca es afectado por el viento.
    Las unidades de los parametros recibidos son px/frame y grados respectivamente.
    */
    int t = 0;
    double parteNoIntdeT = 0.0;
    bool noMeChoco;
    int x0 = pos.x, y0 = pos.y, i;
    double vx0, vy0;
    vx0 = (cos(((double)(angle*PI))/180)*speed);
    //NECESARIO PARA CUANDO HAYA EXPLOSIONES.
//    if (vx0<0)
//        orientation = LEFT;
//    else
//        orientation = RIGHT;
    vy0 = (sin(-((double)(angle*PI))/180)*speed); //el - aparece xq y>0 va para abajo

    t=0;
    //cout<<"\nEntrada a shotWorm()\n";
    do
    {
        i=0;
        do
        {
            parteNoIntdeT = ((double)i)/WJUMP_SUBDIVISIONES;
            //cout<<"(t+parteNoIntdeT):"<<(t+parteNoIntdeT)<<endl;
            pos.x = (int)(x0 + vx0*(t+parteNoIntdeT));
            pos.y = (int) (y0 + vy0*(t+parteNoIntdeT) + GRAVITY*(t+parteNoIntdeT)*((t+parteNoIntdeT)+1)/2);
            if ((GRAVITY*((t+parteNoIntdeT))+vy0)<0)
                noMeChoco = noEstaChocandoseVert(0); //va para arriba, entonces me fijo arriba
            else
                noMeChoco = noEstaChocandoseVert(W_HEIGHT); //va para abajo, entonces me fijo abajo
            i++;
            noMeChoco = noMeChoco&&isInsideMap();
        }while(i<WJUMP_SUBDIVISIONES && canWalk() && noMeChoco );//lo hace unas 5 veces, se fija si no se choca con nada en el intervalo entre frame y su siguiente
        if ( !noMeChoco )
        {
            showWormInAir();
            al_flip_display();
            al_rest(FRAMERATE); //
            health -= calculateFallDamage(GRAVITY*t+vy0);
            updateWormFrameMoving(0);
            al_flip_display();
            return;
        }

        pos.x = x0 + vx0*t;
        pos.y = y0 + vy0*t + GRAVITY*t*(t+1)/2;
        //cout<<"Pos.y:"<<pos.y<<endl;
        showWormInAir();
        al_flip_display();
        al_rest(FRAMERATE); //

        t++;
        noMeChoco = noMeChoco&&isInsideMap();

    }while((noMeChoco&&isInsideMap()&&canWalk()));
    if (!(realPosY()<HEIGHT-W_HEIGHT))
        pos.y = HEIGHT-W_HEIGHT_PNG+(W_HEIGHT_PNG-W_HEIGHT)/2-4; //parche xsi llega muy abajo. En la health real se muere x el agua
    if (noEstaChocandoseVert(W_HEIGHT)&&lastAction!=CAIDA_LIBRE)
    {
        lastAction = CAIDA_LIBRE;
        health -= calculateFallDamage(GRAVITY*t+vy0);
        nextAction = CAIDA_LIBRE;
        //shotWorm(0.0,0); //caida libre. Se choco con un obstaculo durante el salto. Debe caer
        updateWormFrameMoving(0);
        al_flip_display();
        return;
    }

    //updateDamage(GRAVITY*t+vy0); //le paso la velocidad final en y
    health -= calculateFallDamage(GRAVITY*t+vy0);
    updateWormFrameMoving(0);
    al_flip_display();

}
void Cworm::jump()
{
    if (canJump())
    {
        if (orientation == RIGHT)
            shotWorm(WJUMP_SPEED,WJUMP_ANGLE);
        else
            shotWorm(WJUMP_SPEED,180-WJUMP_ANGLE);
    }
}
bool Cworm::canJump()
{
    /*
    Se fija en un entorno de la parte superior del worm si es que hay algo arriba o no.
    El entorno es un rectangulo de W_WIDTH x ENTORNO_DELTA
    */

    return canWalk()&&noEstaChocandoseVert(0); //me fijo en el entorno del costado (antes me fije en el de arriba)
}
int Cworm::calculateFallDamage(double verticalSpeed)
{
    if (verticalSpeed<=(-18.0/50.0+8-1/65536)) //si se choca la cabeza no pierde health
        return 0; //sino retorna que se debe agrandar la health
    else
        return (int)(((verticalSpeed- 8 + 1/65536) * 50+ 18) / 18);
}
void Cworm::jumpLindo()
{
    /*
    Cualquier similitud con shotWorm() es pura coincidencia :P. jajaj
    */
    double speed = WJUMP_SPEED;
    int angle = WJUMP_ANGLE;
    int t = 0;
    int x0 = pos.x, y0 = pos.y;
    double vx0, vy0;
    vx0 = (cos(((double)(angle*PI))/180)*speed); //el - aparece xq y>0 va para abajo
    if (orientation == LEFT)
        vx0 = -vx0;
    vy0 = (sin(-((double)(angle*PI))/180)*speed); //el - aparece xq y>0 va para abajo

    t=0;
    do
    {
        pos.x = x0 + vx0*t;
        pos.y = y0 + vy0*t + GRAVITY*t*(t+1)/2;
        al_draw_bitmap(background,0,0,0);
        al_draw_bitmap(mapa,0,0,0);
        if (t<WJUMP_FRAMES)
        {
            if (orientation== LEFT)
                al_draw_bitmap(pGif->wjumpL[t],pos.x,pos.y,0);
            else if (orientation == RIGHT)
                al_draw_bitmap(pGif->wjumpR[t],pos.x,pos.y,0);
        }
        else
        {
            if (orientation== LEFT)
                al_draw_bitmap(pGif->wwalkL[0],pos.x,pos.y,0);
            else if (orientation == RIGHT)
                al_draw_bitmap(pGif->wwalkR[0],pos.x,pos.y,0);
        }

        t++;
        al_flip_display();
        al_rest(FRAMERATE); //
    }while(noEstaChocandoseVert(0)&&isInsideMap()&&canWalk());

    //updateDamage(GRAVITY*t+vy0); //le paso la velocidad final en y

}
void Cworm::drawHealthBar()
{
    if(health<0)
        health = 0; //corrijo la vida x las dudas.
    al_draw_rectangle(realPosX(), pos.y-10, realPosX()+W_WIDTH, pos.y-10+5, al_map_rgb(0,0,0), 0);
    if (color == BLUE)
        al_draw_filled_rectangle(realPosX(), pos.y-10, realPosX()+((W_WIDTH-1)*health)/START_VIDA, pos.y-10+5-1, al_map_rgb(AL_BLUE));
    if(color == RED)
        al_draw_filled_rectangle(realPosX(), pos.y-10, realPosX()+((W_WIDTH-1)*health)/START_VIDA, (pos.y-10+5-1), al_map_rgb(AL_RED));
}
void Cworm::showBazookaFrame(int j)
{
    if (orientation == LEFT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wUnloadBazookaL[j], pos.x, pos.y,0);
    }
    else if (orientation == RIGHT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wUnloadBazookaR[j], pos.x, pos.y,0);
    }
    showAlmostEverything(); //nombre, health, water, bottomBar, turn, timeLeft
}
void Cworm::showLoadBazooka()
{
    for (int j = WBAZOOKA_FRAMES-1; j!=0; j-- )
    {
        showBazookaFrame(j);
        al_flip_display();
        al_rest(FRAMERATE);
    }
}
void Cworm::showUnloadBazooka()
{
    for (int j = 0; j!=WBAZOOKA_FRAMES; j++ )
    {
        showBazookaFrame(j);
        al_flip_display();
        al_rest(FRAMERATE);
    }
}
//void showUnloadBazooka();
void Cworm::showAlmostEverything()
{
    char temp_buffer_for_time[7];
    char temp_buffer_for_time2[7];
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;
    if (color == BLUE)
        al_draw_text(pCmap->font, al_map_rgb(AL_BLUE), realPosX() - LETTER_LEN * name.length(), pos.y, 0, name.c_str());
    else
        al_draw_text(pCmap->font, al_map_rgb(AL_RED), realPosX() - LETTER_LEN * name.length(), pos.y, 0, name.c_str());
    drawHealthBar();
    pCmap->displayWater();
    drawButtons(pCmap->mouse_state, *pCmap, *this, pCmap->resizeWidth, pCmap->resizeHeight);
//    al_draw_bitmap(pCmap->bottom,0, al_get_bitmap_height(mapa),0);
    if (pCmap->turn == MY_TURN)
        al_draw_bitmap(pCmap->top_my_turn, WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0, 0, 0);
    else
        al_draw_bitmap(pCmap->top_enemy_turn, WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_enemy_turn) / 2.0, 0, 0);

    if (TURN_TIME - al_get_timer_count(pCmap->turn_time) > 9)
        al_draw_text(pCmap->font_for_time, al_map_rgb(255, 255, 255), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 155, 96, 0, itoa(TURN_TIME - al_get_timer_count(pCmap->turn_time), temp_buffer_for_time, 10));
    else
        al_draw_text(pCmap->font_for_time, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 155, 96, 0,
                     strcat(temp_buffer_for_time2, itoa((TURN_TIME - al_get_timer_count(pCmap->turn_time)) >= 0 ? TURN_TIME - al_get_timer_count(pCmap->turn_time) : 0, temp_buffer_for_time, 10)));

    memset(temp_buffer_for_time, 0, 7);
    memset(temp_buffer_for_time2, 0, 7);
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;

    if (TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time) >= 600)
    {
        temp_buffer_for_time2[0] = 0;
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) / 60.0), temp_buffer_for_time, 10));
    }
    else
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) / 60), temp_buffer_for_time, 10));
    strcat(temp_buffer_for_time2, ":");
    if ((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60 >= 10)
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60, temp_buffer_for_time, 10));
    else
    {
        strcat(temp_buffer_for_time2, "0");
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time)) % 60, temp_buffer_for_time, 10));
    }
    ///Modificacion para la feria
    /*
    if (TOTAL_GAME_TIME - al_get_timer_count(pCmap->total_time) >= 300)
        al_draw_text(pCmap->font_for_time_small, al_map_rgb(49, 100, 36), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
    else
        al_draw_text(pCmap->font_for_time_small, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(pCmap->top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
*/


    if (getWindStrength(pCmap->WindStrength) >= 0)
        al_draw_bitmap_region(pCmap->wind_bar,
                              0,
                              0,
                              getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX,
                              36,
                              1496 + 8 + 178,
                              805,
                              0);
    else
        al_draw_bitmap_region(pCmap->wind_bar,
                              0,
                              0,
                              -getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX + 1,
                              36,
                              1502 + getWindStrength(pCmap->WindStrength) * WIND_BAR_FIX - 1,
                              805,
                              ALLEGRO_FLIP_HORIZONTAL);

    if ((pCweapon->type == FUSE) && (!pCweapon->Shots.empty()))
        pCweapon->Shoot(pCmap, this);

}
void Cworm::showWormInAir()
{
    if (orientation == LEFT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wAirL, pos.x, pos.y, 0);
        showAlmostEverything();
    }
    if (orientation == RIGHT)
    {
        showAllWormsButMyself();
        al_draw_bitmap(pGif->wAirR, pos.x, pos.y, 0);
        showAlmostEverything();
    }
}
