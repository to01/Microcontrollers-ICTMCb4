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
  }
}