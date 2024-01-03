#include "End_Game.h"

// function to show the winner
void showWinner()
{
    gameSeconds = 0;

    tft.fillScreen(BACKGROUNDCOLORGAME);
    tft.setCursor(ILI9341_TFTHEIGHT / 2 - 100, 35);
    tft.setTextSize(4);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("You won!");
    tft.setTextSize(2);
    tft.setCursor(ILI9341_TFTHEIGHT / 2 - 80, 70);
    tft.print("The code was:");
    tft.setRotation(0);
    for (uint8_t i = 0; i < 4; i++)
    {
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (i + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[i].currentGameColors].ILI9341Color);
    }
}