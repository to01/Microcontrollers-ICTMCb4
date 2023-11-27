#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "Nunchuk.h"
#include "Wire.h"
#include <avr/delay.h>

#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5

volatile bool previousButtonState = 0;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int main(void)
{
  init();
  tft.begin();
  NunChuk Nunchuk;
  Wire.begin();
  Nunchuk.begin(NUNCHUK_ADDRESS);
  Serial.begin(9600);
  uint16_t posX = ILI9341_TFTWIDTH / 2;
  uint16_t posY = ILI9341_TFTHEIGHT / 2;

  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_WHITE);
  tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);

  while (1)
  {
    tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_WHITE);
    Nunchuk.getState(NUNCHUK_ADDRESS);
    posX += (Nunchuk.state.joy_y_axis - 127) / 32;
    posY += (Nunchuk.state.joy_x_axis - 127) / 32;

    if (posX < RADIUS_PLAYER)
    {
      posX = RADIUS_PLAYER;
    }
    else if (posX > ILI9341_TFTWIDTH - RADIUS_PLAYER - 1)
    {
      posX = ILI9341_TFTWIDTH - RADIUS_PLAYER - 1;
    }

    if (posY < RADIUS_PLAYER)
    {
      posY = RADIUS_PLAYER;
    }
    else if (posY > ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1)
    {
      posY = ILI9341_TFTHEIGHT - RADIUS_PLAYER - 1;
    }

    tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);
    _delay_ms(10); // 100FPS
  }
  return 0;
}