#include <Adafruit_ILI9341.h>

#define TFT_DC 9
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int main(void)
{
  tft.begin();

  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_BLACK);
  tft.fillCircle(25, 25, 25, ILI9341_BLUE);
}