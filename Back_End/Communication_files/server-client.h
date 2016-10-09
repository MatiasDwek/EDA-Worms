#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <fstream>
#include "../Map_files/map.h"
#include "../../polonet.h"

#define PORT_COMM 13225
#define SERVER_WAIT 10000 //In miliseconds
#define BETWEEN_PACKET_WAIT 1500 //In seconds
#define ASCII_NUMBER_FIX 48
#define MAX_MAP_NAME 50
#define ASCII_UP_LET_FIX 0x41
#define ASCII_LOW_LET_FIX 0x61

#define MAX_NAME 256 //Si se llega a modificar este valor, modificar tambien el valor del scanf de la linea 64 del archivo server-client.cpp (%255s)
#define MAX_PACKET 260
#define NAME_LEN 40
#define MAX_MAP 9

using namespace std;
///WHATTT???
enum {NAME_IS = 0x11,
              MAP_IS = 0x12,
              I_START = 0x21,
              YOU_START = 0x20,
              MOVE = 0x31,
              PJUMP = 0x32,
              ATTACK = 0x33,
              YOUR_TURN = 0x35,
              ACK = 0x01,
              NAME = 0x10,
              PASS = 0x34,
              YOU_WON = 0x40,
              TIME_IS_UP = 0x41,
              PLAY_AGAIN = 0x50,
              GAME_OVER = 0x51,
              ERROR_WORMS = 0xFE,
              QUIT = 0xFF,
              HEADER = 0xFD};
typedef int packetType;

typedef enum {SENDING, FINISHED, CONNECTION_ERROR = -1} sendStates;
#define NOTHING_ON_BUFFER 0

typedef enum {} packetErrorType;

struct initData
{
    packetType starting_player;
    MapData wmap;
    char wind_strength;
    bool error;
    bool starter;
};

initData serverSequence(PolonetConn conn);
initData clientSequence(PolonetConn conn);
int sendPacket(PolonetConn conn, char* packet, int packetLen); //Returns bytes sent
int getPacket(packetType type, int lenCount, char* buffer); //buffer's size must be MAX_PACKET, returns bytes received
sendStates NBsendPacket(PolonetConn conn, char* packet, int packetLen, int& bytes_sent);
packetType NBgetPacket(PolonetConn conn, char* buffer);
void createMovePacket(char action, char team, char worm, unsigned short final_row, unsigned short final_column, char* buffer); //buffer's size at least 8 bytes
void createJumpPacket(char team, char worm, unsigned short final_row, unsigned short final_column, char* buffer);
void createAttackPacket(char team, char worm, char weapon, char power, char angle, char fuse,
                        unsigned short final_row, unsigned short final_column, char* buffer);
int random_btwn(int min, int max); //Random number between min and max, max included
unsigned char getChecksum(char* fileName);
char getWindStrength(void);
int twoCharToInt(char MSB, char LSB);

class Map
{
public:
    ALLEGRO_BITMAP* getImage();
    ///Agregar constructor, destructor y boludeces
private:
    ALLEGRO_BITMAP* image;
    ifstream file;
};

class Move
{
    int mo;
};

class Attack
{
    int att;
};

class Packet
{
public:
    Packet(const char* originalBuffer, unsigned long bufSize);
	unsigned long getSize();
	packetType getType();
	packetErrorType getError();
protected:
    char buffer[MAX_PACKET];
	unsigned long size;
};

class PacketNameIs : public Packet
{
public:
	//Pone el nombre en externBuffer. Si el nombre es mayor de bufLen pone hasta bufLen-1 y un terminador. Devuelve en su nombre el largo del nombre
    unsigned int getName(char* externBuffer, unsigned int bufLen);
};

class PacketMapIs : public Packet
{
public:
	// Devuelve en su nombre un Map (clase) a partir de la info del mapa recibido.
	// El constructor copiador de Map está hecho de forma que sea eficiente su copia (por es lo pasa en su nombre).
	// Si no es posible entonces recibe en su argumento un puntero a Map (Map *) que crea en el heap y después quién lo use lo deberá desalocar.
	///Map getMap(void);
};
/**
Map PacketMapIs::getMap(void)
{
    return Map(); ///Checkear esto de aca, falta constructor copiador
}
**/
// Se usa la misma clase para represetar los paquetes YOU_START, I_START y YOUR_TURN (se los distingue mediante la función Packet::getType()).
class PacketSbyStartsOrTurnPass : public Packet
{
public:
    unsigned int getWindStrength();
};

// Se usa la misma clase para representar tanto el paquete MOVE como JUMP.
class PacketMoveOrJump : public Packet
{
public:

	// Devuelve en su nombre un Move (clase) a partir de la info del movimiento recibido.
	// El constructor copiador de Move está hecho de forma que sea eficiente su copia (por es lo pasa en su nombre).
	// Si no es posible entonces recibe en su argumento un puntero a Move (Move *) que crea en el heap y después quién lo use lo deberá desalocar.
	Move getMovement(void);
};

class PacketAttack : public Packet
{
public:
	// Mismo comentario que para Move y Map.
	Attack getAttack(void);
};

// Todos los demás paquetes no tiene campo de datos por lo tanto se los distingue con Packet::getType().

#endif
