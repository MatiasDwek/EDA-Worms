#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <conio.h>
#include <fstream> //files
#include "../../polonet.h"
#include "server-client.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Tabla Checksum
const unsigned char Tabla []= {
 98, 6, 85,150, 36, 23,112,164,135,207,169, 5, 26, 64,165,219, // 1
 61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115, // 2
 90,168,156,203,177,120, 2,190,188, 7,100,185,174,243,162, 10, // 3
 237, 18,253,225, 8,208,172,244,255,126,101, 79,145,235,228,121, // 4
 123,251, 67,250,161, 0,107, 97,241,111,181, 82,249, 33, 69, 55, // 5
 59,153, 29, 9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222, // 6
 197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186, // 7
 39,158,178,187,131,136, 1, 49, 50, 17,141, 91, 47,129, 60, 99, // 8
 154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254, // 9
 133,232,196,144,198,124, 53, 4,108, 74,223,234,134,230,157,139, // 10
 189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44, // 11
 183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12, // 12
 221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166, // 13
 3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117, // 14
 238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110, // 15
 43,119,224, 71,122,142, 42,160,104, 48,247,103, 15, 11,138,239 // 16
};
// Tabla Checksum



initData serverSequence(PolonetConn conn)
{
    char buffer[MAX_PACKET];
    memset(buffer, 0, MAX_PACKET);
    char buffer_for_name[MAX_NAME];
    char buffer_for_map[MAX_MAP_NAME];
    memset(buffer_for_map, 0, MAX_MAP_NAME);
    int map_number;
    char temp_number[5];
    initData Data;
    Data.error = false;

    char* temp_packet = (char*) malloc(MAX_PACKET);

    if (temp_packet != NULL)
    {
        *temp_packet = NAME;
        if (sendPacket(conn, temp_packet, 1))
        {
            if(getPacket(conn, NAME_IS, buffer))
            {
                printf("Your oponent is %.*s.\n", buffer[1], buffer + 2);

                *temp_packet = ACK;
                if (sendPacket(conn, temp_packet, 1))
                {

                    if(getPacket(conn, NAME, buffer))
                    {
                        cout << "Insert name: ";
                        scanf("%255s", buffer_for_name);
                        //temp_packet = (char*) realloc(temp_packet, 2 + strlen(buffer_for_name)); // 2 = 1 byte from packet type + 1 byte from count
                        temp_packet[0] = NAME_IS;
                        temp_packet[1] = strlen(buffer_for_name);
                        temp_packet[2] = 0;
                        strcat(temp_packet, buffer_for_name);

                        if (sendPacket(conn, temp_packet, 2 + strlen(buffer_for_name)))
                        {

                            if (getPacket(conn, ACK, buffer))
                            {

                                memset(temp_packet, 0, MAX_PACKET);
                                *temp_packet = MAP_IS;
                                cout << "Choose map number 1 ~ 9 (0 for random):" << endl;
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
                                        if (random_btwn(0, 1) == 1)
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
                                            return Data;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Data.error = true;
    *temp_packet = ERROR_WORMS;
    sendPacket(conn, temp_packet, 1);
    free(temp_packet);
    return Data;
}

initData clientSequence(PolonetConn conn)
{
    char buffer[MAX_PACKET];
    memset(buffer, 0, MAX_PACKET);
    char buffer_for_name[MAX_NAME];
    char buffer_for_map[MAX_MAP_NAME];
    memset(buffer_for_map, 0, MAX_MAP_NAME);
    initData Data;
    Data.error = false;

    char* temp_packet = (char*) malloc(MAX_PACKET);
    if (temp_packet != NULL)
    {
        if (getPacket(conn, NAME, buffer))
        {
            cout << "Insert name: ";
            scanf ("%255s", buffer_for_name);
            temp_packet[0] = NAME_IS;
            temp_packet[1] = strlen(buffer_for_name);
            temp_packet[2] = 0;
            strcat(temp_packet, buffer_for_name);


            sendPacket(conn, temp_packet, 2 + strlen(buffer_for_name));
            if (getPacket(conn, ACK, buffer))
            {
                *temp_packet = NAME;
                if (sendPacket(conn, temp_packet, 1))
                {
                    if (getPacket(conn, NAME_IS, buffer))
                    {
                        printf("Your opponent is %.*s.\n", buffer[1], buffer + 2);
                        *temp_packet = ACK;
                        if (sendPacket(conn, temp_packet, 1))
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
                                                return Data;
                                            }
                                        }
                                    }
                                }
                                else
                                    cout << "The file " << buffer_for_map << " seems corrupted. Check your file or your oponent's file." << endl;

                            }
                        }
                    }
                }
            }
        }
    }

    Data.error = true;
    *temp_packet = ERROR_WORMS;
    sendPacket(conn, temp_packet, 1);
    free(temp_packet);
    return Data;
}

int sendPacket(PolonetConn conn, char* packet, int packetLen)
{
    ALLEGRO_TIMER* timeout_timer = NULL;
    timeout_timer = al_create_timer(1);

    if (timeout_timer != NULL)
    {
        unsigned int bytes_sent = 0;
        al_start_timer(timeout_timer);

        do
        {
            if (isConnected(conn) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT))
                bytes_sent += sendData(conn, packet, packetLen);
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }

        } while(bytes_sent < (unsigned) packetLen);
        al_destroy_timer(timeout_timer);
        return bytes_sent;
    }
    else
    {
        al_destroy_timer(timeout_timer);
        return 0;
    }
}

