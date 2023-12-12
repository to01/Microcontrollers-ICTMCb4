#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10 //add comment here
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5

 //add comment here, what is TFT? and what are TFT_CS and DC?
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0;

//volatile bool segmentUpdateStatus = 0; //no longer used

volatile bool previousCButtonState = 0; //used to store the state of the nunchuck Cbutton

//removed ISR(INT0). segmentUpdate is no longer used
//removed ISR(PCINT1_vect) because we no longer use the gameshield button

volatile int toggleCount = TOGGLENUMBER; //what does this do?

ISR(TIMER0_COMPA_vect)
{
  if (toggleCount < TOGGLENUMBER)
  {
    PORTD ^= (1 << PORTD6); //toggle IR pin
    toggleCount++;
  }
  ticksSinceLastUpdate++;
}

/*  NOTE:
  this function includes Nunchuk.getState.
  should this be called every so many frames? or should getState be in the main loop?
  currently, i use this getState to update the 7seg in updateSegmentDisplay()
*/

/*
  update the position of the pawn on the lcd screen
*/
void updateDisplay(uint16_t *posXp, uint16_t *posYp)
{
  uint16_t oldPosX = *posXp;
  uint16_t oldPosY = *posYp;
  Nunchuk.getState(NUNCHUK_ADDRESS);
  *posXp += (Nunchuk.state.joy_y_axis - 127) / 32;
  *posYp += (Nunchuk.state.joy_x_axis - 127) / 32;

  if (*posXp < RADIUS_PLAYER)
  {
    *posXp = RADIUS_PLAYER;
  }
  else if (*posXp > ILI9341_TFTWIDTH - RADIUS_PLAYER - 1)
  {
    *posXp = ILI9341_TFTWIDTH - RADIUS_PLAYER - 1;
  }

  if (*posYp < RADIUS_PLAYER)
  {
    *posYp = RADIUS_PLAYER;
  }
  else if (*posYp > ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1)
  {
    *posYp = ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1;
  }

  tft.fillCircle(oldPosX, oldPosY, RADIUS_PLAYER, ILI9341_WHITE);
  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_BLUE);
}

//comment here
volatile void sendSignal()
{
  if (toggleCount >= TOGGLENUMBER)
  {
    toggleCount = 0;
  }
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

void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

/*
  Will ensure the 7seg display starts OFF
*/
void segmentDisplaySetup(void)
{
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

void setup(void)
{
  timerSetup();
  IRSetup();
  sei();
  Wire.begin();
  segmentDisplaySetup();
  tft.begin();
}

/*
  if the C button on the nunchuk was pressed or released then toggle the 7seg display
*/
void updateSegmentDisplay(void)
{
  if (Nunchuk.state.c_button != previousCButtonState)
  {
    toggleSegmentDisplay();
  }
  previousCButtonState = Nunchuk.state.c_button;
}

int main(void)
{
  setup();
  uint16_t posX = ILI9341_TFTWIDTH / 2;
  uint16_t posY = ILI9341_TFTHEIGHT / 2;
  uint16_t *posXp = &posX;
  uint16_t *posYp = &posY;
  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_WHITE);
  tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate > 380) // 100FPS
    {
      updateDisplay(posXp, posYp);
      ticksSinceLastUpdate = 0;
    }
  }
  return 0; //unreached
}