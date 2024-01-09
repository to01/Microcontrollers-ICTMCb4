#include "Make_Code_Opponent.h"

extern Adafruit_ILI9341 tft;

// function to display the new color of the current pin
void changeColorCodeOpponent(Direction direction)
{
    switch (currentPin)
    {
    case 0:
        changeColorPin(currentPin, direction);                                                                                                                                // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 1:
        changeColorPin(currentPin, direction);                                                                                                                                                   // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 2:
        changeColorPin(currentPin, direction);                                                                                                                                                   // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 3:
        changeColorPin(currentPin, direction);                                                                                                                                                   // change the color of the current pin
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    }
    changeColorCodeOpponentCount++;
}

// function to change the selected pin
void selectPinCodeOpponent()
{
    tft.setRotation(0);
    // selectPinSpeed++;
    // if (selectPinSpeed > SELECTPINSPEED)
    // {
    if (getNunchukDirection() == Right) // if the joystick is moved to the right
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

    if (getNunchukDirection() == Left) // if the joystick is moved to the left
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
        // }
        // selectPinSpeed = 0;
    }
    selectPinCount++;
}

// function to draw the first pins whit their color
void drawCodeOpponent()
{
    tft.setRotation(1);
    tft.setCursor(80, 35);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print("Make a Code for");
    tft.setCursor(90, 55);
    tft.print("your Opponent");
    tft.setRotation(0);

    for (uint8_t cg = 0; cg < 4; cg++)
    {
        tft.fillCircle(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (cg + 1), RADIUS_CODEOPPONENT, gameColorsArray[colorCodeArray[cg].currentGameColors].ILI9341Color);
    }
    blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT);

    tft.setRotation(1);
    tft.setCursor(15, 200);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.print("Press C to Cancel");
    tft.setCursor(195, 200);
    tft.print("Press Z to Continue");
    tft.setRotation(0);
}

// function repeatedly called by main loop
// used for all codeOpponent logic
void codeOpponentLoop(const uint16_t ticksPerFrame)
{
    Direction direction = getNunchukDirection();

    if (ticksSinceLastUpdate > ticksPerFrame)
    {
        selectPinCodeOpponent();
        changeColorCodeOpponent(direction);
        ticksSinceLastUpdate = 0;
    }

    // if (selectPinCount > SELECTEDPINCOUNT)
    // {
    //     changeColorCodeOpponent();
    //     selectPinCount = 0;
    // }

    if (changeColorCodeOpponentCount > COUNTFORBLINKING)
    {
        drawCodeOpponent();
        changeColorCodeOpponentCount = 0;
    }

    // get the current state of the nunchuk buttons
    static bool previousZ = Nunchuk.state.z_button;
    if (Nunchuk.state.z_button && Nunchuk.state.z_button != previousZ) // if the Z-button went from released to pressed
    {
        sendCodeToOpponent();
        for (uint8_t i = 0; i < 4; i++)
        {
            colorCodeArray[i].currentGameColors = 0;
            colorCodeArray[i].gameColors = gameColorsArray[0];
        }
        setGameState(GAMEMULTIPLAYER);
        drawPlayingField();
    }
    previousZ = Nunchuk.state.z_button;

    static bool previousC = Nunchuk.state.c_button;
    if (Nunchuk.state.c_button && Nunchuk.state.c_button != previousC) // if the C-button went from released to pressed
    {
        setGameState(MENU);
        for (uint8_t i = 0; i < 4; i++)
        {
            colorCodeArray[i].currentGameColors = 0;
            colorCodeArray[i].gameColors = gameColorsArray[0];
        }
        currentPin = 0;
    }
    previousC = Nunchuk.state.c_button;
}

// function to send the code to the opponent
void sendCodeToOpponent()
{
    sendBits(getColorCodeBinary());
}