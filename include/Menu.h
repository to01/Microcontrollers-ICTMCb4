#ifndef MENU_H_
#define MENU_H_

#include "Adafruit_ILI9341.h"
#include "Nunchuk_state.h"
#include "GameState.h"

#define BACKGROUNDCOLOUR 0x0500 // 5-6-5 color code for dark green

#define XPOS_MIDDLE 100 // the middle of the screen when rotation(1) is used
#define XPOS_TITLE 70   // Xpositon for the title
#define YPOS_TITLE 20   // yPosition for the thile
#define YPOS_SPACER 30  // the space between menu items
#define YPOS_START 70   // the height at which the first menu item is drawn

// font sizes
#define TITLE_FONT_SIZE 3
#define ITEM_FONT_SIZE 2

#define MENU_ARRAY_SIZE 3       // a menu has a max of 3 items
#define MENUHOLDER_ARRAY_SIZE 3 // a menu holder has a max of 3 menus

// positions
#define SELECT_XPOS 20     // distance from the text to where the selection circle is drawn
#define SELECT_YPOS 10     // distance to center the circle with the text
#define SELECTION_RADIUS 3 // radius of selection circle

extern Adafruit_ILI9341 tft;

// create a typedef that points to void functions
typedef void (*ActionPointer)();

struct MenuItem
{
    const char *title;          
    ActionPointer ButtonAction; //pointer to a void function
    uint8_t yPosition;
};

struct Menu
{
    const char *title;
    bool thirdOption;                    // does this menu have a third option?
    MenuItem itemArray[MENU_ARRAY_SIZE]; // insert MenuItems here
    uint8_t itemSelected;                //index of the selected item
};

struct MenuHolder
{
    Menu MenuArray[MENUHOLDER_ARRAY_SIZE]; //insert menus here
    uint8_t selectedMenu;               //index of the current menu
};

void drawMenu(Menu *menu);
void drawBackground();

void drawMenuItem(MenuItem item);
void selectMenuItem(MenuItem item);
void deselectMenuItem(MenuItem item);

bool checkNunchukButton();
void executeAction(MenuItem item);
void switchMenuItems(Menu *menu, Direction direction);

void goToStartMenu();
void goToGameMenu();
void goToSingleplayerMenu();
void goToRaceAgainstTheClock();

// function called in loop to render menu and handle all logic relating to it
void mainMenu();

#endif