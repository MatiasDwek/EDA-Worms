#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <conio.h>
#include "../polonet.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../Back_End/Map_files/map.h"
#include "../Back_End/Communication_files/initConn.h"
#include "../Back_End/Communication_files/server-client.h"
#include "runGame.h"
#include "../Back_End/Communication_files/play_again.h"

int main()
{
    srand(time(NULL));
    connectionStatus currentStatus = CLIENT;
    initData Data;

    if (al_init()) {
    if (al_init_primitives_addon()) {
    if (al_init_image_addon()) {
        al_init_font_addon();
    if (al_init_ttf_addon()) {
    if (al_install_keyboard()) {

    if(!al_install_mouse())
    {
        fprintf(stderr, "failed to initialize mouse!\n");
        return -1;
    }

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

	// Start Modifyed by Agus PM 05-Mar-2014
    if (!al_reserve_samples(2))
    {
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }
    // End Modifyed by Agus PM 05-Mar-2014

/*
        Data.wind_strength = getWindStrength();
        Data.starting_player = I_START;
        runGame(1, Data);
        return 0;
*/





        PolonetConn conn;
        cout << "Welcome to EDA Worms!" << endl;
        al_rest(1);
        cout << "Establishing connection..." << endl;
        cout << "Write host name:" << endl;
        string hostName;

        if (getHostName(hostName))
        {
            cout << "Connecting as client to " << hostName.c_str() << "..." << endl;
            cout << "Press any key to cancel and establish server." << endl;
            currentStatus = becomeClient(conn, hostName);

            if (currentStatus != CONN_ERROR)
            {

                if (currentStatus == CLIENT)
                {
                    cout << "Successfully connected as client." << endl;
                    Data = clientSequence(conn);
                    if(Data.error == false)
                    {
                        cout << "Game starting..." << endl;
                        while (Data.error != true)
                        {
                            cout << "Game starting in: ";
                            cout << "3... ";
                            al_rest(1);
                            cout << "2... ";
                            al_rest(1);
                            cout << "1..." << endl;
                            al_rest(1);
                            system("cls\n");
                            if (runGame(conn, Data) == PLAY_AGAIN)
                            {
                                cout << "Game starting in: ";
                                cout << "3... ";
                                al_rest(1);
                                cout << "2... ";
                                al_rest(1);
                                cout << "1..." << endl;
                                al_rest(1);
                                playAgainSequence(conn, Data);
                                system("cls\n");
                            }
                            else
                                break;
                        }

                    }

                    else
                        cout << "Error in connection sequence." << endl;
                }
                else
                {
                    cout << "Establishing server..." << endl;
                    cout << "Press any key to cancel and close program." << endl;
                    currentStatus = becomeServer(conn, hostName);
                    if (currentStatus != CONN_ERROR)
                    {
                        if (currentStatus != ABORTED)
                        {
                            cout << "Successfully connected as server." << endl;
                            Data = serverSequence(conn);
                            if(Data.error == false)
                            {
                                cout << "Game starting..." << endl;
                                while (Data.error != true)
                                {
                                    cout << "Game starting in: ";
                                    cout << "3... ";
                                    al_rest(1);
                                    cout << "2... ";
                                    al_rest(1);
                                    cout << "1..." << endl;
                                    al_rest(1);
                                    system("cls\n");
                                    if (runGame(conn, Data) == PLAY_AGAIN)
                                    {
                                        cout << "Game starting in: ";
                                        cout << "3... ";
                                        al_rest(1);
                                        cout << "2... ";
                                        al_rest(1);
                                        cout << "1... " << endl;
                                        al_rest(1);
                                        playAgainSequence(conn, Data);
                                        system("cls\n");
                                    }
                                    else
                                        break;
                                }
                            }
                            else
                                cout << "Error in connection sequence." << endl;
                        }
                        else
                            fprintf(stderr, "Aborted by user. Closing...\n");
                    }
                    else
                        fprintf(stderr, "Connection failed. Server could not be loaded properly or wait time exceeded.\n");
                }
                closeConnection(conn);
                if (currentStatus == SERVER)
                    stopListening();
            }
            else
                fprintf(stderr, "failed to initialize timers.\n");
        }
        else
            fprintf(stderr, "failed in hostname input.\n");
        al_uninstall_keyboard();
    } else
        fprintf(stderr, "failed to intall a keyboard driver.\n");
        al_shutdown_font_addon();
        al_shutdown_ttf_addon();
    } else
        fprintf(stderr, "failed to initialize ttf addon.\n");
        al_shutdown_image_addon();
    } else
        fprintf(stderr, "failed to initialize image addon.\n");
        al_shutdown_primitives_addon();
    } else
        fprintf(stderr, "failed to initialize primitives addon.\n");
    } else
        fprintf(stderr, "failed to initialize allegro.\n");

    cout << endl << "Program finished. Press enter to exit." << endl;
    getchar();

    return 0;
}
