#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "runGame.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../Back_End/Weapons_files/weapons.h"
#include "../Back_End/Weapons_files/firepunch.h"
#include "../Back_End/Weapons_files/shotgun.h"
#include "../Back_End/Weapons_files/grenade.h"
#include "../Back_End/Weapons_files/pneumaticdrill.h"
#include "../Back_End/Weapons_files/dynamite.h"
#include "../Back_End/Weapons_files/mine.h"
#include "../Back_End/Weapons_files/uzi.h"
#include "../Back_End/Weapons_files/airstrike.h"
#include "../Back_End/Weapons_files/banana.h"
#include "../Back_End/Weapons_files/clusterbomb.h"
#include "other_buttons.h"
#include "../Back_End/Worms_files/worms.h"
#include "../Back_End/Map_files/map.h"
#include "../Back_End/Communication_files/server-client.h"

#define MAX_WAIT_BTWN_TURN 60 //In seconds

using namespace std;

int runGame(PolonetConn conn, initData& Data)
{
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_KEYBOARD_STATE keybState;
    ALLEGRO_TIMER* refresh_timer = NULL;
    ALLEGRO_TIMER* timeout_timer = NULL;
    ALLEGRO_TIMER* block_key_left_arrow = NULL;
    ALLEGRO_TIMER* block_key_right_arrow = NULL;
    ALLEGRO_TIMER* block_key_tab = NULL;
    ALLEGRO_BITMAP* loading_bar = NULL;
    ALLEGRO_BITMAP* crosshair_image_b = NULL;
    ALLEGRO_BITMAP* crosshair_image_r = NULL;
    ALLEGRO_BITMAP* you_lose = NULL;
    ALLEGRO_BITMAP* you_win = NULL;
    ALLEGRO_SAMPLE* weapon_powerup = NULL;
    ALLEGRO_EVENT_QUEUE * displayEventQueue = NULL;
    ALLEGRO_SAMPLE* soundtrack = NULL;

    // Start Modifyed by Agus PM 05-Mar-2014
    ALLEGRO_SAMPLE_INSTANCE * tempSampleInstance=NULL;
	ALLEGRO_SAMPLE_INSTANCE * soundtrackInstance=NULL;
    // End Modifyed by Agus PM 05-Mar-2014

    //ALLEGRO_SAMPLE_ID sample_id;
    //ALLEGRO_SAMPLE_ID sample_id_soundtrack;
    bool dont_play_twice = true;
    int64_t past_refresh_timer = 0;
    int64_t past_block_key_left_arrow = 0;
    int64_t past_block_key_right_arrow = 0;
    int64_t past_block_key_tab = 0;
    MapData mapData = Data.wmap;
    int bytes_sent = 0;
    char buffer[MAX_PACKET];
    int crosshair_angle = 0; //Angle between 180 and -180 degrees
    int crosshair_angle_enemy;
    char temp_angle;
    bool cycling_allowed = true;
    char game_status = ONGOING;
    char c = 'n';
    Point temp_point;
    bool finished_shooting;
    char fuse = 5;

    ALLEGRO_MOUSE_STATE mouse_state;
    ALLEGRO_MOUSE_STATE previous_mouse_state;

    ALLEGRO_KEYBOARD_STATE previous_keyboard;

    int64_t previous_time = 0;
    int64_t total_time = 0;
    int64_t past_TotalTimeKey = 0;

    Point ptoAenviar;

    loading_bar = al_load_bitmap("./Images/loading_bar.png");
    crosshair_image_b = al_load_bitmap("./Images/Crosshair/crshairb.png");
    crosshair_image_r = al_load_bitmap("./Images/Crosshair/crshairr.png");
    you_lose = al_load_bitmap("./Images/You_Lose.png");
    you_win = al_load_bitmap("./Images/You_Win.png");

    weapon_powerup = al_load_sample("./Sounds/rocketpowerup.wav");
    //soundtrack = al_load_sample("./Sounds/Soundtrack.wav");
    //al_play_sample(soundtrack, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &sample_id_soundtrack);

    timeout_timer = al_create_timer(1);
    refresh_timer = al_create_timer(FRAMERATE);
    block_key_left_arrow = al_create_timer(0.35);
    block_key_right_arrow = al_create_timer(0.35);
    block_key_tab = al_create_timer(9999);

    // Start Modifyed by Agus PM 05-Mar-2014
	soundtrack = al_load_sample( ".\\Sounds\\Soundtrack.ogg" );

    if (!soundtrack)
    {
        printf( "Audio clip sample not loaded!\n" );
        return -1;
    }

	soundtrackInstance = al_create_sample_instance(soundtrack);
	if (!soundtrackInstance)
    {
        printf( "Sound Track Instance not created!\n" );
        return -1;
    }
	al_attach_sample_instance_to_mixer(soundtrackInstance, al_get_default_mixer());
	al_play_sample_instance(soundtrackInstance);
	// End Modifyed by Agus PM 05-Mar-2014

    displayEventQueue = al_create_event_queue();

    if ((refresh_timer != NULL) && (refresh_timer != NULL) && (loading_bar != NULL)
            && (crosshair_image_b != NULL) && (crosshair_image_r != NULL) && (block_key_left_arrow != NULL)
            && (block_key_right_arrow != NULL) && (block_key_tab != NULL) && (displayEventQueue != NULL)
            && (you_lose != NULL) && (you_win != NULL))
    {

        int width = WIDTH;
        int height = HEIGHT;

		double resizeWidth  =WIDTH;
        double resizeHeight =HEIGHT;

        Bazooka _Bazooka("./Images/Weapons/bazooka/missile.png", "./Images/Weapons/bazooka/wbazbak-F1.png");
        FirePunch _FirePunch("./Images/Weapons/firePunch/wFirePunch.png", "./Images/Weapons/firePunch/wbndbak.png");
        Shotgun _Shotgun("./Images/Weapons/bazooka/missile.png", "./Images/Weapons/shotgun/wshotg.png");
        Grenade _Grenade("./Images/Weapons/grenade/grenade.png", "./Images/Weapons/grenade/wgrnbak.png");
        PneumaticDrill _PneumaticDrill("./Images/Weapons/pneumaticDrill/wdrllnk.png", "./Images/Weapons/pneumaticDrill/wPneumaticDrill.png");
        Dynamite _Dynamite("./Images/Weapons/dynamite/dynamite.png", "./Images/Weapons/dynamite/wdynbak.png");
        Mine _Mine("./Images/Weapons/mine/mineoff.png", "./Images/Weapons/mine/wminbak.png");
        Uzi _Uzi("./Images/Weapons/uzi/uzihit.png", "./Images/Weapons/uzi/wuzi.png");
        Banana _Banana("./Images/Weapons/banana/banana.png", "./Images/Weapons/banana/wthrban.png");
        ClusterBomb _ClusterBomb("./Images/Weapons/clusterbomb/cluster.png", "./Images/Weapons/clusterbomb/wthrcls.png");

        Pass _Pass;
        Cycle _Cycle;
        Surrender _Surrender;

		al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
        display = al_create_display(width, height);
        if(display == NULL)
        {
            fprintf(stderr, "failed to create display\n");
            return 0;
        }
        // Start Modifyed by Agus PM 12-Apr.2014
        al_set_window_position(display, 0, 0);
        // End Modifyed by Agus PM 12-Apr.2014
        
        al_register_event_source(displayEventQueue,al_get_display_event_source(display));

        Cmap cMap(display,mapData, Data.wmap.map_name, "background2.png", 0);
        cMap.fillMapData(Data.wmap.map_name);
        cMap.WindStrength = Data.wind_strength;
        AirStrike _AirStrike("./Images/Weapons/airstrike/airmisl.png", "./Images/Weapons/airstrike/wairbak.png", &cMap);
        //Cmap cMap(display,mapData, mapData.map_name, "background2.png", 0);
        //cMap.fillMapData(mapData.map_name);

        if(Data.starting_player == I_START)
        {
            cMap.myWormsColor = RED; //siemrpe empieza el rojo
            cMap.enemyWormsColor = BLUE;
            cMap.turn = MY_TURN;
        }
        else
        {
            cMap.myWormsColor = BLUE; //siemrpe empieza el rojo
            cMap.enemyWormsColor = RED;
            cMap.turn = OPPONENT_TURN;
        }


        cMap.createWorms(&_Bazooka);
        cMap.showWorms();

        //cout << cWorm.pCmap << endl;
        //cout << cMap.myWorms[0].pCmap<< endl;

        vector<Cworm>::iterator i;

        if (cMap.turn == MY_TURN)
            i = cMap.myWorms.begin();
        else
            i = cMap.enemyWorms.begin();

        if (i->orientation == LEFT)
            crosshair_angle = 180;
        else
            crosshair_angle = 0;

        al_get_keyboard_state(&keybState);

        al_start_timer(refresh_timer);
        al_start_timer(timeout_timer);
        al_start_timer(block_key_left_arrow);
        al_start_timer(block_key_right_arrow);
        al_start_timer(block_key_tab);
        al_start_timer(cMap.turn_time);
        al_start_timer(cMap.total_time);
        past_refresh_timer = 0;

        do
        {
            // Start Modifyed by Agus PM 05-Mar-2014
        	if(tempSampleInstance && !al_get_sample_instance_playing(tempSampleInstance))
        	{
        		al_destroy_sample_instance(tempSampleInstance);
        		tempSampleInstance = NULL;
        	}
        	if(!al_get_sample_instance_playing(soundtrackInstance))
				al_play_sample_instance(soundtrackInstance);
        	// End Modifyed by Agus PM 05-Mar-2014

            al_get_keyboard_state(&keybState);
            if (cMap.turn == OPPONENT_TURN)
            {
                if (al_get_timer_count(refresh_timer) > past_refresh_timer)
                {
                	// Start Modifyed by Agus PM 12-Apr-2014
					packetType received;
                    received = NBgetPacket(conn, buffer);
                     // End Modifyed by Agus PM 12-Apr-2014
					al_flip_display();
                    past_refresh_timer = al_get_timer_count(refresh_timer);

                    i->updateWormFrame(0);
                    cMap.deleteDrownedWorms();
                    cMap.deleteUnhealthyWorms();
                    
                     // Start Modifyed by Agus PM 12-Apr-2014
					if (received != NOTHING_ON_BUFFER)
                    {
                        if (received == MOVE)
                        {
                            setWorm(buffer[3], i, cMap);
                            temp_point.x = twoCharToInt(buffer[4], buffer[5]);
                            temp_point.y = twoCharToInt(buffer[6], buffer[7]);
                            switch (buffer[1])
                            {
                            case 'I':
                                i->orientation = LEFT;

                                ptoAenviar = i->getWalkDestination();
                                //cout << ptoAenviar.x << " " << ptoAenviar.y << endl;
                                //printf("%x %x\n", buffer[4], buffer[5]);
                                //cout << "Distance: " << getDistanceBetweenPoints(temp_point, ptoAenviar) << endl;

                                if (getDistanceBetweenPoints(temp_point, i->pos) == 0)
                                    ;//cout << i->pos.x << "." << i->pos.y << endl << endl;
                                else if (getDistanceBetweenPoints(temp_point, ptoAenviar) <= ERROR_TOLERANCE)
                                {
                                    i->walk();
                                    //cout << i->pos.x << "." << i->pos.y << endl << endl;
                                    if (i->nextAction == CAIDA_LIBRE)
                                    {
                                        ptoAenviar = i->getShotDestination(0,0);
                                        //if hay ACK, entonces:
                                        i->shotWormNormal(0,0);
                                    }
                                }
                                else
                                {
                                    buffer[0] = ERROR_WORMS;
                                    sendPacket(conn, buffer, 1);
                                }
                                cMap.deleteDrownedWorms();
                                cMap.deleteUnhealthyWorms();

                                ///IF NO ERROR
                                if (cMap.myWorms.empty())
                                    buffer[0] = YOU_WON;
                                else
                                    buffer[0] = ACK;
                                sendPacket(conn, buffer, 1);
                                break;
                            case 'D':
                                i->orientation = RIGHT;
                                ptoAenviar = i->getWalkDestination();
                                if (getDistanceBetweenPoints(temp_point, i->pos) == 0)
                                    ;
                                else if (getDistanceBetweenPoints(temp_point, ptoAenviar) <= ERROR_TOLERANCE)
                                {
                                    i->walk();
                                    if (i->nextAction == CAIDA_LIBRE)
                                    {
                                        ptoAenviar = i->getShotDestination(0,0);
                                        //if hay ACK, entonces:
                                        i->shotWormNormal(0,0);
                                    }
                                }
                                else
                                {
                                    buffer[0] = ERROR_WORMS;
                                    sendPacket(conn, buffer, 1);
                                }
                                cMap.deleteDrownedWorms();
                                cMap.deleteUnhealthyWorms();
                                ///IF NO ERROR
                                if (cMap.myWorms.empty())
                                    buffer[0] = YOU_WON;
                                else
                                    buffer[0] = ACK;
                                sendPacket(conn, buffer, 1);
                                break;
                            }
                        }
                        else if (received == PJUMP)
                        {
                            setWorm(buffer[2], i, cMap);
                            temp_point.x = twoCharToInt(buffer[3], buffer[4]);
                            temp_point.y = twoCharToInt(buffer[5], buffer[6]);
                            ptoAenviar= i->getJumpDestination();
                            if (getDistanceBetweenPoints(temp_point, ptoAenviar) <= ERROR_TOLERANCE)
                            {
                                i->jump();
                                if (i->nextAction == CAIDA_LIBRE)
                                {
                                    ptoAenviar = i->getShotDestination(0,0);
                                    //if hay ACK, entonces:
                                    i->shotWorm(0,0);
                                    i->nextAction = DO_NOTHING;
                                }
                            }
                            else
                            {
                                buffer[0] = ERROR_WORMS;
                                sendPacket(conn, buffer, 1);
                            }

                            cMap.deleteDrownedWorms();
                            cMap.deleteUnhealthyWorms();
                            ///IF NO ERROR
                            if (cMap.myWorms.empty())
                                buffer[0] = YOU_WON;
                            else
                                buffer[0] = ACK;
                            sendPacket(conn, buffer, 1);

                        }
                        else if (received == ATTACK)
                        {
                            setWorm(buffer[2], i, cMap);
                            switch (buffer[3])
                            {
                            case 'b':
                                i->pCweapon = &_Bazooka;
                                i->showLoadBazooka();
                                break;
                            case 'f':
                                i->pCweapon = &_FirePunch;
                                break;
                            case 's':
                                i->pCweapon = &_Shotgun;
                                break;
                            case 'g':
                                i->pCweapon = &_Grenade;
                                break;
                            case 'p':
                                i->pCweapon = &_PneumaticDrill;
                                break;
                            case 'd':
                                i->pCweapon = &_Dynamite;
                                break;
                            case 'm':
                                i->pCweapon = &_Mine;
                                break;
                            case 'u':
                                i->pCweapon = &_Uzi;
                                break;
                            case 'a':
                                i->pCweapon = &_AirStrike;
                                break;
                            case 'n':
                                i->pCweapon = &_Banana;
                                break;
                            case 'v':
                               i->pCweapon = &_ClusterBomb;
                               break;
                            default:
                                cout << "No weapon selected." << endl;
                            }
                            crosshair_angle_enemy = buffer[5];
                            if (i->orientation == LEFT)
                            {
                                if (crosshair_angle_enemy < 0)
                                    crosshair_angle_enemy = -180 - crosshair_angle_enemy;
                                else
                                    crosshair_angle_enemy = 180 - crosshair_angle_enemy;
                            }
                            if (i->pCweapon->weapon == 'a')
                            {
                                temp_point.x = twoCharToInt(buffer[7], buffer[8]);
                                temp_point.y = twoCharToInt(buffer[9], buffer[10]);
                            }
                            i->pCweapon->createShotW(crosshair_angle_enemy, ///es la misma para todas las armas
                                                     buffer[4],//power
                                                     (i->pCweapon->weapon != 'a') ? i->pos : temp_point,      //shooter_pos
                                                     buffer[6],           //fuse
                                                     i->ID);     //shooter_ID
                            if (i->pCweapon->type != FUSE)
                            {
                                if (i->pCweapon->readyToShoot())
                                {
                                        finished_shooting = i->pCweapon->Shoot(&cMap, &(*i));
                                        if (finished_shooting)
                                            i->pCweapon->clearShots();

                                        cMap.deleteDrownedWorms();
                                        cMap.deleteUnhealthyWorms();
                                }

                                ///IF NO ERROR
                                if (cMap.myWorms.empty())
                                {
                                    if ((!cMap.enemyWorms.empty()) || (cMap.enemyWorms.empty() && cMap.myWormsColor == RED))
                                    {
                                        buffer[0] = YOU_WON;
                                        //sendPacket(conn, buffer, 1);
                                        game_status = LOST;
                                    }
                                }
                                else
                                    buffer[0] = ACK;
                                //cout << "Send: ACK" << endl;
                                sendPacket(conn, buffer, 1);
                            }
                            else
                            {
                                buffer[0] = ACK;
                                //cout << "Send: ACK" << endl;
                                sendPacket(conn, buffer, 1);
                            }

                        }
                        else if (received == YOUR_TURN)
                            changeTurnToMy(cMap.turn, cMap.turn_time, buffer[1], cycling_allowed, crosshair_angle, cMap, i);
                        else if (received == YOU_WON)
                        {
                            game_status = WON;
                        }
                        // Start Modifyed by Agus PM 12-Apr-2014
                        else if (received == QUIT)
                        {
                        	//printf("Recibo el paquete QUIT en Opponent's Turn\n");
                        	buffer[0] = ACK;
                        	sendPacket(conn, buffer, 1);
                        	game_status = GAME_OVER;
                        	break;
                        }
                        // End Modifyed by Agus PM 12-Apr-2014
                    }
                     // End Modifyed by Agus PM 12-Apr-2014
                    if ((i->pCweapon->type == FUSE) && (i->pCweapon->Shots.empty()) && (i->pCweapon->number_of_shots == 0))
                    {
                        i->pCweapon->clearShots();
                        i->updateWormFrame(0);
                        al_flip_display();
                        cMap.deleteDrownedWorms();
                        cMap.deleteUnhealthyWorms();
                        if (cMap.myWorms.empty())
                        {
                            if ((!cMap.enemyWorms.empty()) || (cMap.enemyWorms.empty() && cMap.myWormsColor == RED))
                            {
                                buffer[0] = YOU_WON;
                                //cout << "Send: YOU_WON" << endl;
                                sendPacket(conn, buffer, 1);
                                game_status = LOST;
                            }
                        }

                    }
                }
            }
            else
            {
                al_get_keyboard_state(&keybState);
                al_get_mouse_state(&mouse_state);
                cMap.mouse_state = mouse_state;

                if (al_get_timer_count(cMap.turn_time) >= TURN_TIME)
                    changeTurnToYour(cMap.turn, cMap.turn_time, cMap.WindStrength, cMap, i, conn, buffer);
                else if (al_get_timer_count(refresh_timer) > past_refresh_timer)
                {
                    al_flip_display();
                    past_refresh_timer = al_get_timer_count(refresh_timer);

                    i->updateWormFrame(0);
                    if (cMap.deleteDrownedWorms() && (!cMap.myWorms.empty()) && (!cMap.enemyWorms.empty()))
                    {
                        //cout << "linea 405"  << endl;
                        changeTurnToYour(cMap.turn, cMap.turn_time, cMap.WindStrength, cMap, i, conn, buffer);
                    }

                    cMap.deleteUnhealthyWorms();

                    //adjustAngle(crosshair_angle, i->orientation);
                    //cMap.myWorms[0].walk(); //veo si el primer gusano que meti a mano camina

					// Start Modifyed by Agus PM 12-Apr-2014
					if (NBgetPacket(conn,buffer) == QUIT)
					{
						buffer[0] = ACK;
                		sendPacket(conn, buffer, 1);
						game_status = GAME_OVER;
						break;
					}
					// End Modifyed by Agus PM 12-Apr-2014

                    if (!al_key_down(&keybState, ALLEGRO_KEY_TAB))
                        past_block_key_tab = -1;
                    /*if (al_key_down(&keybState, ALLEGRO_KEY_P))
                    {

                    }
                    else */
                    if ((al_key_down(&keybState, ALLEGRO_KEY_TAB) && (cycling_allowed == true)) || checkClick(mouse_state, previous_mouse_state, _Cycle.button_start, _Cycle.button_end,resizeWidth,resizeHeight))
                    {
                        if (al_get_timer_count(block_key_tab) > past_block_key_tab)
                        {
                            past_block_key_tab = al_get_timer_count(block_key_tab);
                            i++;
                            if (i == cMap.myWorms.end())
                                i = cMap.myWorms.begin();
                            adjustAngle(crosshair_angle, i->orientation);
                        }
                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_LEFT))
                    {
                        if (al_get_timer_count(block_key_left_arrow) > past_block_key_left_arrow)
                        {
                            past_block_key_left_arrow = al_get_timer_count(block_key_left_arrow);

                            if ((crosshair_angle < 90) && (crosshair_angle > -90))
                            {
                                if ((crosshair_angle < 90) && (crosshair_angle >= 0))
                                    crosshair_angle = 180 - crosshair_angle;
                                else
                                    crosshair_angle = -180 - crosshair_angle;
                            }

                            if ((getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_LEFT, total_time, previous_time, refresh_timer)
                                    <= CHANGE_ORIENTATION_TOLERANCE) && (i->orientation != LEFT))
                            {
                                createMovePacket('I', i->color, i->ID, i->pos.x, i->pos.y, buffer);
                                //cout << "Send: ORI" << endl;
                                sendPacket(conn, buffer, 8);
                                i->orientation = LEFT; //aca se debe enviar un MOVE con pos y orientation
                            }
                            else
                            {
                                ptoAenviar= i->getWalkDestination();
                                createMovePacket('I', i->color, i->ID, ptoAenviar.x, ptoAenviar.y, buffer);
                                //cout << "Send: MOVEI" << endl;
                                sendPacket(conn, buffer, 8);
                                i->orientation = LEFT;
                                //if hay ACK, entonces:
                                i->walk();
                                if (i->nextAction == CAIDA_LIBRE)
                                {
                                    ptoAenviar = i->getShotDestination(0,0);
                                    //if hay ACK, entonces:
                                    i->shotWormNormal(0,0);
                                }
                            }
                            if (getPacket(conn, HEADER, buffer))
                            {
                                if (buffer[0] == ERROR_WORMS)
                                    cout << "Error in communication" << endl;
                                else if (buffer[0] == YOU_WON)
                                    game_status = WON;
                            }
                            checkAngle(crosshair_angle, i->orientation);
                            cycling_allowed = false;
                        }

                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_RIGHT))
                    {
                        if (al_get_timer_count(block_key_right_arrow) > past_block_key_right_arrow)
                        {
                            past_block_key_right_arrow = al_get_timer_count(block_key_right_arrow);

                            angleOutOfRange(crosshair_angle);

                            if ((getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_RIGHT, total_time, previous_time, refresh_timer)
                                    <= CHANGE_ORIENTATION_TOLERANCE) && (i->orientation != RIGHT))
                            {
                                createMovePacket('D', i->color, i->ID, i->pos.x, i->pos.y, buffer);
                                //cout << "Send: ORD" << endl;
                                sendPacket(conn, buffer, 8);
                                i->orientation = RIGHT;
                            }
                            else
                            {
                                ptoAenviar= i->getWalkDestination();
                                createMovePacket('D', i->color, i->ID, ptoAenviar.x, ptoAenviar.y, buffer);
                                //cout << "Send: MOVED" << endl;
                                sendPacket(conn, buffer, 8);
                                i->orientation = RIGHT;
                                i->walk();
                                //if hay ACK, entonces:
                                if (i->nextAction == CAIDA_LIBRE)
                                {
                                    ptoAenviar = i->getShotDestination(0,0);
                                    //if hay ACK, entonces:
                                    i->shotWormNormal(0,0);
                                }
                            }
                            if (getPacket(conn, HEADER, buffer))
                            {
                                if (buffer[0] == ERROR_WORMS)
                                    cout << "Error in communication" << endl;
                                else if (buffer[0] == YOU_WON)
                                    game_status = WON;
                            }
                            checkAngle(crosshair_angle, i->orientation);
                            cycling_allowed = false;
                        }

                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_ENTER))
                    {
                        ptoAenviar= i->getJumpDestination();
                        createJumpPacket(i->color, i->ID, ptoAenviar.x, ptoAenviar.y, buffer);
                        //cout << "Send: JUMP" << endl;
                        sendPacket(conn, buffer, 7);
                        //if hay ACK, entonces:
                        i->jump();//i->shotWorm(4.5,60);
                        if (i->nextAction == CAIDA_LIBRE)
                        {
                            ptoAenviar = i->getShotDestination(0,0);
                            //if hay ACK, entonces:
                            i->shotWorm(0,0);
                            i->nextAction = DO_NOTHING;
                        }
                        if (getPacket(conn, HEADER, buffer))
                        {
                            if (buffer[0] == ERROR_WORMS)
                                cout << "Error in communication" << endl;
                            else if (buffer[0] == YOU_WON)
                                game_status = WON;
                        }
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Bazooka.button_start, _Bazooka.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Bazooka;

                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {
                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );
                        }
                        // End Modifyed by Agus PM 05-Mar-2014

                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        i->showLoadBazooka();
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _FirePunch.button_start, _FirePunch.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_FirePunch;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Shotgun.button_start, _Shotgun.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Shotgun;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Grenade.button_start, _Grenade.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Grenade;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _PneumaticDrill.button_start, _PneumaticDrill.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_PneumaticDrill;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Dynamite.button_start, _Dynamite.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Dynamite;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Mine.button_start, _Mine.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Mine;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Uzi.button_start, _Uzi.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Uzi;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _AirStrike.button_start, _AirStrike.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_AirStrike;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Banana.button_start, _Banana.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_Banana;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _ClusterBomb.button_start, _ClusterBomb.button_end,resizeWidth,resizeHeight) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        i->pCweapon = &_ClusterBomb;
                        // Start Modifyed by Agus PM 05-Mar-2014
                        if(tempSampleInstance == NULL)
                        {

                        	tempSampleInstance  = al_create_sample_instance(_Shotgun.reloading_sound);
							if (tempSampleInstance)
   						 	{
								al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
                        		al_play_sample_instance(tempSampleInstance);
								//al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    						}
    						else
    							printf( "Error con sonido tempSampleInstance not created!\n" );

                        }
                        // End Modifyed by Agus PM 05-Mar-2014
                        //al_play_sample(_Shotgun.reloading_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        cycling_allowed = false;
                    }
                    else if (checkClick(mouse_state, previous_mouse_state, _Pass.button_start, _Pass.button_end,al_get_display_width(display),al_get_display_height(display)))
                    {
                        changeTurnToYour(cMap.turn, cMap.turn_time, cMap.WindStrength, cMap, i, conn, buffer);
                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_Q) || checkClick(mouse_state, previous_mouse_state, _Surrender.button_start, _Surrender.button_end,al_get_display_width(display),al_get_display_height(display)))
                    {
                        buffer[0] = YOU_WON;
                        //cout << "Send: YOU_WON" << endl;
                        sendPacket(conn, buffer, 1);
                        game_status = LOST;
                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_DOWN) && !al_key_down(&keybState,ALLEGRO_KEY_SPACE))
                    {
                        moveAngleDown(crosshair_angle, i->orientation);
                        checkAngle(crosshair_angle, i->orientation);
                    }
                    else if (al_key_down(&keybState,ALLEGRO_KEY_UP) && !al_key_down(&keybState,ALLEGRO_KEY_SPACE))
                    {
                        moveAngleUp(crosshair_angle, i->orientation);
                        checkAngle(crosshair_angle, i->orientation);
                    }
                    else if (selectFuse(&keybState))
                        fuse = selectFuse(&keybState);
                    else if (al_key_down(&keybState,ALLEGRO_KEY_SPACE) && (!((i->pCweapon->type == FUSE) && (!i->pCweapon->Shots.empty()))))
                    {
                        if ((dont_play_twice) && (i->pCweapon->weapon != 's'))
                        {
                            // Start Modifyed by Agus PM 05-Mar-2014
                        	if(tempSampleInstance == NULL)
		                    {
		                    	tempSampleInstance  = al_create_sample_instance(weapon_powerup);
								if (tempSampleInstance)
							 	{
									al_attach_sample_instance_to_mixer(tempSampleInstance, al_get_default_mixer());
		                    		al_play_sample_instance(tempSampleInstance);
									//al_play_sample(weapon_powerup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								}
								else
									printf( "Error con sonido tempSampleInstance not created!\n" );
		                    }
		                    // End Modifyed by Agus PM 05-Mar-2014
		                    //al_play_sample(weapon_powerup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }

                        dont_play_twice = false;
                        al_draw_bitmap_region(loading_bar,
                                              0,
                                              0,
                                              POWER_BAR_SPEED*getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_SPACE, total_time, previous_time, refresh_timer) <= 388 ?
                                              POWER_BAR_SPEED*getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_SPACE, total_time, previous_time, refresh_timer) : 388,
                                              50,
                                              1496 + 8,
                                              708 + 19 + 10 ,
                                              0);
                        past_TotalTimeKey = getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_SPACE, total_time, previous_time, refresh_timer);
                    }
                    if (past_TotalTimeKey > getTotalTimeKey(&previous_keyboard, ALLEGRO_KEY_SPACE, total_time, previous_time, refresh_timer))
                    {
                        if (i->pCweapon->weapon != 's')
                        {
                            //al_stop_sample(&sample_id_soundtrack);
                            //al_stop_sample(&sample_id);
                            
							// Start Modifyed by Agus PM 12-Apr-2014
							if(tempSampleInstance != NULL)
							{
								if(al_get_sample_instance_playing(tempSampleInstance))
									al_stop_sample_instance(tempSampleInstance);
								al_destroy_sample_instance(tempSampleInstance);
								tempSampleInstance = NULL;
							}
                            // End Modifyed by Agus PM 12-Apr-2014
                        }

                        dont_play_twice = true;
                        if (i->pCweapon == NULL)
                            cout << "No weapon selected" << endl;
                        else
                        {
                            if (i->pCweapon->weapon == 'a')
                            {
                                do
                                {
                                    al_get_mouse_state(&mouse_state);
                                    if ((mouse_state.x >= 0) && (mouse_state.x < WIDTH) && (mouse_state.y >= 0) && (mouse_state.y <= 696))
                                    {
                                        cMap.showWorms();
                                        al_draw_bitmap(i->color == 'b' ? _AirStrike.marker_b : _AirStrike.marker_r, mouse_state.x - 30, mouse_state.y - 30, 0);
                                        al_flip_display();
                                        al_rest(FRAMERATE);
                                    }
                                    temp_point.x = mouse_state.x;
                                    temp_point.y = mouse_state.y;
                                } while (!(mouse_state.buttons & 1));
                            }

                            i->pCweapon->createShotW(crosshair_angle,
                                                     getPower(past_TotalTimeKey),//power
                                                     (i->pCweapon->weapon != 'a') ? i->pos : temp_point, //shooter_pos
                                                     fuse,           //fuse
                                                     i->ID);     //shooter_ID
                            if (i->pCweapon->readyToShoot())
                            {
                                if (i->pCweapon->type != FUSE)
                                {
                                    if (crosshair_angle > 90)
                                        temp_angle = 180 - crosshair_angle;
                                    else if (crosshair_angle < -90)
                                        temp_angle = -(180 + crosshair_angle);
                                    else
                                        temp_angle = crosshair_angle;

                                    createAttackPacket(i->color, i->ID, i->pCweapon->weapon, getPower(past_TotalTimeKey), temp_angle, fuse, mouse_state.x, mouse_state.y, buffer);
                                    sendData(conn, buffer, 11);

                                    finished_shooting = i->pCweapon->Shoot(&cMap, &(*i));
                                    if (finished_shooting)
                                        i->pCweapon->clearShots();

                                    i->updateWormFrame(0);
                                    al_flip_display();
                                    if (getPacket(conn, HEADER, buffer))
                                    {
                                        if (buffer[0] == ERROR_WORMS)
                                            cout << "Error in communication" << endl;
                                        else if (buffer[0] == YOU_WON)
                                            game_status = WON;
                                    }
                                    cycling_allowed = false;
                                    ///IF FINISHED MOVING
                                    cMap.deleteDrownedWorms();
                                    cMap.deleteUnhealthyWorms();
                                    if (cMap.myWorms.empty() && (game_status != LOST))
                                    {
                                        if ((!cMap.enemyWorms.empty()) || (cMap.enemyWorms.empty() && cMap.myWormsColor == RED))
                                        {
                                            buffer[0] = YOU_WON;
                                            //cout << "Send: YOU_WON" << endl;
                                            sendPacket(conn, buffer, 1);
                                            game_status = LOST;
                                        }
                                    }
                                    else
                                    {
                                        if ((finished_shooting == true) && (!cMap.enemyWorms.empty()) && (!cMap.myWorms.empty()))
                                        {
                                            //cout << "linea 677"  << endl;
                                            changeTurnToYour(cMap.turn, cMap.turn_time, cMap.WindStrength, cMap, i, conn, buffer);
                                        }

                                    }
                                }
                                else if ((i->pCweapon->type == FUSE) && (i->pCweapon->number_of_shots != 0))
                                {
                                    if (crosshair_angle > 90)
                                        temp_angle = 180 - crosshair_angle;
                                    else if (crosshair_angle < -90)
                                        temp_angle = -(180 + crosshair_angle);
                                    else
                                        temp_angle = crosshair_angle;

                                    createAttackPacket(i->color, i->ID, i->pCweapon->weapon, getPower(past_TotalTimeKey), temp_angle, fuse, 1, 1, buffer);
                                    sendData(conn, buffer, 11);
                                    if (getPacket(conn, HEADER, buffer))
                                    {
                                        if (buffer[0] == ERROR_WORMS)
                                            cout << "Error in communication" << endl;
                                        else if (buffer[0] == YOU_WON)
                                            game_status = WON;
                                    }
                                }
                            }
                        }
                        past_TotalTimeKey = 0;
                    }

                    previous_keyboard = keybState;
                    previous_mouse_state = mouse_state;

                    al_draw_bitmap(i->color == BLUE ? crosshair_image_b : crosshair_image_r,
                                   i->pos.x + CROSSHAIR_RADIUS * cos(crosshair_angle * PI / 180.0),
                                   i->pos.y - CROSSHAIR_RADIUS * sin(crosshair_angle * PI / 180.0),
                                   0);

                    if ((i->pCweapon->type == FUSE) && (i->pCweapon->Shots.empty()) && (i->pCweapon->number_of_shots == 0))
                    {
                        i->pCweapon->clearShots();
                        i->updateWormFrame(0);
                        al_flip_display();
                        cMap.deleteDrownedWorms();
                        cMap.deleteUnhealthyWorms();
                        cycling_allowed = false;
                        if (cMap.myWorms.empty() && (game_status != LOST))
                        {
                            if ((!cMap.enemyWorms.empty()) || (cMap.enemyWorms.empty() && cMap.myWormsColor == RED))
                            {
                                buffer[0] = YOU_WON;
                                //cout << "Send: YOU_WON" << endl;
                                sendPacket(conn, buffer, 1);
                                game_status = LOST;
                            }
                        }
                        else
                        {
                            if ((!cMap.myWorms.empty()) && (!cMap.enemyWorms.empty()))
                                changeTurnToYour(cMap.turn, cMap.turn_time, cMap.WindStrength, cMap, i, conn, buffer);
                        }

                    }
                }
                //al_draw_filled_ellipse(i->pos.x, i->pos.y, 3, 3, al_map_rgb(255,0,0));
                //al_draw_filled_ellipse(i->realPosX(), i->realPosY(), 3, 3, al_map_rgb(0,0,0));

            }


            while(!al_is_event_queue_empty(displayEventQueue))
            {
            	ALLEGRO_EVENT displayEvent;
            	al_get_next_event(displayEventQueue,&displayEvent);
            	if(displayEvent.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
            	{
					 resizeWidth  = displayEvent.display.width;
            		 resizeHeight = displayEvent.display.height;

            		 cMap.resizeWidth  = displayEvent.display.width;
            		 cMap.resizeHeight = displayEvent.display.height;
            	}
            	
            	// Start Modifyed by Agus PM 12-Apr-2014
            	if(displayEvent.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            	{
                	buffer[0] = QUIT;
                	sendPacket(conn, buffer, 1);
					getPacket(conn, ACK, buffer);
					if(buffer[0] != ACK)
        				cout <<"Communications Error" << endl; 
                	game_status = GAME_OVER;
                	break;
            	}
            	// End Modifyed by Agus PM 12-Apr-2014
            }

            al_flip_display();
            if (cMap.myWorms.empty() && (game_status != LOST))
            {
                buffer[0] = YOU_WON;
                //cout << "Send: YOU_WON" << endl;
                sendPacket(conn, buffer, 1);
                game_status = LOST;
            }
            
			// Start Modifyed by Agus PM 12-Apr-2014
			if (al_key_down(&keybState,ALLEGRO_KEY_ESCAPE))
            {
                	buffer[0] = QUIT;
                	sendPacket(conn, buffer, 1);
					getPacket(conn, ACK, buffer);
					if(buffer[0] != ACK)
        				cout <<"Communications Error" << endl; 
                	game_status = GAME_OVER;
            }
        	// End Modifyed by Agus PM 12-Apr-2014
            
			///Modificacion para la feria
            /*
            if (al_get_timer_count(cMap.total_time) >= TOTAL_GAME_TIME)
            {
                cout << "Time's up!" << endl;
                game_status = whoWonByHealth(cMap.myWorms, cMap.enemyWorms);
            }*/
        }
        while (game_status == ONGOING);

        //al_stop_sample(&sample_id_soundtrack);


        cout << endl << endl;

        if (game_status == WON)
        {
            al_draw_bitmap(you_win, 0, 0, 0);
            al_flip_display();
            al_rest(5);
            al_destroy_display(cMap.display);
            cout << "You win!" << endl;
            cout << "Play again? y/n" << endl;
            Data.starter = false;
            fflush(stdin);
            c = getchar();
            fflush(stdin);
            if ((c == 'y') || (c == 'Y'))
            {
                buffer[0] = PLAY_AGAIN;
                sendPacket(conn, buffer, 1);
                getPacket(conn, HEADER, buffer);
                if (buffer[0] == PLAY_AGAIN)
                    game_status = PLAY_AGAIN;
                else
                {
                    //printf("--%x--\n", buffer[0]);
                    cout << "The other player cowardly refused another game." << endl;
                    game_status = GAME_OVER;
                }
            }
            else
            {
                buffer[0] = GAME_OVER;
                sendPacket(conn, buffer, 1);
                game_status = GAME_OVER;
            }
        }
        else if (game_status == LOST)
        {
            al_draw_bitmap(you_lose, 0, 0, 0);
            al_flip_display();
            al_rest(5);
            al_destroy_display(cMap.display);
            cout << "You lose!" << endl;
            Data.starter = true;
            getPacket(conn, HEADER, buffer);
            //printf("--%x--", buffer[0]);
            if (buffer[0] == PLAY_AGAIN)
            {
                cout << "Play again? y/n" << endl;
                fflush(stdin);
                c = getchar();
                fflush(stdin);
                if ((c == 'y') || (c == 'Y'))
                {
                    buffer[0] = PLAY_AGAIN;
                    sendPacket(conn, buffer, 1);
                    game_status = PLAY_AGAIN;
                }

                else
                {
                    buffer[0] = GAME_OVER;
                    sendPacket(conn, buffer, 1);
                    game_status = GAME_OVER;
                }
            }
            else
                game_status = GAME_OVER;
        }
        
        // Start Modifyed by Agus PM 12-Apr-2014
       /*else if(game_status == GAME_OVER)
        {
			printf("Llego a game_status == GAME_OVER\n");
        	system("PAUSE");
        }*/
        // End Modifyed by Agus PM 12-Apr-2014
    }
    else
    {
        Data.error = true;
        printf("Some issue occurred with Allegro's timers or an image file is missing.\n");
    }



    al_destroy_timer(refresh_timer);
    al_destroy_timer(timeout_timer);
    al_destroy_timer(block_key_left_arrow);
    al_destroy_timer(block_key_right_arrow);
    al_destroy_timer(block_key_tab);

    al_destroy_bitmap(you_lose);
    al_destroy_bitmap(you_win);

    // Start Modifyed by Agus PM 05-Mar-2014
    if(tempSampleInstance != NULL)
		al_destroy_sample_instance(tempSampleInstance);
	al_destroy_sample_instance(soundtrackInstance);
    // End Modifyed by Agus PM 05-Mar-2014

    return game_status;
}

