#ifndef SUPER_MENU_HEADER
#define SUPER_MENU_HEADER

#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define BACKGROUNDCOLOUR 0x0500

class Super_Menu_Header
{
public:
    Super_Menu_Header(); //con
    ~Super_Menu_Header();//des
    void drawBackground(Adafruit_ILI9341 *tft);
private:

};


#endif