//buffer's size must be MAX_PACKET
// type = 0 if type unknown
int getPacket(PolonetConn conn, packetType type, char* buffer)
{
    ALLEGRO_TIMER* timeout_timer = NULL;
    timeout_timer = al_create_timer(1);

    if (timeout_timer != NULL)
    {
        int numBytesReceived = 0;
        memset(buffer, 0, MAX_PACKET);
        char temp_buffer[MAX_PACKET];
        memset(temp_buffer, 0, MAX_PACKET);

        al_start_timer(timeout_timer);
        switch (type)
        {
        case ACK:
        case NAME:
        case PASS:
        case YOU_WON:
        case TIME_IS_UP:
        case PLAY_AGAIN:
        case GAME_OVER:
        case ERROR_WORMS:
        case HEADER:
        case QUIT:
            while (isConnected(conn) && (!receiveData(conn, buffer, 1)) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT))
                ;
            if ((buffer[0] == type) || (type == HEADER))
            {
                al_destroy_timer(timeout_timer);
                return 1;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        case NAME_IS:
            while (numBytesReceived != 2)
            {
                if (numBytesReceived == 1)
                    numBytesReceived += receiveData(conn, buffer + 1, 2);
                else
                    numBytesReceived += receiveData(conn, buffer, 2);
            }
            do
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                memcpy(buffer + numBytesReceived - buffer[1], temp_buffer, 2 + buffer[1]);
                memset(temp_buffer, 0, MAX_PACKET);
            } while (isConnected(conn) && (numBytesReceived < 2 + buffer[1]) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)); // 2 = 1 byte from packet type + 1 byte from count

            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        case MAP_IS:
            while (numBytesReceived != 2)
            {
                if (numBytesReceived == 1)
                    numBytesReceived += receiveData(conn, buffer + 1, 2);
                else
                    numBytesReceived += receiveData(conn, buffer, 2);
            }
            do
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                memcpy(buffer + numBytesReceived - buffer[1] - 2, temp_buffer, 4 + buffer[1]);
                memset(temp_buffer, 0, MAX_PACKET);
            } while (isConnected(conn) && (numBytesReceived < 4 + buffer[1]) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)); // 4 = 1 byte from packet type + 1 byte from count + (1 + 1) one from each checksum
            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        case I_START:
        case YOU_START:
        case YOUR_TURN:
            while (isConnected(conn) && (numBytesReceived < 2) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)) // 2 = 1 byte from header + 1 byte from wind strength
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                strcat(buffer, temp_buffer);
                memset(temp_buffer, 0, MAX_PACKET);
            }
            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        case MOVE:
            do
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                memcpy(buffer + numBytesReceived, temp_buffer, 8);
                memset(temp_buffer, 0, MAX_PACKET);
            } while (isConnected(conn) && (numBytesReceived < 8) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)); // 8 similar to the other cases, see packet info for specifications
            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }

            break;
        case PJUMP:
            do
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                memcpy(buffer + numBytesReceived, temp_buffer, 7);
                memset(temp_buffer, 0, MAX_PACKET);
            } while (isConnected(conn) && (numBytesReceived < 7) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)); // 7 similar to the other cases, see packet info for specifications
            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        case ATTACK:
            do
            {
                numBytesReceived += receiveData(conn, temp_buffer, MAX_PACKET);
                memcpy(buffer + numBytesReceived, temp_buffer, 11);
                memset(temp_buffer, 0, MAX_PACKET);
            } while (isConnected(conn) && (numBytesReceived < 11) && (al_get_timer_count(timeout_timer) < BETWEEN_PACKET_WAIT)); // 11 similar to the other cases, see packet info for specifications
            if (numBytesReceived)
            {
                al_destroy_timer(timeout_timer);
                return numBytesReceived;
            }
            else
            {
                al_destroy_timer(timeout_timer);
                return 0;
            }
            break;
        default:
            al_destroy_timer(timeout_timer);
            return 0;
        }
    }
    else
    {
        al_destroy_timer(timeout_timer);
        return 0;
    }
}

