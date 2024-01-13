#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "End_Game.h"
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "Nunchuk_state.h"
#include "IR_Communication.h"
#include "Menu.h"

#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52

#define CLOCKRATE 38000000

const uint16_t ticksPerFrame = (CLOCKRATE / 100000); // 100 FPS = 380 tpf

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
  // tft.fillScreen(BACKGROUNDCOLORGAME);
}

int main(void)
{
  setup();
  Nunchuk.begin(0x52);
  while (1)
  {
    if (ticksSinceLastUpdate > ticksPerFrame) // 100FPS
    {
      switch (getGameState())
      {
      case MENU:
        mainMenu();
        break;
      case GAMECODEOPPONENT:
        codeOpponentLoop(ticksPerFrame);
        break;
      case GAMEMULTIPLAYER:
        multiplayerLoop(ticksPerFrame);
        break;
      case ENDGAME:
        endGameLoop();
        break;
      default:
        break;
      }
    }
  }
  return 0;
}