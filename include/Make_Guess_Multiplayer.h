#ifndef _MAKE_GUESS_MULTIPLAYER_
#define _MAKE_GUESS_MULTIPLAYER_

#include "Adafruit_ILI9341.h"
#include "Queue.h"
#include "Make_Code_Mechanics.h"
#include "IR_Communication.h"
#include "End_Game.h"

#define TFT_DC 9
#define TFT_CS 10

// values for the playing field
#define STARTVALUEXGUESS 20
#define STARTVALUEXFEEDBACK 114
#define STARTVALUEY 35
#define STEPVALUE 25
#define STEPVALUEFEEDBACK 12
#define VALUEYCURRENTGUESS 200
#define RADIUSPREVIOUSGEUSS 10
#define RADIUSFEEDBACK 5
#define RADIUSCURRENTGUESS 15
#define STARTVALUECURRENTGUESS 25
#define GAPCURRENTGEUSS 35

extern Adafruit_ILI9341 tft; // define the tft screen

extern Queue previousGuessQueue; // queue for the previous guesses

extern uint16_t colorCodeReceivedFromOpponent; // variable to store the color code received from the opponent

void drawPlayingField(void); // function to draw the playing field

void selectPinMultiplayer(void); // function to change the selected pin

void changeColorPinMultiplayer(void); // function to change the color of the current pin

void drawCodeMultiplayer(void); // function to draw the code

void storePreviousGuessMultiplayer(void); // function to store the previous guess

void drawPreviousGuessMultiplayer(void); // function to draw the previous guesses

void inputCodeMultiplayer(void); // function to input the code

void setColorCodeReceivedFromOpponentToArray(void); // function to set the color code received from the opponent to an array

void giveFeedbackMultiplayer(void); // function to give feedback

void checkIfGuessedCodeIsCorrectMultiplayer(void); // function to check if the guessed code is correct

void multiplayerLoop(const uint16_t ticksPerFrame); // function called repeatedly by main loop

#endif // _MAKE_GUESS_MULTIPLAYER_