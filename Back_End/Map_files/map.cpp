#include "map.h"
#include "../../Front_end/runGame.h"
#include "../Communication_files/server-client.h"
#include <string>
using namespace std;
#include <iostream>
#include <fstream>
extern "C"
{
    #include <stdlib.h>
}

const char* red_team_names[MAX_NUM_WORMS] = {"Newton", "Einstein", "Maxwell", "Bohr", "Planck", "Galileo"};
const char* blue_team_names[MAX_NUM_WORMS] = {"Euler", "Gauss", "Riemann", "Cauchy", "Lagrange", "Leibniz"};

Cmap::Cmap(ALLEGRO_DISPLAY *disp, MapData& mapData_, string mapa_, string background_, char WindStrength_)
{
    display = disp;
    mapa = al_load_bitmap(("./Images/Scenarios/"+mapa_).c_str());
    background = al_load_bitmap(("./Images/"+background_).c_str());
    if (!(mapa||background))
        return;
    al_lock_bitmap(mapa,al_get_bitmap_format(mapa), ALLEGRO_LOCK_READWRITE );
    //lo pongo read&wrte para poder poner pixeles
    //RECORDAR CUANDO SE QUIERA MODIFICAR EL MAPA (cuando se dispara un arma por ej) , HACER
    //void al_unlock_bitmap(ALLEGRO_BITMAP *bitmap), Y LUEGO RE LOCKEARLO AL FINAL DE LA FUNCION.
    mapData = mapData_;
    WindStrength = WindStrength_;
    //avg_color = alGetAvgColor(mapa);
    water = al_load_bitmap("./Images/water.png");
    bottom = al_load_bitmap("./Images/Bottom2.png");
    top_my_turn = al_load_bitmap("./Images/top_my_turn.png");
    top_enemy_turn = al_load_bitmap("./Images/top_enemy_turn.png");
    wind_bar = al_load_bitmap("./Images/wind_bar.png");
    button = al_load_bitmap("./Images/buttons/button.png");
    button_pressed = al_load_bitmap("./Images/buttons/button_pressed.png");
    button_selected = al_load_bitmap("./Images/buttons/button_selected.png");
    weapon_bar = al_load_bitmap("./Images/buttons/weapon_bar.png");

    resizeWidth = WIDTH;
    resizeHeight = HEIGHT;

    char path[64];
    char path2[64];
    char c[3];
    for (int i=0; i<WWALK_FRAMES; i++)  /// WWALK
    {
        strcpy(path, "./Images/wwalking/wwalk-F");
        itoa(i+1,c,10);
        strcat(path, c);
        strcat(path, ".png");

//        cout<<path<<endl;
        gif.wwalkL[i] = al_load_bitmap(path);
        path[strlen(path)-strlen(".png")]='\0';
        strcat(path, "R.png");
        gif.wwalkR[i] = al_load_bitmap(path);
    }
    for (int i=0; i<WJUMP_FRAMES; i++) /// WJUMP
    {
        strcpy(path, "./Images/wjump/wjump-F");
        itoa(i+1,c,10);
        strcat(path, c);
        strcpy(path2, path);
        strcat(path, ".png");
        strcat(path2, ".png"); //agregar la R despues!
        //cout<<path2<<endl;
        gif.wjumpL[i] = al_load_bitmap(path);
        gif.wjumpR[i] = al_load_bitmap(path2);
    }
    for (int i=0; i<WBAZOOKA_FRAMES; i++) ///WLOADBAZOOKA
    {
        strcpy(path, "./Images/Weapons/bazooka/wbazbak-F");
        itoa(i+1,c,10);
        strcat(path, c);
        strcpy(path2, path);
        strcat(path, ".png");
        strcat(path2, ".png"); //agregar la R despues!
        //cout<<path2<<endl;
        gif.wUnloadBazookaL[i] = al_load_bitmap(path);
        path[strlen(path)-strlen(".png")]='\0';
        strcat(path, "R.png");
        gif.wUnloadBazookaR[i] = al_load_bitmap(path);
    }
    al_draw_bitmap(gif.wUnloadBazookaR[4], 300,300, 0);
    al_flip_display();
    gif.wAirR = al_load_bitmap("./Images/wjump/worm_airR.png");
    gif.wAirL = al_load_bitmap("./Images/wjump/worm_air.png");
    font = al_load_font("./Fonts/RockwellStd.otf", FONT_SIZE, 0);
    font_for_fuse = al_load_font("./Fonts/RockwellStd.otf", 20, 0);
    font_for_time = al_load_font("./Fonts/BebasNeue.otf", FONT_SIZE_FOR_TIME, 0);
    font_for_time_small = al_load_font("./Fonts/BebasNeue.otf", FONT_SIZE_FOR_TIME - 20, 0);
//
//    for (int i=1; i<wormQuantity; i++)
//    }
//        (this->myWorms).push_back(cWorm);
//    }

    turn_time = al_create_timer(1);
    total_time = al_create_timer(1);

    //mapa streetFigther{1606,158};
    //mapa 11:{1400,686-(W_HEIGHT_PNG-W_HEIGHT)/2-W_HEIGHT};
    //mapa17:{1440,179}; //parra el street fighter: {330,171}; //punto de prueba
//
//    Cworm cWorm(disp,pto,RIGHT,mapa,background,&gif, 0, this);
//
//
//
//    (myWorms).push_back(cWorm);
//    pto.x -= 500;

//-comienzo prueba chota.
//    al_draw_bitmap(this->mapa, 0,0,0);
//    al_flip_display();
//    al_clear_to_color(al_map_rgb(0,0,0));
//    al_flip_display();
//    al_draw_bitmap((static_cast<Cmap*>(gato))->mapa,0,0,0);
    vector<Cworm>::iterator i;
    i = myWorms.begin();
    al_flip_display();

//- FIN prueba chota.

//    Cworm cWorm2(disp,pto,RIGHT,mapa,background,&gif, 1,this);
//    (myWorms).push_back(cWorm2);
//    for (int i = 0; i<ALGO; i++)
//    {
//        cWorm(disp,pto,RIGHT,mapa,background,&gif, i);
//        (myWorms).push_back(cWorm);
//        i++;
//        cWorm(disp,pto,RIGHT,mapa,background,&gif, i);
//        (enemyWorms).push_back(cWorm);
//    }

}
void Cmap::fillMapData(string mapName)
{ //mapName tiene que ser algo asi: "mapa21312.png" o .map
    Point point;
    string line;
    mapName.erase(mapName.end()-sizeof(".png")+1,mapName.end());

    mapName = mapName+".map";
    mapName = "./Images/Scenarios/"+mapName;
    cout<<endl;
    ifstream myfile (mapName.c_str());
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            if(line[0] == 'r')
            {
               // cout<<line<<endl;
//                cout<<line.substr(line.find(' ')+1, line.find(';')-line.find(' ')-1)<<endl;
//                cout<<line.substr(line.find(';')+1, line.rfind(';')-line.find(';')-1)<<endl;
//                cout<<endl;
                sscanf((line.substr(line.find(' ')+1, line.find(';')-line.find(' ')-1)).c_str(), "%d", &(point.x));
                sscanf((line.substr(line.find(';')+1, line.rfind(';')-line.find(';')-1)).c_str(), "%d", &(point.y));
                mapData.redTeamXY.push_back(point);
                if (line[line.size()-1]=='L')
                    mapData.redTeamOri.push_back(LEFT);
                else if (line[line.size()-1]=='R')
                    mapData.redTeamOri.push_back(RIGHT);
            }
            else if (line[0] == 'b')
            {
                //cout<<line<<endl;
//                cout<<line.substr(line.find(' ')+1, line.find(';')-line.find(' ')-1)<<endl;
//                cout<<line.substr(line.find(';')+1, line.rfind(';')-line.find(';')-1)<<endl;
//                cout<<endl;

                sscanf((line.substr(line.find(' ')+1, line.find(';')-line.find(' ')-1)).c_str(), "%d", &(point.x));
                sscanf((line.substr(line.find(';')+1, line.rfind(';')-line.find(';')-1)).c_str(), "%d", &(point.y));
                mapData.blueTeamXY.push_back(point);
                if (line[line.size()-1]=='L')
                    mapData.blueTeamOri.push_back(LEFT);
                else if (line[line.size()-1]=='R')
                    mapData.blueTeamOri.push_back(RIGHT);
            }
        }
        myfile.close();
    }
    else cout << "Unable to open '.map' file.\n";

