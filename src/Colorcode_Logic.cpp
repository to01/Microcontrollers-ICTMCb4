#include "Colorcode_Logic.h"

uint16_t colorCodeForOpponent = 0; // variable to store the color code for the opponent

uint8_t colorCodeForOpponentArray[4] = {0, 0, 0, 0}; // array used for the feedback displayed on screen

uint8_t guessFromOpponentArray[4] = {0, 0, 0, 0}; // array to set every pin to the correct color

uint16_t guessFromOpponent = 0; // variable to store the guess received from the opponent

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

// function to set the color code received from the opponent to an array
void setColorCodeReceivedFromOpponentToArray()
{
    colorCodeReceivedFromOpponentArray[0] = colorCodeReceivedFromOpponent >> 12 & 0b1111;
    colorCodeReceivedFromOpponentArray[1] = colorCodeReceivedFromOpponent >> 8 & 0b1111;
    colorCodeReceivedFromOpponentArray[2] = colorCodeReceivedFromOpponent >> 4 & 0b1111;
    colorCodeReceivedFromOpponentArray[3] = colorCodeReceivedFromOpponent & 0b1111;
}

// function to store the previous guesses in a queue
void storePreviousGuess()
{
    GameColors tempArray[4] = {
        colorCodeArray[0].gameColors,
        colorCodeArray[1].gameColors,
        colorCodeArray[2].gameColors,
        colorCodeArray[3].gameColors};

    previousGuessQueue.enqueue(tempArray, 4);
}

// function to set the guess received from the opponent to an array
// guessFromOponent is the received infrared code
void setGuessFromOpponentToArray()
{
    guessFromOpponentArray[0] = guessFromOpponent >> 12 & 0b1111;
    guessFromOpponentArray[1] = guessFromOpponent >> 8 & 0b1111;
    guessFromOpponentArray[2] = guessFromOpponent >> 4 & 0b1111;
    guessFromOpponentArray[3] = guessFromOpponent & 0b1111;
}

// function to store the previous guesses from the opponent in a queue
void storePreviousGuessOpponent()
{
    GameColors tempArray[4] = {
        gameColorsArray[guessFromOpponentArray[0]],
        gameColorsArray[guessFromOpponentArray[1]],
        gameColorsArray[guessFromOpponentArray[2]],
        gameColorsArray[guessFromOpponentArray[3]]};

    previousGuessOpponentQueue.enqueue(tempArray, 4);
}

// function to check if the guessed code is correct and give the win
void checkIfGuessedCodeIsCorrect()
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

// function to check if the guessed code from the opponent is correct and give the win
void checkIfGuessedCodeOpponentIsCorrect()
{
    uint8_t correctGuessesOpponent = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        if (guessFromOpponentArray[i] == colorCodeForOpponentArray[i])
        {
            correctGuessesOpponent++;
        }
    }

    if (correctGuessesOpponent == 4)
    {
        showLoser();
    }
}