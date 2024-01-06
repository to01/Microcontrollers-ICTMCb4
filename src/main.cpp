#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "Nunchuk_state.h"
#include "IR_Communication.h"
#include "Menu.h"

#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_CODEOPPONENT 25
#define RADIUS_CODEOPPONENTGLOW 27

#define FPS 380
#define SELECTEDPINCOUNT 25
#define SELECTPINSPEED 50
#define CHANGECOLORCODEOPPONENTCOUNT 3
#define BACKGROUNDCOLORGAME 0x0500

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

extern Menu startMenu;
extern Menu gameModeMenu;
extern Menu singlePlayerMenu;
extern MenuHolder menuHolder;

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
  tft.setRotation(1);
  drawMenu(&menuHolder.MenuArray[menuHolder.selectedMenu]);
}

int main(void)
{
  setup();
  while (1)
  {
    if (ticksSinceLastUpdate > FPS) // 100FPS
    {
      mainMenu();
    }
  }
  return 0;
}