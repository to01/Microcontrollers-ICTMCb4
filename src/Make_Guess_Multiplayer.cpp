#include "Make_Guess_Multiplayer.h"

Queue previousGuessQueue = Queue(); // queue for the previous guesses

uint16_t colorCodeReceivedFromOpponent = 0x0101; // variable to store the color code received from the opponent

uint8_t colorCodeReceivedFromOpponentArray[4] = {0, 0, 0, 0};

// function to draw the playing field and initialize the color code
void drawPlayingField()
{
    tft.fillScreen(BACKGROUNDCOLORGAME);
    tft.fillRect(0, ILI9341_TFTHEIGHT / 2 - 1, ILI9341_TFTWIDTH - 20, 3, ILI9341_WHITE);
    tft.drawRect(ILI9341_TFTWIDTH - 20, ILI9341_TFTHEIGHT / 2 - 25, 21, 53, ILI9341_WHITE);
    tft.drawRect(ILI9341_TFTWIDTH - 21, ILI9341_TFTHEIGHT / 2 - 26, 22, 55, ILI9341_WHITE);
    tft.drawRect(ILI9341_TFTWIDTH - 22, ILI9341_TFTHEIGHT / 2 - 27, 23, 57, ILI9341_WHITE);

    tft.setCursor(ILI9341_TFTHEIGHT / 2 - 21, 3);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setRotation(1);
    tft.print("3:00");

    for (uint8_t i = 0; i < 6; i++)
    {
        for (uint8_t pg = 0; pg < 4; pg++)
        {
            tft.fillCircle(STARTVALUEXGUESS + STEPVALUE * pg, STARTVALUEY + STEPVALUE * i, RADIUSPREVIOUSGEUSS, ILI9341_WHITE);
            tft.fillCircle(ILI9341_TFTHEIGHT - STARTVALUEXGUESS - STEPVALUE * pg, STARTVALUEY + STEPVALUE * i, RADIUSPREVIOUSGEUSS, ILI9341_WHITE);
        }

        for (uint8_t f = 0; f < 4; f++)
        {
            tft.fillCircle(STARTVALUEXFEEDBACK + STEPVALUEFEEDBACK * f, STARTVALUEY + STEPVALUE * i, RADIUSFEEDBACK, ILI9341_RED);
            tft.fillCircle(ILI9341_TFTHEIGHT - STARTVALUEXFEEDBACK - STEPVALUEFEEDBACK * f, STARTVALUEY + STEPVALUE * i, RADIUSFEEDBACK, ILI9341_RED);
        }
    }

    for (uint8_t cg = 0; cg < 4; cg++)
    {
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * cg, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[cg].currentGameColors].ILI9341Color);
    }

    setColorCodeReceivedFromOpponentToArray();
}

// function to change the selected pin
void selectPinMultiplayer()
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
                blinkCurrentPin(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS);
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
                blinkCurrentPin(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS);
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

// function to display the new color of the current pin
void changeColorPinMultiplayer()
{
    switch (currentPin)
    {
    case 0:
        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    case 1:
        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;

    case 2:

        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;

    case 3:
        changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
        break;
    }
    changeColorCodeOpponentCount++;
}

// function to draw the code
void drawCodeMultiplayer()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        tft.fillCircle(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * i, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS, gameColorsArray[colorCodeArray[i].currentGameColors].ILI9341Color);
    }
    blinkCurrentPin(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS);
}

// function to store the previous guesses in a queue
void storePreviousGuessMultiplayer()
{
    GameColors tempArray[4] = {
        colorCodeArray[0].gameColors,
        colorCodeArray[1].gameColors,
        colorCodeArray[2].gameColors,
        colorCodeArray[3].gameColors};

    previousGuessQueue.enqueue(tempArray, 4);
}

// function to draw the previous guesses
void drawPreviousGuessMultiplayer()
{
    for (uint8_t i = 0; i < previousGuessQueue.rear + 1; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            tft.fillCircle(STARTVALUEXGUESS + STEPVALUE * j, STARTVALUEY + STEPVALUE * i, RADIUSPREVIOUSGEUSS, previousGuessQueue.queue[i][j]);
        }
    }
}

