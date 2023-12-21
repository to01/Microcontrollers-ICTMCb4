#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>
#include "Adafruit_ILI9341.h"

#define BACKGROUNDCOLOUR 0x0500 //5-6-5 color code for dark green
#define XPOS_MIDDLE 100 //the middle of the screen when rotation(1) is used

extern Adafruit_ILI9341 tft;

struct MenuItem
{
    char title[10];
    uint8_t yposition;
    void *(*function); //a pointer to any void functions
};

struct Menu
{
    char title[22];
    bool thirdOption;      //does this item have a third option?
    MenuItem itemArray[3]; //insert MenuItems here
};

void drawMenu(Menu menu);
void drawBackground();
void drawMenuItem(MenuItem item);


#endif