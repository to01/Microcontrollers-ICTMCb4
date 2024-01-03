#ifndef _CHOOSE_CODE_OPPONENT_
#define _CHOOSE_CODE_OPPONENT_

#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define TFT_DC 9
#define TFT_CS 10

#define BACKGROUNDCOLORGAME 0x0500

#define RADIUS_CODEOPPONENT 25  // radius of the pins
#define HEIGHT_CODEOPPONENT 120 // height of the pins

void changeColorCodeOpponent(void); // function to change the color of the current pin

void selectPinCodeOpponent(void); // function to select the pin

void drawCodeOpponent(void); // function to draw the pins

void codeOpponentLoop(const uint16_t ticksPerFrame); // function repeatedly called by main loop

uint16_t getColorCodeBinary(void); // function to get the color code in binary

#endif // _CHOOSE_CODE_OPPONENT_