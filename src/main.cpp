#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "IR_Communication.h"

#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_CODEOPPONENT 25
#define RADIUS_CODEOPPONENTGLOW 27

#define FPS 380
#define SELECTEDPINCOUNT 25
#define SELECTPINSPEED 50
#define CHANGECOLORCODEOPPONENTCOUNT 3
#define BACKGROUNDCOLORGAME 0x0500

// values for the playing field
#define STARTVALUEXGUESS 20
#define STARTVALUEXFEEDBACK 114
#define STARTVALUEY 35
#define STEPVALUE 25
#define STEPVALUEFEEDBACK 12

#pragma region GameColors
// colors whit their corrisponding infraredprotocol code
#define RED 0b0000
#define ORANGE 0b0001
#define YELLOW 0b0010
#define LIGHT_GREEN 0b0011
#define GREEN 0b0100
#define LIGHT_BLUE 0b0101
#define BLUE 0b0110
#define PURPLE 0b0111

// data for the colors in the array
struct GameColors
{
  GameColors(uint8_t colorCode, uint16_t ILI9341Color) : colorCode(colorCode), ILI9341Color(ILI9341Color) {}
  uint8_t colorCode;     // every color has a code for infraredprotocol
  uint16_t ILI9341Color; // color combined to ILI9341Color
};

GameColors gameColorsArray[8] = {
    GameColors(RED, ILI9341_RED),
    GameColors(ORANGE, ILI9341_ORANGE),
    GameColors(YELLOW, ILI9341_YELLOW),
    GameColors(LIGHT_GREEN, ILI9341_GREEN),
    GameColors(GREEN, ILI9341_OLIVE),
    GameColors(LIGHT_BLUE, ILI9341_CYAN),
    GameColors(BLUE, ILI9341_BLUE),
    GameColors(PURPLE, ILI9341_PURPLE)};

#pragma endregion

#pragma region CodeOpponent
// data for the currentPin in the array
struct CodeOpponent
{
  CodeOpponent(GameColors gameColors, uint16_t currentGameColors) : gameColors(gameColors), currentGameColors(currentGameColors) {}
  GameColors gameColors;      // need this to combine the four colours to 1 code for infraredprotocol
  uint16_t currentGameColors; // to display the color on the screen
};

int currentPin = 0;

CodeOpponent codeOpponentArray[4] = {
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0)};

#pragma endregion

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// function to change the color of the current pin
void changeColorPin(uint16_t currentPin)
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if (Nunchuk.state.joy_y_axis > 200)
  {
    if (codeOpponentArray[currentPin].currentGameColors > 0)
    {
      codeOpponentArray[currentPin].currentGameColors--;
    }
    else
    {
      codeOpponentArray[currentPin].currentGameColors = 7;
    }
  }

  if (Nunchuk.state.joy_y_axis < 50)
  {
    if (codeOpponentArray[currentPin].currentGameColors < 7)
    {
      codeOpponentArray[currentPin].currentGameColors++;
    }
    else
    {
      codeOpponentArray[currentPin].currentGameColors = 0;
    }
  }
  codeOpponentArray[currentPin].gameColors = gameColorsArray[codeOpponentArray[currentPin].currentGameColors];
}

int changeColorCodeOpponentCount = 0;

// function to display the new color of the current pin
void changeColorCodeOpponent()
{
  switch (currentPin)
  {
  case 0:
    changeColorPin(currentPin);                                                                                                                                                            // change the color of the current pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
    break;
  case 1:
    changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
    break;
  case 2:
    changeColorPin(currentPin);                                                                                                                                                                // change the color of the current pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
    break;
  case 3:
    changeColorPin(currentPin);                                                                                                                                            // change the color of the current pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draw the current pin
    break;
  }
  changeColorCodeOpponentCount++;
}

// function to draw the glow of the current pin and deletes the glow of the previous pin
void displayCurrentPinCodeOpponent()
{
  if (currentPin == 0)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
  }
  else if (currentPin == 1)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
  }
  else if (currentPin == 2)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
  }
  else if (currentPin == 3)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
  }
}

int selectPinCount = 0;
int selectPinSpeed = 0;

// function to change the selected pin
void selectPin()
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
        displayCurrentPinCodeOpponent();
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
        displayCurrentPinCodeOpponent();
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

// function to draw the first pins whit their color
void drawCodeOpponent()
{
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[0].currentGameColors].ILI9341Color);     // draw the first pin
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[1].currentGameColors].ILI9341Color); // draw the second pin
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[2].currentGameColors].ILI9341Color); // draw the third pin
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[3].currentGameColors].ILI9341Color);                     // draw the fourth pin
  displayCurrentPinCodeOpponent();
}

// function to draw the playing field
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

  for (int i = 0; i < 6; i++)
  {
    for (int g = 0; g < 4; g++)
    {
      tft.fillCircle(STARTVALUEXGUESS + STEPVALUE * g, STARTVALUEY + STEPVALUE * i, 10, ILI9341_WHITE);
      tft.fillCircle(ILI9341_TFTHEIGHT - STARTVALUEXGUESS - STEPVALUE * g, STARTVALUEY + STEPVALUE * i, 10, ILI9341_WHITE);
    }

    for (int f = 0; f < 4; f++)
    {
      tft.fillCircle(STARTVALUEXFEEDBACK + STEPVALUEFEEDBACK * f, STARTVALUEY + STEPVALUE * i, 5, ILI9341_RED);
      tft.fillCircle(ILI9341_TFTHEIGHT - STARTVALUEXFEEDBACK - STEPVALUEFEEDBACK * f, STARTVALUEY + STEPVALUE * i, 5, ILI9341_RED);
    }
  }
  tft.fillCircle(25, 200, 15, ILI9341_WHITE);
  tft.fillCircle(60, 200, 15, ILI9341_WHITE);
  tft.fillCircle(95, 200, 15, ILI9341_WHITE);
  tft.fillCircle(130, 200, 15, ILI9341_WHITE);
}

// function to get the binary code for the infraredprotocol of the color code
// use this function after the player has selected the colors and pressed the send-button
uint16_t getColorCodeBinary()
{
  uint16_t binaryCode = 0;

  for (int i = 0; i < 4; i++) // loop through the pin array
  {
    uint8_t colorCode = codeOpponentArray[i].gameColors.colorCode; // get the color code of the current pin

    for (int j = 0; j < 4; j++) // loop through the color code
    {
      binaryCode = (binaryCode << 1) | ((colorCode >> (3 - j)) & 1); // shift the binary code to the left and add the current bit of the color code
    }
  }
  return binaryCode;
}

void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

void setup(void)
{
  timerSetup();
  IRSetup();
  sei();
  tft.begin();
}

int main(void)
{
  setup();

  tft.fillScreen(ILI9341_WHITE);
  // drawCodeOpponent();
  drawPlayingField();
  while (1)
  {
    // updateSegmentDisplay();
    if (ticksSinceLastUpdate > FPS) // 100FPS
    {
      // selectPin();
      // getColorCodeBinary();

      ticksSinceLastUpdate = 0;
    }

    // if (selectPinCount > SELECTEDPINCOUNT)
    // {
    //   changeColorCodeOpponent();
    //   selectPinCount = 0;
    // }

    // if (changeColorCodeOpponentCount > CHANGECOLORCODEOPPONENTCOUNT)
    // {
    //   drawCodeOpponent();
    //   changeColorCodeOpponentCount = 0;
    // }
  }
  return 0;
}