#ifndef RUNGAME_H
#define RUNGAME_H

#include "../Back_end/Communication_files/server-client.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define CHANGE_ORIENTATION_TOLERANCE 50
#define PI 3.14159265
#define CROSSHAIR_RADIUS 75 //In pixels
#define CROSSHAIR_SPEED 2 //Arbitrary, if changed, chack fix preventing angle overflow (angles becoming bigger than 180 or smaller than -180)
#define POWER_BAR_SPEED 4 //Arbitrary
#define TURN_TIME 60 //In seconds
#define TOTAL_GAME_TIME 1200 //In seconds
#define ERROR_TOLERANCE (WORM_WIDTH / 2.0)

bool initAll(ALLEGRO_DISPLAY** disp);
void uninstallAll(ALLEGRO_DISPLAY* disp);
int runGame(PolonetConn conn, initData& Data);
int64_t getTotalTimeKey(ALLEGRO_KEYBOARD_STATE* previous_keyboard, int keycode, int64_t& total_time, int64_t& previous_time, ALLEGRO_TIMER* refresh_timer);
char getPower(int64_t past_TotalTimeKey);
void setWorm(int worm_ID, vector<Cworm>::iterator& i, Cmap& cMap);
void moveAngleDown(int& crosshair_angle, int orientation);
void moveAngleUp(int& crosshair_angle, int orientation);
void checkAngle(int& crosshair_angle, int orientation);
void adjustAngle(int& crosshair_angle, int orientation);
void angleOutOfRange(int& crosshair_angle);
void changeTurnToMy(char& turn, ALLEGRO_TIMER* turn_time, char WindStrength, bool& cycling_allowed, int& angle, Cmap& cMap, vector<Cworm>::iterator& i);
void changeTurnToYour(char& turn, ALLEGRO_TIMER* turn_time, char& WindStrength, Cmap& cMap, vector<Cworm>::iterator& i, PolonetConn conn, char* buffer);
int whoWonByHealth(vector<Cworm>& myWorms, vector<Cworm>& enemyWorms);
char selectFuse(ALLEGRO_KEYBOARD_STATE* keybState);
bool checkClick(ALLEGRO_MOUSE_STATE& mouse_state, ALLEGRO_MOUSE_STATE& previous_mouse_state, Point start, Point end, double dispWidth, double dispHeight);
void drawButtons(ALLEGRO_MOUSE_STATE& mouse_state, Cmap& map, Cworm& worm, double dispWidth, double dispHeight);

enum turns {MY_TURN = 1, OPPONENT_TURN};
enum gameStatus {ONGOING, WON, LOST};

#endif // RUNGAME_H
