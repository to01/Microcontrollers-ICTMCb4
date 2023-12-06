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
#define RADIUS_PLAYER 5

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

void updateDisplay(uint16_t *posXp, uint16_t *posYp)
{
  static int i = 0;
  static int j = 180;
  i++;
  if (i>=100) {
    j--;
    i = 0;
    tft.setCursor(0,0);
    tft.print(j);
  }
  uint16_t oldPosX = *posXp;
  uint16_t oldPosY = *posYp;
  Nunchuk.getState(NUNCHUK_ADDRESS);
  *posXp += (Nunchuk.state.joy_x_axis - 127) / 32;
  *posYp -= (Nunchuk.state.joy_y_axis - 127) / 32;

  if (*posXp < RADIUS_PLAYER)
  {
    *posXp = RADIUS_PLAYER;
  }
  else if (*posXp > ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1)
  {
    *posXp = ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1;
  }

  if (*posYp < RADIUS_PLAYER)
  {
    *posYp = RADIUS_PLAYER;
  }
  else if (*posYp > ILI9341_TFTWIDTH - RADIUS_PLAYER - 1)
  {
    *posYp = ILI9341_TFTWIDTH - RADIUS_PLAYER - 1;
  }

  tft.fillCircle(oldPosX, oldPosY, RADIUS_PLAYER, ILI9341_WHITE);
  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_BLUE);
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
  uint16_t posX = ILI9341_TFTHEIGHT / 2;
  uint16_t posY = ILI9341_TFTWIDTH / 2;
  uint16_t *posXp = &posX;
  uint16_t *posYp = &posY;
  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_WHITE);
  tft.setRotation(1);
  // tft.drawChar(ILI9341_TFTHEIGHT/2, 0, '5', ILI9341_BLACK, ILI9341_WHITE, 3);
  tft.setCursor(0,0);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setTextSize(2);
  tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate >= 380) // 100FPS
    {
      updateDisplay(posXp, posYp);
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}