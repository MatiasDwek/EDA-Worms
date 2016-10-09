#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <conio.h>
#include "../../polonet.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "../Map_files/map.h"
#include "initConn.h"
#include "server-client.h"


bool getHostName(string& hostName)
{
    int i = 0;
    char c;

    while (((c = getchar()) != '\n') && (i < 15))
    {
        hostName[i++] = c;
        if (((c < 46) || (c > 57)) || (c == 47))
            return false;
    }

    if (i > 15)
        return false;
    hostName[i] = 0;
    return true;
}

connectionStatus becomeClient(PolonetConn& conn, string hostName)
{
    ALLEGRO_TIMER* connectionTimer = NULL;
    int conn_wait;
    connectionStatus Status = CLIENT;
    connectionTimer = al_create_timer(0.001);

    if (connectionTimer != NULL)
    {
        al_start_timer(connectionTimer);
        conn_wait = random_btwn(MIN_WAIT, MAX_WAIT);
        //conn_wait = random_btwn(20, 50) * 300;
        //cout << conn_wait << endl;
        do
        {
            conn = openConnection(hostName.c_str(), PORT_COMM);
            if (conn)
            {
                while (isPending(conn) && (al_get_timer_count(connectionTimer) < conn_wait) && (al_get_timer_count(connectionTimer) % 50))
                    ;

                if(isConnected(conn))
                    Status = CLIENT;
                else
                {
                    closeConnection(conn);
                    Status = SERVER;
                }

            }
            else
                Status = SERVER;

            if (kbhit())
            {
                Status = ABORTED;
                fflush(stdin);
            }

        }
        while ((al_get_timer_count(connectionTimer) <= conn_wait) && (Status != ABORTED) && (Status != CLIENT));
    }
    else
        Status = CONN_ERROR;

    al_destroy_timer(connectionTimer);
    return Status;
}

connectionStatus becomeServer(PolonetConn& conn, string hostName)
{
    ALLEGRO_TIMER* connectionTimer = NULL;
    ALLEGRO_TIMER* waitTimer = NULL;
    connectionStatus Status = SERVER;
    connectionTimer = al_create_timer(0.001);
    waitTimer = al_create_timer(1);
    if ((connectionTimer != NULL) && (waitTimer != NULL))
    {
        al_start_timer(waitTimer);
        while (al_get_timer_count(waitTimer) < 2)   //To prevent connection with itself, weird issue when testing on the same pc
            ;

        al_start_timer(connectionTimer);
        if (startListening(PORT_COMM))
        {
            cout << "Server waiting..." << endl;
            while ((!(conn = getAvailableConnection())) && (Status == SERVER))
            {
                if (kbhit())
                {
                    Status = ABORTED;
                    fflush(stdin);
                }
                if (al_get_timer_count(connectionTimer) > SERVER_WAIT)
                    Status = CONN_ERROR;
            }

            if ((Status != CONN_ERROR) && (Status != ABORTED))
            {
                while (isPending(conn) && (Status == SERVER))
                {
                    if (kbhit())
                    {
                        Status = ABORTED;
                        fflush(stdin);
                    }

                    if (al_get_timer_count(connectionTimer) > SERVER_WAIT)
                        Status = CONN_ERROR;
                }
            }
        }
        else
            Status = CONN_ERROR;
    }
    else
        Status = CONN_ERROR;
    return Status;
}


/*
int getData(PolonetConn conn, char *buffer, int buffersize)
{
        while (isConnected(conn))
        {
                int bytesReceived;

                if (bytesReceived = receiveData(conn, buffer, buffersize))
                        return bytesReceived;

                // Wait 10 milliseconds, so the CPU is not clogged
                usleep(10000);
        }

        return 0;
}

        char *temp_buffer;
        temp_buffer = (char*) malloc(MAX_PACKET_SIZE);
        memset(temp_buffer, 0, MAX_PACKET_SIZE);

        while (isConnected(conn) && (receiveData(conn, temp_buffer, MAX_PACKET_SIZE) == 0))
            usleep(10000);

        while (isConnected(conn) &&  ())
        {
            strcat(buffer, temp_buffer);
            memset(temp_buffer, 0, MAX_PACKET_SIZE);
            receiveData(conn, temp_buffer, MAX_PACKET_SIZE);
        }

        free(temp_buffer);


int sendData(PolonetConn conn, char *buffer, int buffersize)
{
    int bytesSent = 0;
    while (bytesSent < buffersize)
    {
        if (isConnected(conn))
        {

        }
        else
            return bytesSent;
    }

                int bytesSent;

                if (bytesSent = receiveData(conn, buffer, buffersize))
                        return bytesReceived;

                //Wait 10 milliseconds, so the CPU is not clogged
                usleep(10000);
        }

        return 0;
}
*/

/*
int getKeyPressed(ALLEGRO_KEYBOARD_STATE* keybState)
{
    for (int i = ALLEGRO_KEY_A; i < ALLEGRO_KEY_MAX; i++)
        if (al_key_down(keybState,ALLEGRO_KEY_ESCAPE))
            return i;
    return 0;
}

    ALLEGRO_KEYBOARD_STATE keybState;
    ALLEGRO_FONT* FranklinGothic = NULL;
    int i = 0;

    FranklinGothic = al_load_font("./Fonts/ITCFranklinGothicStd-DmCd.ttf", 100, 0);

    if (FranklinGothic != NULL)
    {
        al_draw_text(FranklinGothic,al_map_rgb(0,0,0),WIDTH/2,HEIGHT/2,
                     ALLEGRO_ALIGN_CENTRE,"Write IP adress of host:");
        al_flip_display();

        al_get_keyboard_state(&keybState);
        while ((getKeyPressed(&keybState) != ALLEGRO_KEY_ENTER) && (i < 15))
        {
            if (!getKeyPressed(&keybState))
            {
                if ((getKeyPressed(&keybState) >= ALLEGRO_KEY_0) && ()
                hostName[i] =
            }
        }


        return true;
    }
*/

