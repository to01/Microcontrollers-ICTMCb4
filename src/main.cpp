#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "Menu.h"

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

extern Menu startMenu;
extern Menu gameModeMenu;
extern Menu singlePlayerMenu;

/*
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
}*/

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
  drawMenu(&startMenu);
  startMenu.itemArray[0].ButtonAction();

  while (1)
  {

  }
  return 0;
}