#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_CODEOPPONENT 25
#define RADIUS_CODEOPPONENTGLOW 27

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

// data for the nodes in the linked list
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

struct CodeOpponent
{
  GameColors gameColors;    // need this to combine the four colours to 1 code for infraredprotocol
  uint16_t indexGameColors; // to display the color on the screen
  CodeOpponent(GameColors gameColors, uint16_t indexGameColors) : gameColors(gameColors), indexGameColors(indexGameColors) {}
};

int currentPin = 0;

// GameColors codeOpponentArray[4];

CodeOpponent codeOpponentArray[4] = {
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0),
    CodeOpponent(GameColors(RED, ILI9341_RED), 0)};

#pragma endregion

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0;

volatile bool segmentUpdateStatus = 0;

ISR(INT0_vect)
{
  segmentUpdateStatus = !segmentUpdateStatus;
}

volatile int toggleCount = TOGGLENUMBER;

ISR(TIMER0_COMPA_vect)
{
  if (toggleCount < TOGGLENUMBER)
  {
    PORTD ^= (1 << PORTD6);
    toggleCount++;
  }
  ticksSinceLastUpdate++;
}

void changeColorPin(uint16_t currentPin)
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if (Nunchuk.state.joy_y_axis > 200)
  {
    if (codeOpponentArray[currentPin].indexGameColors > 0)
    {
      codeOpponentArray[currentPin].indexGameColors--;
    }
    else
    {
      codeOpponentArray[currentPin].indexGameColors = 7;
    }
  }

  if (Nunchuk.state.joy_y_axis < 50)
  {
    if (codeOpponentArray[currentPin].indexGameColors < 7)
    {
      codeOpponentArray[currentPin].indexGameColors++;
    }
    else
    {
      codeOpponentArray[currentPin].indexGameColors = 0;
    }
  }
  codeOpponentArray[currentPin].gameColors = gameColorsArray[codeOpponentArray[currentPin].indexGameColors];
  // tft.setCursor(0, 0);
  // tft.setTextSize(1);
  // tft.setTextColor(ILI9341_BLACK);
  // tft.fillRect(0, 0, 240, 20, ILI9341_WHITE);
  // tft.print(codeOpponentArray[currentPin].indexGameColors);
}

void selectPin(uint16_t currentPin)
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if (Nunchuk.state.joy_x_axis > 200)
  {
    if (currentPin < 3)
    {
      currentPin++;
    }
    else
    {
      currentPin = 3;
    }
  }

  if (Nunchuk.state.joy_x_axis < 50)
  {
    if (currentPin > 0)
    {
      currentPin--;
    }
    else
    {
      currentPin = 0;
    }
  }
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 20, ILI9341_WHITE);
  tft.print(currentPin);
}

void changeColorCodeOpponent()
{
  if (currentPin == 0)
  {
    changeColorPin(currentPin);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
  }
  else if (currentPin == 1)
  {
    changeColorPin(currentPin);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
  }
  else if (currentPin == 2)
  {
    changeColorPin(currentPin);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
  }
  else if (currentPin == 3)
  {
    changeColorPin(currentPin);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
  }
}

void makeCodeOpponent()
{
  selectPin(currentPin);

  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[0].indexGameColors].ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[1].indexGameColors].ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[2].indexGameColors].ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[3].indexGameColors].ILI9341Color);

  if (currentPin == 0)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentPin == 1)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentPin == 2)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentPin == 4)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, gameColorsArray[codeOpponentArray[currentPin].indexGameColors].ILI9341Color);
    changeColorCodeOpponent();
  }
}

volatile void sendSignal()
{
  if (toggleCount >= TOGGLENUMBER)
  {
    toggleCount = 0;
  }
}

ISR(PCINT1_vect)
{
  sendSignal();
}

void toggleSegmentDisplay(void)
{
  static bool currentStatus = 0;
  Wire.beginTransmission(SEVENSEGMENTADDR);
  if (currentStatus)
  {
    Wire.write(OFF);
  }
  else
  {
    Wire.write(ON);
  }
  currentStatus = !currentStatus;
  Wire.endTransmission();
}

void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

void buttonSetup(void)
{
  PORTC |= (1 << PORTC1);
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT9);
}

void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

void segmentDisplaySetup(void)
{
  Wire.begin();
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

void setup(void)
{
  timerSetup();
  buttonSetup();
  IRSetup();
  sei();
  segmentDisplaySetup();
  tft.begin();
}

void updateSegmentDisplay(void)
{
  if (segmentUpdateStatus)
  {
    toggleSegmentDisplay();
    segmentUpdateStatus = 0;
  }
}

int main(void)
{
  setup();

  tft.fillScreen(ILI9341_WHITE);
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate > 380) // 100FPS
    {
      // updateDisplay(posXp, posYp);
      // changeColor(currentColor);
      makeCodeOpponent();
      // changeColorCodeOpponent();
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}