#ifndef SUPER_MENU_SCREEN
#define SUPER_MENU_SCREEN

/*  DESCRIPTION
    super header
*/

#include "Adafruit_ILI9341.h"


#define TFT_DC 9
#define TFT_CS 10
#define BACKGROUNDCOLOUR 0x0500 //5-6-5 color code for dark green

class Super_Menu_Screen
{
public:
    Adafruit_ILI9341* tft;

    Super_Menu_Screen(Adafruit_ILI9341* tft); //con
    ~Super_Menu_Screen();//des
    void drawBackground();
    void drawTitle(String title, uint8_t xPosition, uint8_t yPosition);
private:

};


#endif