// function to input the code with a nunchukbutton
void inputCodeMultiplayer()
{
    if (Nunchuk.state.z_button > 0)
    {
        storePreviousGuessMultiplayer();
        drawPreviousGuessMultiplayer();
        giveFeedbackMultiplayer();
        sendBits(getColorCodeBinary()); // sends the guess to the opponent
        currentPin = 0;

        blinkCurrentPin(STARTVALUECURRENTGUESS + GAPCURRENTGEUSS * currentPin, VALUEYCURRENTGUESS, RADIUSCURRENTGUESS);
    }
}

// function to set the color code received from the opponent to an array
void setColorCodeReceivedFromOpponentToArray()
{
    // uint8_t colorCodeReceivedFromOpponentArray[4] = {0, 0, 0, 0};

    colorCodeReceivedFromOpponentArray[0] = colorCodeReceivedFromOpponent >> 12 & 0b1111;
    colorCodeReceivedFromOpponentArray[1] = colorCodeReceivedFromOpponent >> 8 & 0b1111;
    colorCodeReceivedFromOpponentArray[2] = colorCodeReceivedFromOpponent >> 4 & 0b1111;
    colorCodeReceivedFromOpponentArray[3] = colorCodeReceivedFromOpponent & 0b1111;
}

// function to check if the code is correct and to give feedback
void giveFeedbackMultiplayer()
{
    uint8_t feedbackArray[4] = {0, 0, 0, 0};

    for (uint8_t i = 0; i < 4; i++)
    {
        if (colorCodeReceivedFromOpponentArray[i] == colorCodeArray[i].gameColors.colorCode)
        {
            feedbackArray[i] = 2;
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (feedbackArray[i] == 0)
        {
            for (uint8_t j = 0; j < 4; j++)
            {
                if (colorCodeReceivedFromOpponentArray[i] == colorCodeArray[j].gameColors.colorCode && feedbackArray[j] == 0)
                {
                    feedbackArray[j] = 1;
                    break;
                }
            }
        }
    }

    for (uint8_t i = 0; i < 4; i++)
    {
        if (feedbackArray[i] == 0)
        {
            tft.fillCircle(STARTVALUEXFEEDBACK + STEPVALUEFEEDBACK * i, STARTVALUEY + STEPVALUE * previousGuessQueue.rear, RADIUSFEEDBACK, ILI9341_RED);
        }
        else if (feedbackArray[i] == 1)
        {
            tft.fillCircle(STARTVALUEXFEEDBACK + STEPVALUEFEEDBACK * i, STARTVALUEY + STEPVALUE * previousGuessQueue.rear, RADIUSFEEDBACK, ILI9341_YELLOW);
        }
        else if (feedbackArray[i] == 2)
        {
            tft.fillCircle(STARTVALUEXFEEDBACK + STEPVALUEFEEDBACK * i, STARTVALUEY + STEPVALUE * previousGuessQueue.rear, RADIUSFEEDBACK, ILI9341_GREEN);
        }
    }
    checkIfGuessedCodeIsCorrectMultiplayer();
}

// function to check if the guessed code is correct and give the win
void checkIfGuessedCodeIsCorrectMultiplayer()
{
    uint8_t correctGuesses = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        if (colorCodeReceivedFromOpponentArray[i] == colorCodeArray[i].gameColors.colorCode)
        {
            correctGuesses++;
        }
    }

    if (correctGuesses == 4)
    {
        showWinner();
    }
}

// function loop for multiplayerscreen
void loopMultiplayer()
{
    if (ticksSinceLastUpdate > FPS) // 100FPS
    {
        selectPinMultiplayer();

        ticksSinceLastUpdate = 0;
        fpsToSeconds++;

        if (gameSeconds > 0)
        {
            if (fpsToSeconds > FRAMESTOSECONDS)
            {
                gameSeconds--;
                updateTimeMultiplayer(gameSeconds);
                fpsToSeconds = 0;
            }
        }
        else
        {
            // game over
        }
    }

    if (selectPinCount > SELECTEDPINCOUNT)
    {
        changeColorPinMultiplayer();
        inputCodeMultiplayer();

        selectPinCount = 0;
    }

    if (changeColorCodeOpponentCount > CHANGECOLORCODEOPPONENTCOUNT)
    {
        drawCodeMultiplayer();

        changeColorCodeOpponentCount = 0;
    }
}