//    for (vector<Point>::iterator i= mapData.redTeamXY.begin(); i!=mapData.redTeamXY.end(); i++)
//        cout<<i->x<<"; "<<i->y<<endl;
//    for (vector<Point>::iterator i= mapData.blueTeamXY.begin(); i!=mapData.blueTeamXY.end(); i++)
//        cout<<i->x<<"; "<<i->y<<endl;



}

void Cmap::createWorms(Bazooka* _Bazooka)
{
    int i = 1;
    int k = 0;
    //char c[4];
    for (vector<Point>::iterator j=mapData.blueTeamXY.begin(); j!=mapData.blueTeamXY.end();j++)
    {
        Cworm cWorm(display,*j,mapData.blueTeamOri[k],mapa,background,&gif, i, this, _Bazooka);
        i++;
        k++;
        if (myWormsColor == BLUE)
        {
            cWorm.color = BLUE;
            /*
            cWorm.name = "b";
            itoa(k, c, 10);
            cWorm.name += c;
            */
            if (k - 1 < MAX_NUM_WORMS)
                cWorm.name = blue_team_names[k - 1];
            else
                cWorm.name = "Worm";
            (myWorms).push_back(cWorm);
        }
        else if (enemyWormsColor == BLUE)
        {
            cWorm.color = BLUE;
            /*
            cWorm.name = "b";
            itoa(k, c, 10);
            cWorm.name += c;
            */
            if (k - 1 < MAX_NUM_WORMS)
                cWorm.name = blue_team_names[k - 1];
            else
                cWorm.name = "Worm";
            (enemyWorms).push_back(cWorm);
        }

    }
    k=0;
    for (vector<Point>::iterator j=mapData.redTeamXY.begin(); j!=mapData.redTeamXY.end();j++)
    {
        Cworm cWorm(display,*j,mapData.redTeamOri[k],mapa,background,&gif, i, this, _Bazooka);
        i++;
        k++;
        if (myWormsColor == RED)
        {
            cWorm.color = RED;
            /*
            cWorm.name = "r";
            itoa(k, c, 10);
            cWorm.name += c;
            */
            if (k - 1 < MAX_NUM_WORMS)
                cWorm.name = red_team_names[k - 1];
            else
                cWorm.name = "Worm";
            (myWorms).push_back(cWorm);
        }
        else if (enemyWormsColor == RED)
        {
            cWorm.color = RED;
            /*
            cWorm.name = "r";
            itoa(k, c, 10);
            cWorm.name += c;
            */
            if (k - 1 < MAX_NUM_WORMS)
                cWorm.name = red_team_names[k - 1];
            else
                cWorm.name = "Worm";
            (enemyWorms).push_back(cWorm);
        }
//        if (myWormsColor == RED)
//            (myWorms).push_back(cWorm);
//        else if (enemyWormsColor == RED)
//            (enemyWorms).push_back(cWorm);
    }
}

