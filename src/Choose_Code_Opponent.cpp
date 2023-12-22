#include "Choose_Code_Opponent.h"

uint8_t currentPin = 0; // current pin that is selected

uint8_t changeColorCodeOpponentCount = 0; // used to change the color of the current pin

uint8_t selectPinCount = 0; // used to select the pin

uint8_t selectPinSpeed = 0; // used to select the pin

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

struct GameColors
{
    GameColors(uint8_t colorCode, uint16_t ILI9341Color) : colorCode(colorCode), ILI9341Color(ILI9341Color) {}
    uint8_t colorCode;     // every color has a code for infraredprotocol
    uint16_t ILI9341Color; // color combined to ILI9341Color
};

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

// data for the currentPin in the array
struct CodeOpponent
{
    CodeOpponent(GameColors gameColors, uint16_t currentGameColors) : gameColors(gameColors), currentGameColors(currentGameColors) {}
    GameColors gameColors;      // need this to combine the four colours to 1 code for infraredprotocol
    uint16_t currentGameColors; // to display the color on the screen
};

CodeOpponent codeOpponentArray[4] = {
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0)};

// function to change the color of the current pin
void changeColorPin(uint16_t currentPin)
{
    Nunchuk.getState(NUNCHUK_ADDRESS);
    if (Nunchuk.state.joy_y_axis > 200)
    {
        if (codeOpponentArray[currentPin].currentGameColors > 0)
        {
            codeOpponentArray[currentPin].currentGameColors--;
        }
        else
        {
            codeOpponentArray[currentPin].currentGameColors = 7;
        }
    }

    if (Nunchuk.state.joy_y_axis < 50)
    {
        if (codeOpponentArray[currentPin].currentGameColors < 7)
        {
            codeOpponentArray[currentPin].currentGameColors++;
        }
        else
        {
            codeOpponentArray[currentPin].currentGameColors = 0;
        }
    }
    codeOpponentArray[currentPin].gameColors = gameColorsArray[codeOpponentArray[currentPin].currentGameColors];
}

// function to display the new color of the current pin
void changeColorCodeOpponent()
{
    switch (currentPin)
    {
    case 0:
        changeColorPin(currentPin);                                                                                                                                                            // change the color of the current pin
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 1:
        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 2:
        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 3:
        changeColorPin(currentPin);                                                                                                                                            // change the color of the current pin
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    }
    changeColorCodeOpponentCount++;
}

// function to draw the glow of the current pin and deletes the glow of the previous pin
void displayCurrentPinCodeOpponent()
{
    if (currentPin == 0)
    {
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, ILI9341_WHITE); // draws the glow of the current pin
    }
    else if (currentPin == 1)
    {
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, ILI9341_WHITE); // draws the glow of the current pin
    }
    else if (currentPin == 2)
    {
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, ILI9341_WHITE); // draws the glow of the current pin
    }
    else if (currentPin == 3)
    {
        tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, ILI9341_WHITE); // draws the glow of the current pin
    }
}

// function to change the selected pin
void selectPin()
{
    selectPinSpeed++;
    if (selectPinSpeed > SELECTPINSPEED)
    {
        Nunchuk.getState(NUNCHUK_ADDRESS);  // get the state of the nunchuk
        if (Nunchuk.state.joy_x_axis > 200) // if the joystick is moved to the right
        {
            if (currentPin < 3) // if the current pin is not the last pin
            {
                currentPin++;
                displayCurrentPinCodeOpponent();
            }
            else // if the current pin is the last pin
            {
                currentPin = 3;
            }
        }

        if (Nunchuk.state.joy_x_axis < 50) // if the joystick is moved to the left
        {
            if (currentPin > 0) // if the current pin is not the first pin
            {
                currentPin--;
                displayCurrentPinCodeOpponent();
            }
            else // if the current pin is the first pin
            {
                currentPin = 0;
            }
        }
        selectPinSpeed = 0;
    }
    selectPinCount++;
}

// function to draw the first pins whit their color
void drawCodeOpponent()
{
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[0].currentGameColors].ILI9341Color);     // draw the first pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[1].currentGameColors].ILI9341Color); // draw the second pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[2].currentGameColors].ILI9341Color); // draw the third pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[3].currentGameColors].ILI9341Color);                     // draw the fourth pin
    displayCurrentPinCodeOpponent();
}

// function to get the binary code for the infraredprotocol of the color code
// use this function after the player has selected the colors and pressed the send-button
uint16_t getColorCodeBinary()
{
    uint16_t binaryCode = 0;

    for (int i = 0; i < 4; i++) // loop through the pin array
    {
        uint8_t colorCode = codeOpponentArray[i].gameColors.colorCode; // get the color code of the current pin

        for (int j = 0; j < 4; j++) // loop through the color code
        {
            binaryCode = (binaryCode << 1) | ((colorCode >> (3 - j)) & 1); // shift the binary code to the left and add the current bit of the color code
        }
    }
    return binaryCode;
}