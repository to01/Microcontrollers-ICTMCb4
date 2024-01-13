#include "Game_Timer.h"

uint8_t fpsToSeconds = 0;  // variable to convert the fps to seconds
uint8_t gameSeconds = 180; // variable to store the seconds

// function to update the time
void updateTimeMultiplayer(uint8_t secondsLeft)
{
    tft.fillRect(ILI9341_TFTHEIGHT / 2 - 24, 0, 50, 19, BACKGROUNDCOLORGAME);
    tft.setCursor(ILI9341_TFTHEIGHT / 2 - 21, 3);
    tft.print(secondsLeft / 60);
    tft.print(":");
    if (secondsLeft % 60 < 10)
    {
        tft.print("0");
    }
    tft.print(secondsLeft % 60);
}