int64_t getTotalTimeKey(ALLEGRO_KEYBOARD_STATE* previous_keyboard, int keycode, int64_t& total_time, int64_t& previous_time, ALLEGRO_TIMER* refresh_timer)
{
    if (al_key_down(previous_keyboard, keycode))
    {
        total_time += al_get_timer_count(refresh_timer) - previous_time;
        previous_time = al_get_timer_count(refresh_timer);
    }
    else
    {
        total_time = 0;
        previous_time = al_get_timer_count(refresh_timer);
    }
    return total_time;
}

char getPower(int64_t past_TotalTimeKey)
{
    if ((POWER_BAR_SPEED * past_TotalTimeKey) < 84)
        return 1;
    else if ((POWER_BAR_SPEED * past_TotalTimeKey) < 168)
        return 2;
    else if ((POWER_BAR_SPEED * past_TotalTimeKey) < 252)
        return 3;
    else if ((POWER_BAR_SPEED * past_TotalTimeKey) < 336)
        return 4;
    else
        return 5;
}

void moveAngleDown(int& crosshair_angle, int orientation)
{
    if ((crosshair_angle < 90) && (crosshair_angle > -90))
        crosshair_angle -= CROSSHAIR_SPEED;
    else if ((crosshair_angle > 90) || (crosshair_angle < -90))
        crosshair_angle += CROSSHAIR_SPEED;
    else if (orientation == RIGHT)
        crosshair_angle -= CROSSHAIR_SPEED;
    else
        crosshair_angle += CROSSHAIR_SPEED;
}

