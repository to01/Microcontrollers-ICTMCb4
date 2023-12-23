#include "Make_Code_Mechanics.h"

uint8_t currentPin = 0; // current pin that is selected

uint8_t changeColorCodeOpponentCount = 0; // used to change the color of the current pin

uint8_t selectPinCount = 0; // used to select the pin

uint8_t selectPinSpeed = 0; // used to select the pin

// data for the colors in the array
GameColors gameColorsArray[8] = {
    GameColors(RED, ILI9341_RED),
    GameColors(ORANGE, ILI9341_ORANGE),
    GameColors(YELLOW, ILI9341_YELLOW),
    GameColors(LIGHT_GREEN, ILI9341_GREEN),
    GameColors(GREEN, ILI9341_OLIVE),
    GameColors(LIGHT_BLUE, ILI9341_CYAN),
    GameColors(BLUE, ILI9341_BLUE),
    GameColors(PURPLE, ILI9341_PURPLE)};

ColorCode colorCodeArray[4] = {
    ColorCode(GameColors(RED, ILI9341_RED), 0),
    ColorCode(GameColors(RED, ILI9341_RED), 0),
    ColorCode(GameColors(RED, ILI9341_RED), 0),
    ColorCode(GameColors(RED, ILI9341_RED), 0)};

// function to change the color of the current pin
void changeColorPin(uint8_t currentPin)
{
    Nunchuk.getState(NUNCHUK_ADDRESS);
    if (Nunchuk.state.joy_y_axis > 200)
    {
        if (colorCodeArray[currentPin].currentGameColors > 0)
        {
            colorCodeArray[currentPin].currentGameColors--;
        }
        else
        {
            colorCodeArray[currentPin].currentGameColors = 7;
        }
    }

    if (Nunchuk.state.joy_y_axis < 50)
    {
        if (colorCodeArray[currentPin].currentGameColors < 7)
        {
            colorCodeArray[currentPin].currentGameColors++;
        }
        else
        {
            colorCodeArray[currentPin].currentGameColors = 0;
        }
    }
    colorCodeArray[currentPin].gameColors = gameColorsArray[colorCodeArray[currentPin].currentGameColors];
}

// function to blink the current pin
void blinkCurrentPin(uint8_t valueXCurrentPin, uint16_t valueYCurrentPin, uint8_t valueRadiusCurrentPin)
{
    if (currentPin == 0)
    {
        tft.fillCircle(valueXCurrentPin, valueYCurrentPin, valueRadiusCurrentPin, BACKGROUNDCOLORGAME); // draws the blink of the current pin
    }
    else if (currentPin == 1)
    {
        tft.fillCircle(valueXCurrentPin, valueYCurrentPin, valueRadiusCurrentPin, BACKGROUNDCOLORGAME); // draws the blink of the current pin
    }
    else if (currentPin == 2)
    {
        tft.fillCircle(valueXCurrentPin, valueYCurrentPin, valueRadiusCurrentPin, BACKGROUNDCOLORGAME); // draws the blink of the current pin
    }
    else if (currentPin == 3)
    {
        tft.fillCircle(valueXCurrentPin, valueYCurrentPin, valueRadiusCurrentPin, BACKGROUNDCOLORGAME); // draws the blink of the current pin
    }
}
