#ifndef COLORCODE_LOGIC_H
#define COLORCODE_LOGIC_H

#include "Queue.h"
#include "End_Game.h"

extern uint16_t colorCodeForOpponent; // variable to store the color code for the opponent

extern uint8_t colorCodeForOpponentArray[4]; // array used for the feedback displayed on screen

extern uint16_t colorCodeReceivedFromOpponent; // variable to store the color code received from the opponent

extern uint8_t colorCodeReceivedFromOpponentArray[4]; // array to set every pin to the correct color

extern uint16_t guessFromOpponent; // variable to store the guess received from the opponent

extern uint8_t guessFromOpponentArray[4]; // array to set every pin to the correct color

extern Queue previousGuessQueue; // queue for the previous guesses

extern Queue previousGuessOpponentQueue; // queue for the previous guesses from the opponent

uint16_t getColorCodeBinary(void); // function to get the color code in binary

void setColorCodeReceivedFromOpponentToArray(void); // function to set the color code received from the opponent to an array

void setGuessFromOpponentToArray(void); // function to set the guess received from the opponent to an array

void storePreviousGuess(void); // function to store the previous guess

void storePreviousGuessOpponent(void); // function to store the previous guess from the opponent

void checkIfGuessedCodeIsCorrect(void); // function to check if the guessed code is correct

void checkIfGuessedCodeOpponentIsCorrect(void); // function to check if the guessed code from the opponent is correct

#endif // COLORCODE_LOGIC_H