void moveAngleUp(int& crosshair_angle, int orientation)
{
    if ((crosshair_angle < 90) && (crosshair_angle > -90))
        crosshair_angle += CROSSHAIR_SPEED;
    else if ((crosshair_angle > 90) || (crosshair_angle < -90))
        crosshair_angle -= CROSSHAIR_SPEED;
    else if (orientation == RIGHT)
        crosshair_angle += CROSSHAIR_SPEED;
    else
        crosshair_angle -= CROSSHAIR_SPEED;
}

//Checking for crosshair_angle inconsistencies:
void checkAngle(int& crosshair_angle, int orientation)
{

    if (crosshair_angle == 182)
        crosshair_angle = -180;
    else if (crosshair_angle == -182)
        crosshair_angle = 180;
    if ((orientation == RIGHT) && (crosshair_angle > 90))
        crosshair_angle = 90;
    else if ((orientation == RIGHT) && (crosshair_angle < -90))
        crosshair_angle = -90;
    else if ((orientation == LEFT) && (crosshair_angle == 88))
        crosshair_angle = 90;
    else if ((orientation == LEFT) && (crosshair_angle == -88))
        crosshair_angle = -90;
}

void adjustAngle(int& crosshair_angle, int orientation)
{
    if (orientation == LEFT)
        crosshair_angle = 180;
    else
        crosshair_angle = 0;
}

