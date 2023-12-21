#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define TOGGLENUMBER 38 // number of times the IR emitter toggles to send one bit
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
#define INITIALONEDURATION 684    // 9ms
#define INITIALZERODURATION 342   // 4.5ms
#define DATALENGTH 32             // total amount of bits sent, including logical inverse
#define ALLOWEDINITIALVARIANCE 32 // allowed variance for initial one and zero

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0; // used to refresh display at a consistent rate

volatile uint16_t toggleCount = 0; // keeps track of the number of toggles by IR emitter

volatile uint16_t dataToSend; // 16 bit integer sent via IR

/*
  used to track which bit should be sent,
  maximum value 32 unless sending new bits, in which case it is set to 64
*/
volatile uint8_t bitTurn;

/*
  returns either 1 or 0 depending on bitTurn, if bitTurn is 16 or above it will send the inverse of the first 16 bits
*/
volatile bool getDigitToSend()
{
  if (bitTurn >= 16)
  {
    return !((dataToSend >> (15 - (bitTurn - 16))) % 2);
  }
  return ((dataToSend >> (15 - bitTurn)) % 2);
}

volatile void sendZero(void)
{
  PORTD &= ~(1 << PORTD6);
}

volatile void sendOne(void)
{
  PORTD ^= (1 << PORTD6);
}

volatile bool sendingIR = false;

volatile bool recievingIR = false;

ISR(INT0_vect)
{
  if (!sendingIR)
  {
    recievingIR = true;
  }
}

void sendIR(void)
{
  if (bitTurn < DATALENGTH || bitTurn == 64)
  {
    if (bitTurn == 64) // if communication start
    {
      if (toggleCount < INITIALONEDURATION) // send 1 for 9ms
      {
        sendOne();
      }
      else if (toggleCount < INITIALONEDURATION + INITIALZERODURATION) // then send 0 for 4.5ms
      {
        sendZero();
      }
      else // then start sending data
      {
        bitTurn = 0;
        toggleCount = 0;
      }
    }
    else
    {
      if (toggleCount >= TOGGLENUMBER) // reset togglecount and go to next bit if togglecount reaches TOGGLENUMBER
      {
        toggleCount = 0;
        bitTurn++;
      }
      if (getDigitToSend())
      {
        sendOne();
      }
      else
      {
        sendZero();
      }
    }
    toggleCount++;
  }
  else
  { // When done sending
    sendingIR = false;
  }
}

/*
  Returns status of IR reciever, inverted to correspond with sent bits
*/
bool getRecieverStatus(void)
{
  return !((PIND >> PIND2) % 2);
}

uint16_t readCount = 0; // number of times certain things have been read, used in different ways in recieveIR()

enum recieveStatus
{
  initialOne,
  initialZero,
  dataBits,
  inverseBits
};

recieveStatus currentRecieveStatus = initialOne; // used to decern the current status of recieveIR()
uint16_t recievedBits = 0;

void resetRecieveIR(void) // resets all values needed in recieveIR to their starting values
{
  recievingIR = false;
  currentRecieveStatus = initialOne;
  readCount = 0;
}

/*
  Function used by TIMER0 to recieve IR
*/
void recieveIR(void)
{
  static bool previousValue;
  static uint8_t bitCount; // used to track the number of bits read so far 
  static uint16_t currentBits; // temporary variable to keep track of bits

  switch (currentRecieveStatus)
  {
  case initialOne: // checks for 9ms 1
    if (readCount < INITIALONEDURATION - ALLOWEDINITIALVARIANCE)
    {
      if (getRecieverStatus())
      {
        readCount++;
      }
      else
      {
        resetRecieveIR();
      }
    }
    else
    {
      if (readCount > INITIALONEDURATION)
      {
        resetRecieveIR();
      }
      else if (!getRecieverStatus())
      {
        currentRecieveStatus = initialZero;
        readCount = 0;
      }
    }
    break;
  case initialZero: // checks for 4.5ms 0 
    if (!(readCount < INITIALZERODURATION - ALLOWEDINITIALVARIANCE))
    {
      if (readCount > INITIALZERODURATION || getRecieverStatus())
      {
        currentRecieveStatus = dataBits;
        readCount = 0;
        bitCount = 0;
        currentBits = 0;
      }
    }
    readCount++;
    break;
  case dataBits: // reads data bits
    if (readCount == TOGGLENUMBER / 2)
    {
      currentBits = (currentBits << 1);
      currentBits |= previousValue;
      previousValue = getRecieverStatus();
    }
    else if (readCount == TOGGLENUMBER)
    {
      readCount = 0;
      bitCount++;
    }
    readCount++;
    if (bitCount > 16)
    {
      currentRecieveStatus = inverseBits;
      readCount = 0;
      bitCount = 0;
    }
    break;
  case inverseBits: // currently only used for resetting and setting recievedBits, might be used to check inverse later
    recievedBits = currentBits;
    resetRecieveIR();
    break;
  }
}

ISR(TIMER0_COMPA_vect)
{
  if (sendingIR)
  {
    sendIR();
  }
  else if (bitTurn >= DATALENGTH) // makes sure the IR emitter is off while idle
  {
    sendZero();
    if (recievingIR)
    {
      recieveIR();
    }
  }
  ticksSinceLastUpdate++;
}

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
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                         // delete the glow of the previous pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draws the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[1].currentGameColors].ILI9341Color);      // draws the previous pin
  }
  else if (currentPin == 1)
  {
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                                 // delete the glow of the previous pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                             // delete the glow of the previous pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draws the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[0].currentGameColors].ILI9341Color);              // draws the previous pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[2].currentGameColors].ILI9341Color);          // draws the previous pin
  }
  else if (currentPin == 2)
  {
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                             // delete the glow of the previous pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                                                 // delete the glow of the previous pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color); // draws the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[1].currentGameColors].ILI9341Color);          // draws the previous pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[3].currentGameColors].ILI9341Color);                              // draws the previous pin
  }
  else if (currentPin == 3)
  {
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);                                                    // delete the glow of the previous pin
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE); // draws the glow of the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].currentGameColors].ILI9341Color);            // draws the current pin
    // tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[2].currentGameColors].ILI9341Color); // draws the previous pin
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
  DDRD |= (1 << DDD6); // set IR pin output
}

void setup(void)
{
  timerSetup();
  IRSetup();
  sei();
  tft.begin();
}

/*
  Sets the variables needed to send bits, returns false if not possible (e.g. when sending or recieving IR)
*/
bool sendBits(uint16_t bitsToSend)
{
  if (!sendingIR && !recievingIR)
  {
    dataToSend = bitsToSend;
    bitTurn = 64;
    sendingIR = true;
    return true;
  }
  return false;
}

int main(void)
{
  setup();

  tft.fillScreen(ILI9341_WHITE);
  drawCodeOpponent();
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate > FPS) // 100FPS
    {
      selectPin();
      getColorCodeBinary();
      ticksSinceLastUpdate = 0;
    }

    if (selectPinCount > SELECTEDPINCOUNT)
    {
      changeColorCodeOpponent();
      selectPinCount = 0;
    }

    if (changeColorCodeOpponentCount > CHANGECOLORCODEOPPONENTCOUNT)
    {
      drawCodeOpponent();
      changeColorCodeOpponentCount = 0;
    }
  }
  return 0;
}