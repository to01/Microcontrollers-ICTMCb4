#include "Make_Code_Opponent.h"

extern Adafruit_ILI9341 tft;

// function to change the selected pin
void selectPinCodeOpponent(Direction direction)
{
    tft.setRotation(0);
    // selectPinSpeed++;
    // if (selectPinSpeed > SELECTPINSPEED)
    // {
    // Direction direction = getFilteredDirection();

    if (direction == Right) // if the joystick is moved to the right
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

    if (direction == Left) // if the joystick is moved to the left
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
    // blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT);

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
    Direction direction = getFilteredDirection();
    static bool sentCodeToOpponent = false;
    static bool recievedCodeFromOpponent = false;

    // if (ticksSinceLastUpdate > ticksPerFrame)
    // {

    if (!sentCodeToOpponent)
    {
        selectPinCodeOpponent(direction);
        changeColorPin(currentPin, direction); // change the color of the current pin

        //     ticksSinceLastUpdate = 0;
        // }

        // if (selectPinCount > SELECTEDPINCOUNT)
        // {
        //     changeColorCodeOpponent();
        //     selectPinCount = 0;
        // }

        if (countForBlinking > COUNTFORBLINKING)
        {
            blinkCurrentPin(HEIGHT_CODEOPPONENT, (15 + RADIUS_CODEOPPONENT * 2) * (currentPin + 1), RADIUS_CODEOPPONENT); // blinks the current pin

            countForBlinking = 0;
            countForDrawCurrentGuess++;
        }

        if (countForDrawCurrentGuess > 1)
        {
            drawCodeOpponent(); // draws the pins
            countForDrawCurrentGuess = 0;
        }
    }
    else
    {
        drawCodeOpponent();
    }

    // get the current state of the nunchuk buttons
    static bool previousZ = Nunchuk.state.z_button;
    if (Nunchuk.state.z_button && Nunchuk.state.z_button != previousZ) // if the Z-button went from released to pressed
    {
        sendCodeToOpponent();
        tft.setCursor(65, 175);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(1);
        tft.setRotation(1);
        tft.print("Code succesfully sent to opponent");
        sentCodeToOpponent = true;
    }
    previousZ = Nunchuk.state.z_button;

    if (getNewRecievedBits())
    {
        tft.setCursor(70, 175);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(1);
        tft.setRotation(1);
        tft.print("Code succesfully received");
        colorCodeReceivedFromOpponent = recievedBits;
        recievedCodeFromOpponent = true;
        bitsRead();
    }

    if (sentCodeToOpponent && recievedCodeFromOpponent)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            colorCodeArray[i].currentGameColors = 0;
            colorCodeArray[i].gameColors = gameColorsArray[0];
        }
        setColorCodeReceivedFromOpponentToArray();
        setGameState(GAMEMULTIPLAYER);
        drawPlayingField();
    }

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
        sentCodeToOpponent = false;
        recievedCodeFromOpponent = false;
    }
    previousC = Nunchuk.state.c_button;
}

// function to send the code to the opponent
void sendCodeToOpponent()
{
    sendBits(getColorCodeBinary());
}