void angleOutOfRange(int& crosshair_angle)
{
    if (!((crosshair_angle < 90) && (crosshair_angle > -90)))
    {
        if ((crosshair_angle >= 90) && (crosshair_angle <= 180))
            crosshair_angle = 180 - crosshair_angle;
        else
            crosshair_angle = -180 - crosshair_angle;
    }
}

void changeTurnToMy(char& turn, ALLEGRO_TIMER* turn_time, char WindStrength, bool& cycling_allowed, int& angle, Cmap& cMap, vector<Cworm>::iterator& i)
{
    turn = MY_TURN;
    al_set_timer_count(cMap.turn_time, 0);
    cMap.WindStrength = WindStrength;
    cycling_allowed = true;
    i = cMap.myWorms.begin();
    adjustAngle(angle, i->orientation);
}

void changeTurnToYour(char& turn, ALLEGRO_TIMER* turn_time, char& WindStrength, Cmap& cMap, vector<Cworm>::iterator& i, PolonetConn conn, char* buffer)
{
    turn = OPPONENT_TURN;
    WindStrength = getWindStrength();
    al_set_timer_count(turn_time, 0);
    //i->pCweapon->clearShots();
    i = cMap.enemyWorms.begin();

    buffer[0] = YOUR_TURN;
    buffer[1] = WindStrength;
    sendPacket(conn, buffer, 2);
}

