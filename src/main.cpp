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
}

int main(void)
{
  setup();

  drawMenu(&menuHolder.MenuArray[menuHolder.selectedMenu]);

  // menuHolder.MenuArray[menuHolder.selectedMenu].itemArray[1].ButtonAction();

  bool previousZ = Nunchuk.state.z_button;
  bool previousC = Nunchuk.state.c_button;
  while (1)
  {
    if (Nunchuk.state.z_button && Nunchuk.state.z_button != previousZ)
    {
      menuHolder.MenuArray[menuHolder.selectedMenu].itemArray[menuHolder.MenuArray[menuHolder.selectedMenu].itemSelected].ButtonAction();
      // take currently selected menu, take the currently selected item of that menu and run that item's buttonaction
    }
    if (Nunchuk.state.c_button && Nunchuk.state.c_button != previousC)
    {
      goToStartMenu();
    }
    previousC = Nunchuk.state.c_button;
    previousZ = Nunchuk.state.z_button;
    if (ticksSinceLastUpdate > FPS * 10) // 100FPS
    {
      switchMenuItems(&menuHolder.MenuArray[menuHolder.selectedMenu], getFilteredDirection());
    }
  }
  return 0;
}