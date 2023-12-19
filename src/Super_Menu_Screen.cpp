#include "Menu_Headers/Super_Menu_Header.h"

Super_Menu_Header::Super_Menu_Header()
{

}

Super_Menu_Header::~Super_Menu_Header()
{
}

void Super_Menu_Header::drawBackground(Adafruit_ILI9341* tft)
{
    *tft->begin();
    *tft->fillScreen(BACKGROUNDCOLOUR);
}
