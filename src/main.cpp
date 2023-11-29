#include <Adafruit_ILI9341.h>
#include "Nunchuk.h"
#include "Wire.h"

#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5

volatile bool previousButtonState = 0;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0;

ISR(TIMER0_COMPA_vect)
{
  ticksSinceLastUpdate++;
}

void updateDisplay(uint16_t *posXp, uint16_t *posYp)
{
  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_WHITE);
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

  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_BLUE);
}

void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

int main(void)
{
  timerSetup();
  sei();
  tft.begin();
  NunChuk Nunchuk;
  Wire.begin();
  Nunchuk.begin(NUNCHUK_ADDRESS);
  uint16_t posX = ILI9341_TFTWIDTH / 2;
  uint16_t posY = ILI9341_TFTHEIGHT / 2;
  uint16_t *posXp = &posX;
  uint16_t *posYp = &posY;

  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_WHITE);
  tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);

  while (1)
  {
    if (ticksSinceLastUpdate > 380) // 100FPS
    {
      updateDisplay(posXp, posYp);
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}