void setWorm(int worm_ID, vector<Cworm>::iterator& i, Cmap& cMap)
{
    for (i = cMap.enemyWorms.begin(); i != cMap.enemyWorms.end(); i++)
    {
        if (i->ID == worm_ID)
            return;
    }
}

int whoWonByHealth(vector<Cworm>& myWorms, vector<Cworm>& enemyWorms)
{
    vector<Cworm>::iterator i;
    int my_total_health = 0;
    int enemy_total_health = 0;

    for (i = myWorms.begin(); i != myWorms.end(); i++)
        my_total_health += i->health;
    for (i = enemyWorms.begin(); i != enemyWorms.end(); i++)
        enemy_total_health += i->health;

    if (my_total_health > enemy_total_health)
        return WON;
    else if (my_total_health < enemy_total_health)
        return LOST;
    else
    {
        i = myWorms.begin();
        if (i->color == BLUE)
            return WON;
        else
            return LOST;
    }
}

char selectFuse(ALLEGRO_KEYBOARD_STATE* keybState)
{
    if (al_key_down(keybState, ALLEGRO_KEY_1) || al_key_down(keybState, ALLEGRO_KEY_PAD_1))
        return 1;
    else if (al_key_down(keybState, ALLEGRO_KEY_2) || al_key_down(keybState, ALLEGRO_KEY_PAD_2))
        return 2;
    else if (al_key_down(keybState, ALLEGRO_KEY_3) || al_key_down(keybState, ALLEGRO_KEY_PAD_3))
        return 3;
    else if (al_key_down(keybState, ALLEGRO_KEY_4) || al_key_down(keybState, ALLEGRO_KEY_PAD_4))
        return 4;
    else if (al_key_down(keybState, ALLEGRO_KEY_5) || al_key_down(keybState, ALLEGRO_KEY_PAD_5))
        return 5;
    else
        return 0;
}

