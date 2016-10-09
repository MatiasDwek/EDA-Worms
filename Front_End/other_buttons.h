#ifndef OTHER_BUTTONS_H
#define OTHER_BUTTONS_H
#include "../Back_End/Defines/point.h"

#define ALLEGRO_STATICLINK
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

class Button
{
public:
    Point number;
    Point button_start;
    Point button_end;
};

class Pass : public Button
{
public:
    Pass(void);
};

class Cycle : public Button
{
public:
    Cycle(void);
};

class Surrender : public Button
{
public:
    Surrender(void);
};

#endif // OTHER_BUTTONS_H