sendStates NBsendPacket(PolonetConn conn, char* packet, int packetLen, int& bytes_sent)
{
    if (isConnected(conn))
    {
        bytes_sent += sendData(conn, packet + bytes_sent, packetLen);

        if (bytes_sent == packetLen)
            return FINISHED;
        else if (bytes_sent < packetLen)
            return SENDING;
        else
            return CONNECTION_ERROR;
    }
    else
        return CONNECTION_ERROR;
}

 // Start Modifyed by Agus PM 12-Apr-2014
packetType NBgetPacket(PolonetConn conn, char* buffer)
{

    int numBytesReceived = 0;
    int lastBytesReceived = 0;
    memset(buffer, 0, MAX_PACKET);
    char temp_buffer[MAX_PACKET];
    memset(temp_buffer, 0, MAX_PACKET);



    if(!isConnected(conn))
        return CONNECTION_ERROR;

    numBytesReceived = receiveData(conn, buffer, 1);


    if (numBytesReceived)
    {
        switch ((unsigned char) buffer[0])
        {
        case ACK:
        	return ACK;
        case PASS:
        	return PASS;
        case YOU_WON:
        	return YOU_WON;
        case TIME_IS_UP:
        	return TIME_IS_UP;
        case PLAY_AGAIN:
        	return PLAY_AGAIN;
        case GAME_OVER:
        	return GAME_OVER;
        case ERROR_WORMS:
        	return ERROR_WORMS;
        case HEADER:
        	return HEADER;
        case QUIT:
		    return QUIT;
            break;
        case YOUR_TURN:
            while (isConnected(conn) && (numBytesReceived < 2)) // 2 = 1 byte from header + 1 byte from wind strength
            {
                lastBytesReceived = receiveData(conn, temp_buffer, 2 - numBytesReceived);
                numBytesReceived += lastBytesReceived;
                memcpy(buffer + numBytesReceived - lastBytesReceived, temp_buffer, 2);
                memset(temp_buffer, 0, 2);
            }
            if (numBytesReceived == 2)
                return YOUR_TURN;
            else
                return CONNECTION_ERROR;
            break;
        case MOVE:
            while (isConnected(conn) && (numBytesReceived < 8)) // 8 similar to the other cases, see packet info for specifications
            {
                lastBytesReceived = receiveData(conn, temp_buffer, 8 - numBytesReceived);
                numBytesReceived += lastBytesReceived;
                memcpy(buffer + numBytesReceived - lastBytesReceived, temp_buffer, 8);
                memset(temp_buffer, 0, 8);
            }
            if (numBytesReceived == 8)
                //return numBytesReceived;
                return MOVE;
            else
                return CONNECTION_ERROR;
            break;
        case PJUMP:
            while (isConnected(conn) && (numBytesReceived < 7)) // 7 similar to other cases
            {
                lastBytesReceived = receiveData(conn, temp_buffer, 7 - numBytesReceived);
                numBytesReceived += lastBytesReceived;
                memcpy(buffer + numBytesReceived - lastBytesReceived, temp_buffer, 7);
                memset(temp_buffer, 0, 7);
            }
            if (numBytesReceived == 7)
                return PJUMP;
            else
                return CONNECTION_ERROR;
            break;
        case ATTACK:
            while (isConnected(conn) && (numBytesReceived < 11)) // 11 similar to other cases
            {
                lastBytesReceived = receiveData(conn, temp_buffer, 11 - numBytesReceived);
                numBytesReceived += lastBytesReceived;
                memcpy(buffer + numBytesReceived - lastBytesReceived, temp_buffer, 11);
                memset(temp_buffer, 0, 11);
            }
            if (numBytesReceived == 11)
                return ATTACK;
            else
                return CONNECTION_ERROR;
            break;
        default:
            return CONNECTION_ERROR;
        }
    }
    else
        return NOTHING_ON_BUFFER;
}
 // End Modifyed by Agus PM 12-Apr-2014