bool checkClick(ALLEGRO_MOUSE_STATE& mouse_state, ALLEGRO_MOUSE_STATE& previous_mouse_state, Point start, Point end, double dispWidth,double dispHeight)
{

	double resizeRelationWidth = dispWidth/WIDTH;
	double resizeRelationHeight =dispHeight/HEIGHT;

    if ((!(mouse_state.buttons & 1)) && (previous_mouse_state.buttons & 1))
    {
        if ((mouse_state.x >= start.x*resizeRelationWidth) &&
            (mouse_state.x <= end.x*resizeRelationWidth) &&
            (mouse_state.y >= start.y*resizeRelationHeight) &&
            (mouse_state.y <= end.y*resizeRelationHeight))
            return true;
    }
    return false;
}

void drawButtons(ALLEGRO_MOUSE_STATE& mouse_state, Cmap& map, Cworm& worm, double dispWidth, double dispHeight)
{
    int x = 413;
    int y = 711;
    double resizeRelationWidth = dispWidth/WIDTH;
	double resizeRelationHeight = dispHeight/HEIGHT;
	int xResized = 	x * resizeRelationWidth;
	int yResized = y * resizeRelationHeight;

    int i, j;
    Point temp_start;
    Point temp_end;
    bool flag = false;
    al_draw_bitmap(map.bottom,0, al_get_bitmap_height(map.mapa),0);
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 11; j++)
        {
            if ((mouse_state.buttons & 1) && (mouse_state.x >= xResized + j * 75 * resizeRelationWidth) && (mouse_state.y >= yResized + i * 75 * resizeRelationHeight)
                && (mouse_state.x <= xResized + (j + 1) * 75 * resizeRelationWidth) && (mouse_state.y <= yResized + (i + 1) * 75 * resizeRelationHeight))
            {
                al_draw_bitmap(map.button_pressed, x + j * 75, y + i * 75, 0);
                temp_start.x = x + j * 75;
                temp_start.y = y + i * 75;
                temp_end.x = x + (j + 1) * 75;
                temp_end.y = y + (i + 1) * 75;
                flag = true;
            }
            else if ((worm.pCweapon->number.x == j) && (worm.pCweapon->number.y == i))
                al_draw_bitmap(map.button_selected, x + j * 75, y + i * 75, 0);
            else
                al_draw_bitmap(map.button, x + j * 75, y + i * 75, 0);
        }
    }
    al_draw_bitmap(map.weapon_bar,0, al_get_bitmap_height(map.mapa),0);
    if (flag)
        al_draw_filled_rectangle(temp_start.x, temp_start.y, temp_end.x, temp_end.y, al_map_rgba(0, 0, 0, 100));


}

