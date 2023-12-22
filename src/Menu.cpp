#include "Menu.h"

void drawMenu(Menu *menu)
{
    drawBackground();

    tft.setTextSize(TITLE_FONT_SIZE);
    tft.setCursor(XPOS_TITLE, YPOS_SPACER);
    tft.print(menu->title);

    uint8_t spacer = YPOS_SPACER;

    tft.setTextSize(ITEM_FONT_SIZE);
    tft.setTextColor(ILI9341_DARKGREEN);

    for (uint8_t i = 0; i < MENU_ARRAY_SIZE; i++) 
    {
        menu->itemArray[i].yPosition = (YPOS_START + spacer);
        drawMenuItem(menu->itemArray[i]);

        if (!menu->thirdOption) // if there is no third option, then skip the middle position.
        {
            spacer = spacer * 2;
        }

        spacer += YPOS_SPACER; //increase the value of spacer
    }
}

void drawBackground()
{
    tft.fillScreen(BACKGROUNDCOLOUR);
}

void drawMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE,item.yPosition);
    tft.print(item.title);
}

void selectMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE,item.yPosition);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(item.title);
}