#include <Adafruit_ILI9341.h>

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int main(void)
{
  tft.begin();

  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_BLACK);

  for (int i = 0; i < 150; i++)
  {
    tft.fillCircle(25 + i - 4, 25 + i - 4, 5, ILI9341_BLACK);
    tft.fillCircle(25 + i, 25 + i, 5, ILI9341_BLUE);
    _delay_ms(150);
    i += 3;
#include <Arduino.h>
#include "Nunchuk.h"
#include "Wire.h"

#define BAUDRATE	9600
#define CHUNKSIZE	32
#define BUFFERLEN	256
#define NUNCHUK_ADDRESS 0x52

volatile bool previousButtonState = 0;

int main(void)
{
  init();
  NunChuk Nunchuk;
  Wire.begin();
  Nunchuk.begin(NUNCHUK_ADDRESS);
  Serial.begin(9600);
  while(1)
  {
    Nunchuk.getState(NUNCHUK_ADDRESS);
    if(previousButtonState != Nunchuk.state.c_button)
    {
      //execute something when c is pressed here
    }
    previousButtonState = Nunchuk.state.c_button;
  }
}