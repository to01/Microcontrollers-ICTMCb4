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
  tft.fillCircle(posX, posY, 5, ILI9341_BLUE);

  while (1)
  {
    tft.fillCircle(posX, posY, 5, ILI9341_WHITE);
    Nunchuk.getState(NUNCHUK_ADDRESS);
    posX += (Nunchuk.state.joy_y_axis-127)/32;
    posY += (Nunchuk.state.joy_x_axis-127)/32;
    tft.fillCircle(posX, posY, 5, ILI9341_BLUE);
    _delay_ms(10); // 100FPS

    // tft.fillCircle(posX, posY, 5, ILI9341_BLACK);
    // tft.fillCircle(posX += 0.5, posY, 5, ILI9341_BLUE);
    // Nunchuk.getState(NUNCHUK_ADDRESS);
    // if (Nunchuk.state.joy_y_axis > 128)
    // {
    //   tft.fillCircle(posX, posY, 5, ILI9341_BLACK);
    //   tft.fillCircle(posX += 0.5, posY, 5, ILI9341_BLUE);
    // }
    // else if (Nunchuk.state.joy_y_axis < 128)
    // {
    //   tft.fillCircle(posX, posY, 5, ILI9341_BLACK);
    //   tft.fillCircle(posX -= 0.5, posY, 5, ILI9341_BLUE);
    // }
    // else if (Nunchuk.state.joy_x_axis > 128)
    // {
    //   tft.fillCircle(posX, posY, 5, ILI9341_BLACK);
    //   tft.fillCircle(posX, posY += 0.5, 5, ILI9341_BLUE);
    // }
    // else if (Nunchuk.state.joy_x_axis < 128)
    // {
    //   tft.fillCircle(posX, posY, 5, ILI9341_BLACK);
    //   tft.fillCircle(posX, posY -= 0.5, 5, ILI9341_BLUE);
    // }
  }
  return 0;
}