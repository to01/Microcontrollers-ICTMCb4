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

extern Menu startMenu;
extern Menu gameModeMenu;
extern Menu singlePlayerMenu;
extern MenuHolder menuHolder;

void setup(void)
{
  TCCR0B |= (1 << CS00);   // no prescaler
  sei();
  tft.begin();
  tft.setRotation(1);
}

int main(void)
{
  setup();
  Serial.begin(9600);
  Nunchuk.begin(0x52);
  Direction curdir;
  while (1)
  {
    curdir = getNunchukDirection();
    Serial.println(curdir);
  }
  return 0;
}