int random_btwn(int min, int max)
{
    rand();
    int range = max - min + 1;
    if (range > 0)
        return rand() % range + min;
    else
        return -(rand() % range + min);
}

//file_name already includes extension
unsigned char getChecksum(char* fileName)
{
    return 1;
    string full_path;
    full_path = full_path + "./Images/Scenarios/";
    full_path = full_path + fileName;
    ifstream file(full_path.c_str(), ios::in | ios::binary);
    unsigned char index = 0;
    unsigned char byteRead;
    if (file.is_open())
    {
        while (file.good())
        {
            byteRead = file.get();
            if (file.good())
            {
                index = Tabla[index xor byteRead];
            }
        }
        file.close();
    }
    return index;
}

char getWindStrength(void)
{
    if (random_btwn(0, 1) == 1) //Choose bewteen right or left wind
    {
        if (random_btwn(0, 10))
            return random_btwn(0, 9) + ASCII_UP_LET_FIX;
        else
            return 0;
    }
    else
    {
        if (random_btwn(0, 10))
            return random_btwn(0, 9) + ASCII_LOW_LET_FIX;
        else
            return 0;
    }
}

//buffer's size must be at least of 8 bytes
void createMovePacket(char action, char team, char worm, unsigned short final_row, unsigned short final_column, char* buffer)
{
    buffer[0] = MOVE;
    buffer[1] = action;
    buffer[2] = team;
    buffer[3] = worm;
    buffer[4] = (unsigned char) (final_row >> 8);
    buffer[5] = (unsigned char) final_row;
    buffer[6] = (unsigned char) (final_column >> 8);
    buffer[7] = (unsigned char) final_column;
}

void createJumpPacket(char team, char worm, unsigned short final_row, unsigned short final_column, char* buffer)
{
    buffer[0] = PJUMP;
    buffer[1] = team;
    buffer[2] = worm;
    buffer[3] = (unsigned char) (final_row >> 8);
    buffer[4] = (unsigned char) final_row;
    buffer[5] = (unsigned char) (final_column >> 8);
    buffer[6] = (unsigned char) final_column;
}

void createAttackPacket(char team, char worm, char weapon, char power, char angle, char fuse,
                        unsigned short final_row, unsigned short final_column, char* buffer)
{
    buffer[0] = ATTACK;
    buffer[1] = team;
    buffer[2] = worm;
    buffer[3] = weapon;
    buffer[4] = power;
    buffer[5] = angle;
    buffer[6] = fuse;
    buffer[7] = (unsigned char) (final_row >> 8);
    buffer[8] = (unsigned char) final_row;
    buffer[9] = (unsigned char) (final_column >> 8);
    buffer[10] = (unsigned char) final_column;
}

int twoCharToInt(char MSB, char LSB)
{
    unsigned char MSBu = MSB, LSBu = LSB;
    int b = MSBu * 256 + LSBu;
    return b;
}
