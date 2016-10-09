#include <cstring>
#include <cstdio>
#include<iostream>

#include "play_again.h"
#include "server-client.h"
#include "../../polonet.h"

using namespace std;



void playAgainSequence(PolonetConn conn, initData& Data)
{
    char buffer[MAX_PACKET];
    memset(buffer, 0, MAX_PACKET);
    char buffer_for_map[MAX_MAP_NAME];
    memset(buffer_for_map, 0, MAX_MAP_NAME);
    int map_number;
    char temp_number[5];
    Data.error = false;
    char* temp_packet = (char*) malloc(MAX_PACKET);

    if (Data.starter == true)
    {
        memset(temp_packet, 0, MAX_PACKET);
        *temp_packet = MAP_IS;

        cout << "Choose map number 1 ~ 9 (0 for random)" << endl;
        do
        {
            cin >> map_number;
            if ((map_number < 0) && (map_number > MAX_MAP))
                cout << "Insert a number:" << endl;
        } while ((map_number < 0) && (map_number > MAX_MAP));
        if (map_number == 0)
            map_number = random_btwn(1, MAX_MAP);
        if (map_number < 10)
            *(temp_packet + 1) = 0x05;
        else
            *(temp_packet + 1) = 0x06;
        *(temp_packet + 2) = 0;
        strcat(buffer_for_map, "mapa");
        itoa(map_number, temp_number, 10);
        strcat(buffer_for_map, temp_number);
        strcat(temp_packet, buffer_for_map);

        *(temp_packet + strlen(temp_packet)) = getChecksum(strcat(buffer_for_map, ".png"));

        memcpy(Data.wmap.map_name, buffer_for_map, strlen(buffer_for_map)); //buffer_for_map with NULL terminator due to memset after declaration
        Data.wmap.map_name[strlen(buffer_for_map)] = 0;

        buffer_for_map[strlen(buffer_for_map) - 3 - 1] = 0;

        *(temp_packet + strlen(temp_packet)) = getChecksum(strcat(buffer_for_map, ".map"));

        if (sendPacket(conn, temp_packet, strlen(temp_packet)))
        {
            if (getPacket(conn, ACK, buffer))
            {
                buffer_for_map[strlen(buffer_for_map) - 3 - 1] = 0;
                cout << "The game will take place in " << buffer_for_map << "." << endl;
                if (Data.starting_player == I_START)
                {
                    cout << "Your opponent starts." << endl;
                    *(temp_packet) = YOU_START;
                    Data.starting_player = YOU_START;
                }
                else
                {
                    cout << "You start." << endl;
                    *(temp_packet) = I_START;
                    Data.starting_player = I_START;
                }

                Data.wind_strength = getWindStrength();
                *(temp_packet + 1) = Data.wind_strength;
                if (sendPacket(conn, temp_packet, 2))
                {
                    free(temp_packet);
                    return;
                }
            }
        }
    }
    else
    {
        if (getPacket(conn, MAP_IS, buffer))
        {
            memcpy(buffer_for_map, buffer + 2, buffer[1]);
            // buffer_for_map already contains null terminator because of memset() after declaration

            if (getChecksum(strcat(buffer_for_map, ".png")) == buffer[2 + buffer[1] + 1 - 1])
            {
                memcpy(Data.wmap.map_name, buffer_for_map, buffer[1]);
                Data.wmap.map_name[(int) buffer[1]] = 0;
                strcat(Data.wmap.map_name, ".png");

                buffer_for_map[strlen(buffer_for_map) - 3 - 1] = 0;
                if (getChecksum(strcat(buffer_for_map, ".map")) == buffer[2 + buffer[1] + 2 - 1])
                {
                    buffer_for_map[strlen(buffer_for_map) - 3 - 1] = 0;
                    cout << "The game will take place in " << buffer_for_map << "." << endl;

                    *temp_packet = ACK;
                    if (sendPacket(conn, temp_packet, 1))
                    {
                        if (getPacket(conn, I_START, buffer))
                        {
                            if (buffer[0] == I_START)
                            {
                                cout << "Your opponent starts." << endl;
                                Data.starting_player = YOU_START;
                            }
                            else
                            {
                                cout << "You start." << endl;
                                Data.starting_player = I_START;
                            }

                            Data.wind_strength = buffer[1];

                            free(temp_packet);
                            return;
                        }
                    }
                }
            }
            else
                cout << "The file " << buffer_for_map << " seems corrupted. Check your file or your oponent's file." << endl;
        }
    }
    Data.error = true;
}
