#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "IR_Communication.h"
#include "Choose_Code_Opponent.h"

#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256

#define FPS 380
#define BACKGROUNDCOLORGAME 0x0500

// values for the playing field
#define STARTVALUEXGUESS 20
#define STARTVALUEXFEEDBACK 114
#define STARTVALUEY 35
#define STEPVALUE 25
#define STEPVALUEFEEDBACK 12

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
  drawCodeOpponent();
  // drawPlayingField();
  while (1)
  {
    // updateSegmentDisplay();
    if (ticksSinceLastUpdate > FPS) // 100FPS
    {
      selectPin();
      // getColorCodeBinary();

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