void Cmap::showWorms()
{
    char temp_buffer_for_time[7];
    char temp_buffer_for_time2[7];
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;
    al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(background, 0,0,0);
    al_draw_bitmap(mapa, 0,0,0);

    vector<Cworm>::iterator i;
    for (i=myWorms.begin(); i!=myWorms.end(); i++)
    {
        if (i->state == AIR)
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wAirL,i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wAirL,i->pos.x,i->pos.y,0);
        }
        else
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
        }
        if (i->color == BLUE)
            al_draw_text(font, al_map_rgb(AL_BLUE), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
        else
           al_draw_text(font, al_map_rgb(AL_RED), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
        i->drawHealthBar();
    }
    for (i=enemyWorms.begin(); i!=enemyWorms.end(); i++)
    {
        if (i->state == AIR)
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wAirL,i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wAirL,i->pos.x,i->pos.y,0);
        }
        else
        {
            if (i->orientation == LEFT)
                al_draw_bitmap(i->pGif->wwalkL[0],i->pos.x,i->pos.y,0);
            if (i->orientation == RIGHT)
                al_draw_bitmap(i->pGif->wwalkR[0],i->pos.x,i->pos.y,0);
        }

        if (i->color == BLUE)
            al_draw_text(font, al_map_rgb(AL_BLUE), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
        else
            al_draw_text(font, al_map_rgb(AL_RED), i->realPosX() - LETTER_LEN * i->name.length(), i->pos.y, 0, i->name.c_str());
        i->drawHealthBar();
    }
    displayWater();


    if (turn == MY_TURN)
        al_draw_bitmap(top_my_turn, WIDTH / 2.0 - al_get_bitmap_width(top_my_turn) / 2.0, 0, 0);
    else
        al_draw_bitmap(top_enemy_turn, WIDTH / 2.0 - al_get_bitmap_width(top_enemy_turn) / 2.0, 0, 0);

    if (TURN_TIME - al_get_timer_count(turn_time) > 9)
        al_draw_text(font_for_time, al_map_rgb(255, 255, 255), WIDTH / 2.0 - al_get_bitmap_width(top_my_turn) / 2.0 + 155, 96, 0, itoa(TURN_TIME - al_get_timer_count(turn_time), temp_buffer_for_time, 10));
    else
        al_draw_text(font_for_time, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(top_my_turn) / 2.0 + 155, 96, 0,
                     strcat(temp_buffer_for_time2, itoa((TURN_TIME - al_get_timer_count(turn_time)) >= 0 ? TURN_TIME - al_get_timer_count(turn_time) : 0, temp_buffer_for_time, 10)));

    memset(temp_buffer_for_time, 0, 7);
    memset(temp_buffer_for_time2, 0, 7);
    temp_buffer_for_time2[0] = '0';
    temp_buffer_for_time2[1] = 0;

    if (TOTAL_GAME_TIME - al_get_timer_count(total_time) >= 600)
    {
        temp_buffer_for_time2[0] = 0;
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(total_time)) / 60.0), temp_buffer_for_time, 10));
    }
    else
        strcat(temp_buffer_for_time2, itoa((int) ((TOTAL_GAME_TIME - al_get_timer_count(total_time)) / 60), temp_buffer_for_time, 10));
    strcat(temp_buffer_for_time2, ":");
    if ((TOTAL_GAME_TIME - al_get_timer_count(total_time)) % 60 >= 10)
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(total_time)) % 60, temp_buffer_for_time, 10));
    else
    {
        strcat(temp_buffer_for_time2, "0");
        strcat(temp_buffer_for_time2, itoa((TOTAL_GAME_TIME - al_get_timer_count(total_time)) % 60, temp_buffer_for_time, 10));
    }
    ///Modificacion para la feria
    /*
    if (TOTAL_GAME_TIME - al_get_timer_count(total_time) >= 300)
        al_draw_text(font_for_time_small, al_map_rgb(49, 100, 36), WIDTH / 2.0 - al_get_bitmap_width(top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
    else
        al_draw_text(font_for_time_small, al_map_rgb(235, 0, 0), WIDTH / 2.0 - al_get_bitmap_width(top_my_turn) / 2.0 + 195, 110, 0, temp_buffer_for_time2);
*/



    if (getWindStrength(WindStrength) >= 0)
        al_draw_bitmap_region(wind_bar,
                              0,
                              0,
                              getWindStrength(WindStrength) * WIND_BAR_FIX,
                              36,
                              1496 + 8 + 178,
                              805,
                              0);
    else
        al_draw_bitmap_region(wind_bar,
                              0,
                              0,
                              -getWindStrength(WindStrength) * WIND_BAR_FIX + 1,
                              36,
                              1502 + getWindStrength(WindStrength) * WIND_BAR_FIX - 1,
                              805,
                              ALLEGRO_FLIP_HORIZONTAL);
    al_flip_display();
}
ALLEGRO_COLOR Cmap::alGetAvgColor(ALLEGRO_BITMAP* map)
{
    int x, y;
    int map_height = al_get_bitmap_height(map);
    int map_width = al_get_bitmap_width(map);
    int number_pixels = 0;
    ALLEGRO_COLOR avg_color = {0, 0, 0, 0};

    al_lock_bitmap(map, al_get_bitmap_format(map), ALLEGRO_LOCK_READWRITE);
    for (x = 0; x < map_width; x++)
    {
        for (y = 0; y < map_height; y++)
        {
            if (al_get_pixel(map, x, y).a != 0)
            {
                avg_color.r += al_get_pixel(map, x, y).r;
                avg_color.b  += al_get_pixel(map, x, y).b;
                avg_color.g += al_get_pixel(map, x, y).g;
                number_pixels++;
            }

        }
    }
    al_unlock_bitmap(map);

    avg_color.r /= number_pixels;
    avg_color.b  /= number_pixels;
    avg_color.g /= number_pixels;
    avg_color.a = 255;

    return avg_color;
}
bool Cmap::deleteUnhealthyWorms()
{
    bool deletion = false;
    for (vector<Cworm>::iterator i = myWorms.begin(); i!=myWorms.end();i++)
    {
        if (i->health <= 0)
        {
            myWorms.erase(i); /// MATO al worm
            i--;
            deletion = true;
        }
    }
    for (vector<Cworm>::iterator i = enemyWorms.begin(); i!=enemyWorms.end();i++)
    {
        if (i->health <= 0)
        {
            enemyWorms.erase(i); /// MATO al worm
            i--;
            deletion = true;
        }
    }
    return deletion;
}
bool Cmap::deleteDrownedWorms()
{
    bool deletion = false;
    for (vector<Cworm>::iterator i = myWorms.begin(); i!=myWorms.end();i++)
    {
        if ((i->realPosY()>al_get_bitmap_height(mapa)-W_HEIGHT) || ((i->realPosX() > al_get_bitmap_width(mapa)) || (i->realPosX() < 0)))
        {
            (i->realPosX() < 0);
            myWorms.erase(i); /// MATO al worm
            i--;
            deletion = true;
        }
    }
    for (vector<Cworm>::iterator i = enemyWorms.begin(); i!=enemyWorms.end();i++)
    {
        if ((i->realPosY()>al_get_bitmap_height(mapa)-W_HEIGHT) || ((i->realPosX() > al_get_bitmap_width(mapa)) || (i->realPosX() < 0)))
        {
            enemyWorms.erase(i); /// MATO al worm
            i--;
            deletion = true;
        }
    }
    return deletion;
}
void Cmap::displayWater()
{
    al_draw_bitmap(water, 0, al_get_bitmap_height(mapa)-WATER_HEIGHT, 0);
}

Cmap::~Cmap()
{
    al_destroy_bitmap(mapa);
    al_destroy_bitmap(background);
    al_destroy_bitmap(water);
    al_destroy_bitmap(bottom);
    al_destroy_bitmap(top_my_turn);
    al_destroy_bitmap(top_enemy_turn);
    al_destroy_bitmap(wind_bar);
    al_destroy_bitmap(button);
    al_destroy_bitmap(button_pressed);
    al_destroy_bitmap(button_selected);
    al_destroy_bitmap(weapon_bar);

    al_destroy_timer(turn_time);
    al_destroy_timer(total_time);

    al_destroy_font(font);
    al_destroy_font(font_for_fuse);
    al_destroy_font(font_for_time);
    al_destroy_font(font_for_time_small);
}
