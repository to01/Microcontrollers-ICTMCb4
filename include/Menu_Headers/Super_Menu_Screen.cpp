#include "Menu_Headers/Super_Menu_Screen.h"

Super_Menu_Screen::Super_Menu_Screen(Adafruit_ILI9341* screenObj)
{
    this->tft = screenObj;
}

Super_Menu_Screen::~Super_Menu_Screen()
{

}

void Super_Menu_Screen::drawBackground()
{
    //tft->fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, BACKGROUNDCOLOUR);
   
    tft->fillScreen(BACKGROUNDCOLOUR);
    tft->setRotation(1);
}

void Super_Menu_Screen::drawTitle(String title, uint8_t xPosition, uint8_t yPosition)
{
    tft->setTextSize(FONTSIZE);
    tft->setCursor(xPosition,yPosition);
    tft->print(title);
}