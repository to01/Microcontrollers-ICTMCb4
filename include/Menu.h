#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>
#include "Adafruit_ILI9341.h"

#define BACKGROUNDCOLOUR 0x0500 // 5-6-5 color code for dark green

#define XPOS_MIDDLE 100 // the middle of the screen when rotation(1) is used
#define XPOS_TITLE 70   // Xpositon for the title
#define YPOS_TITLE 20   // yPosition for the thile
#define YPOS_SPACER 30  // the space between menu items
#define YPOS_START 70   // the height at which the first menu item is drawn

//font sizes
#define TITLE_FONT_SIZE 3
#define ITEM_FONT_SIZE 2

#define MENU_ARRAY_SIZE 3  // a menu always has a max of 3 items

//positions
#define SELECT_XPOS 20     // distance from the text to where the selection circle is drawn
#define SELECT_YPOS 10     // distance to center the circle with the text
#define SELECTION_RADIUS 3 // radius of selection circle

extern Adafruit_ILI9341 tft;

struct MenuItem
{
    char title[23];
    bool isSelected;
    uint8_t yPosition;
    void (*function)();
};

struct Menu
{
    char title[12];
    bool thirdOption;                    // does this item have a third option?
    MenuItem itemArray[MENU_ARRAY_SIZE]; // insert MenuItems here
};

void drawMenu(Menu *menu);
void drawBackground();

void drawMenuItem(MenuItem item);
void selectMenuItem(MenuItem* item);
void deselectMenuItem(MenuItem* item);

void goToGameMenu();
void goToSingleplayerMenu();

#endif