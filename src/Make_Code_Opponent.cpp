#include "Make_Code_Mechanics.h"
#include "Make_Code_Opponent.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// function to display the new color of the current pin
void changeColorCodeOpponent()
{
    switch (currentPin)
    {
    case 0:
        changeColorPin(currentPin);                                                                                                                                           // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 1:
        changeColorPin(currentPin);                                                                                                                                                              // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 2:
        changeColorPin(currentPin);                                                                                                                                                              // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 3:
        changeColorPin(currentPin);                                                                                                                                                              // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    }
    changeColorCodeOpponentCount++;
}

// function to change the selected pin
void selectPinCodeOpponent()
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
                blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT);
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
                blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT);
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
    for (uint8_t cg = 0; cg < 4; cg++)
    {
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (cg + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[cg].currentGameColors].ILI9341Color);
    }
    blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT);
}

extern uint16_t ticksSinceLastUpdate; // ticks since last display update

// function repeatedly called by main loop
// used for all codeOpponent logic
void codeOpponentLoop(const uint16_t ticksPerFrame)
{
    if (ticksSinceLastUpdate > ticksPerFrame)
    {
      selectPinCodeOpponent();
      ticksSinceLastUpdate = 0;
    }

    if (selectPinCount > SELECTEDPINCOUNT)
    {
      changeColorCodeOpponent();
      selectPinCount = 0;
    }

    if (changeColorCodeOpponentCount > CHANGECOLORCODEOPPONENTCOUNT)
    {
      drawCodeOpponent();
      changeColorCodeOpponentCount = 0;
    }
}

// function to get the binary code for the infraredprotocol of the color code
// use this function after the player has selected the colors and pressed the send-button
uint16_t getColorCodeBinary()
{
    uint16_t binaryCode = 0;

    for (int i = 0; i < 4; i++) // loop through the pin array
    {
        uint8_t colorCode = colorCodeArray[i].gameColors.colorCode; // get the color code of the current pin

        for (int j = 0; j < 4; j++) // loop through the color code
        {
            binaryCode = (binaryCode << 1) | ((colorCode >> (3 - j)) & 1); // shift the binary code to the left and add the current bit of the color code
        }
    }
    return binaryCode;
}