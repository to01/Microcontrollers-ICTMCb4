#ifndef _MULTIPLAYER_
#define _MULTIPLAYER_

#include "Adafruit_ILI9341.h"
#include "Queue.h"
#include "Make_Code_Mechanics.h"
#include "IR_Communication.h"
#include "Colorcode_Logic.h"

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

void drawPlayingField(void); // function to draw the playing field

void selectPinMultiplayer(void); // function to change the selected pin

void changeColorPinMultiplayer(void); // function to change the color of the current pin

void drawCodeMultiplayer(void); // function to draw the code

void drawPreviousGuess(void); // function to draw the previous guesses

void inputCodeMultiplayer(void); // function to input the code

void giveFeedbackGuess(void); // function to give feedback

void giveFeedbackGuessOpponent(void); // function to draw the feedback from the opponent

void multiplayerLoop(const uint16_t ticksPerFrame); // function called repeatedly by main loop

#endif // _MULTIPLAYER_