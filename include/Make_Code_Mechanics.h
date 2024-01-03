#ifndef _MAKE_CODE_MECHANICS_
#define _MAKE_CODE_MECHANICS_

#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define BACKGROUNDCOLORGAME 0x0500 // background color of the game

// colors whit their corrisponding infraredprotocol code
#define RED 0b0000
#define ORANGE 0b0001
#define YELLOW 0b0010
#define LIGHT_GREEN 0b0011
#define GREEN 0b0100
#define LIGHT_BLUE 0b0101
#define BLUE 0b0110
#define PURPLE 0b0111

#define SELECTEDPINCOUNT 25            // amount of times per 100fps the value of the joystick is read
#define SELECTPINSPEED 50              // speed at which the pins are switched
#define CHANGECOLORCODEOPPONENTCOUNT 3 // amount of times per selectedpincount the value of the joystick is red to change the color of a pin

extern uint8_t currentPin; // current pin that is selected

extern uint8_t changeColorCodeOpponentCount; // used to change the color of the current pin

extern uint8_t selectPinCount; // used to select the pin

extern uint8_t selectPinSpeed; // used to select the pin

struct GameColors
{
    GameColors(uint8_t colorCode, uint16_t ILI9341Color) : colorCode(colorCode), ILI9341Color(ILI9341Color) {}
    uint8_t colorCode;     // every color has a code for infraredprotocol
    uint16_t ILI9341Color; // color combined to ILI9341Color
};

extern GameColors gameColorsArray[8]; // data for the colors in the array

struct ColorCode
{
    ColorCode(GameColors gameColors, uint8_t currentGameColors) : gameColors(gameColors), currentGameColors(currentGameColors) {}
    GameColors gameColors;     // need this to combine the four colours to 1 code for infraredprotocol
    uint8_t currentGameColors; // to display the color on the screen
};

extern ColorCode colorCodeArray[4]; // data for the currentPin in the array

extern Adafruit_ILI9341 tft; // define the tft screen

void changeColorPin(uint8_t currentPin); // function to change the color of the current pin

void blinkCurrentPin(uint8_t valueXCurrentPin, uint16_t valueYCurrentPin, uint8_t valueRadiusCurrentPin); // function to blink the current pin

uint16_t getColorCodeBinary(void); // function to get the color code in binary

#endif // _MAKE_CODE_MECHANICS_