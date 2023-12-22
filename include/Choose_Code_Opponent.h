#ifndef _CHOOSE_CODE_OPPONENT_
#define _CHOOSE_CODE_OPPONENT_

// #include <avr/io.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define TFT_DC 9
#define TFT_CS 10

#define BACKGROUNDCOLORGAME 0x0500

// colors whit their corrisponding infraredprotocol code
#define RED 0b0000
#define ORANGE 0b0001
#define YELLOW 0b0010
#define LIGHT_GREEN 0b0011
#define GREEN 0b0100
#define LIGHT_BLUE 0b0101
#define BLUE 0b0110
#define PURPLE 0b0111

#define RADIUS_CODEOPPONENT 25 // radius of the pins

#define SELECTEDPINCOUNT 25            // amount of times per 100fps the value of the joystick is read
#define SELECTPINSPEED 50              // speed at which the pins are switched
#define CHANGECOLORCODEOPPONENTCOUNT 3 // amount of times per selectedpincount the value of the joystick is red to change the color of a pin

extern uint8_t currentPin; // current pin that is selected

extern uint8_t changeColorCodeOpponentCount; // used to change the color of the current pin

extern uint8_t selectPinCount; // used to select the pin

extern uint8_t selectPinSpeed; // used to select the pin

extern Adafruit_ILI9341 tft; // define the tft screen

void changeColorPin(uint16_t currentPin); // function to change the color of the current pin

void changeColorCodeOpponent(void); // function to change the color of the current pin

void displayCurrentPinCodeOpponent(void); // function to display the current pin

void selectPin(void); // function to select the pin

void drawCodeOpponent(void); // function to draw the pins

uint16_t getColorCodeBinary(void); // function to get the color code in binary

#endif // _CHOOSE_CODE_OPPONENT_