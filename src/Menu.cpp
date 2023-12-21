#include "Menu.h"

//get the tft in here!
void drawMenu(Menu menu)
{
    tft.setCursor(10,10);
    tft.print(menu.title);
}

void drawBackground()
{
    tft.fillScreen(BACKGROUNDCOLOUR);
}

void drawMenuItem(MenuItem item)
{

}


