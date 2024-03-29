#ifndef _CHOOSE_CODE_OPPONENT_
#define _CHOOSE_CODE_OPPONENT_

#include "IR_Communication.h"
#include "Make_Code_Mechanics.h"
#include "GameState.h"
#include "Multiplayer.h"

#define TFT_DC 9
#define TFT_CS 10

#define BACKGROUNDCOLORGAME 0x0500

#define RADIUS_CODEOPPONENT 25  // radius of the pins
#define HEIGHT_CODEOPPONENT 120 // height of the pins

void changeColorCodeOpponent(Direction direction); // function to change the color of the current pin

void selectPinCodeOpponent(void); // function to select the pin

void drawCodeOpponent(void); // function to draw the pins

void codeOpponentLoop(const uint16_t ticksPerFrame); // function repeatedly called by main loop

void sendCodeToOpponent(void); // function to send the code to the opponent

#endif // _CHOOSE_CODE_OPPONENT_