#include "Menu_Headers/Super_Menu_Header.h"

Super_Menu_Header::Super_Menu_Header()
{

}

Super_Menu_Header::~Super_Menu_Header()
{
}

void Super_Menu_Header::drawBackground(Adafruit_ILI9341* tft)
{
    (*tft).fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, BACKGROUNDCOLOUR);
}
