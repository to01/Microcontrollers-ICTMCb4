#include "Menu_Headers/Super_Menu_Screen.h"
//NOTE: i should look for a way for tft to be in the super so that it doesnt have to be passed along each time

Super_Menu_Screen::Super_Menu_Screen(Adafruit_ILI9341* tft)
{
    this->tft = tft;
}

Super_Menu_Screen::~Super_Menu_Screen()
{

}

void Super_Menu_Screen::drawBackground()
{
    //tft->fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, BACKGROUNDCOLOUR);
    tft->fillScreen(BACKGROUNDCOLOUR);
}

void Super_Menu_Screen::drawTitle(String title, uint8_t xPosition, uint8_t yPosition)
{
    
}