bool initAll(ALLEGRO_DISPLAY** display)
{
    if(!al_init())
    {
        fprintf(stderr, "failed to initialize allegro\n");
        return 0;
    }
    if(!al_init_primitives_addon())
    {
        fprintf(stderr, "failed to initialize primitives addon\n");
        return 0;
    }
    if(!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize image addon\n");
        al_shutdown_primitives_addon();
        return 0;
    }
    al_init_font_addon();
    if(!al_init_ttf_addon())
    {
        fprintf(stderr, "failed to initialize ttf addon\n");
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        al_shutdown_primitives_addon();
        return 0;
    }
    if(!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize keyboard driver\n");
        al_shutdown_ttf_addon();
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        al_shutdown_primitives_addon();
        return 0;
    }
    int width = WIDTH;
    int height = HEIGHT;
    *display = al_create_display(width, height);
    if(*display == NULL)
    {
        fprintf(stderr, "failed to create display\n");
        al_uninstall_keyboard();
        al_shutdown_ttf_addon();
        al_shutdown_font_addon();
        al_shutdown_image_addon();
        al_shutdown_primitives_addon();
        return 0;
    }
    return true;
    /* Varios
    ALLEGRO_BITMAP* piso_sucio = NULL;
    ALLEGRO_FONT* bloodlust = NULL;
    ALLEGRO_SAMPLE*soundtrack = NULL;
    ALLEGRO_TIMER* countDownTimer = NULL;
    int64_t pastCountDownCount=0;

    bloodlust = al_load_font("bloodlust.ttf", 100, 0);
    */
}

void uninstallAll(ALLEGRO_DISPLAY *display)
{
    al_destroy_display(display);
    al_uninstall_keyboard();
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_shutdown_image_addon();
    al_shutdown_primitives_addon();

}

/*Sound
        if(!al_install_audio())
        {
            fprintf(stderr, "failed to initialize audio!\n");
            return -1;
        }

        if(!al_init_acodec_addon())
        {
            fprintf(stderr, "failed to initialize audio codecs!\n");
            return -1;
        }

        if (!al_reserve_samples(1))
        {
            fprintf(stderr, "failed to reserve samples!\n");
            return -1;
        }

        soundtrack = al_load_sample( "Rocky_soundtrack.wav" );

        if (!soundtrack)
        {
            printf( "Audio clip sample not loaded!\n" );
            return -1;
        }
        al_play_sample(soundtrack, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);


*/
