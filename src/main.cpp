#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "IR_Communication.h"
#include "Make_Code_Mechanics.h"
#include "Make_Code_Opponent.h"
#include "Multiplayer.h"
#include "Game_Timer.h"
#include "End_Game.h"

#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256

#define CLOCKRATE 38000000

const uint16_t ticksPerFrame = FPS * (CLOCKRATE / 10000000); // 100 FPS = 380 tpf

enum GameState
{
  MENUPLAYERS,
  MENUGAMEMODE,
  GAMECODEOPPONENT,
  GAMEMULTIPLAYER,
  ENDGAME
};

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
  tft.fillScreen(BACKGROUNDCOLORGAME);
}

int main(void)
{
  setup();

  drawPlayingField();
  //  drawCodeOpponent();
  while (1)
  {
    multiplayerLoop(ticksPerFrame);
    //  codeOpponentLoop(ticksPerFrame);
  }
  return 0;
}