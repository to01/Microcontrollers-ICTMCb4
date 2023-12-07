#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"
#include "math.h"

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5
#define RADIUS_BALL 3

// position ball
uint16_t posXBall = ILI9341_TFTWIDTH / 2;
uint16_t posYBall = ILI9341_TFTHEIGHT / 2;
uint16_t *posXb = &posXBall;
uint16_t *posYb = &posYBall;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0;

volatile bool segmentUpdateStatus = 0;

ISR(INT0_vect)
{
  segmentUpdateStatus = !segmentUpdateStatus;
}

volatile int toggleCount = TOGGLENUMBER;

ISR(TIMER0_COMPA_vect)
{
  if (toggleCount < TOGGLENUMBER)
  {
    PORTD ^= (1 << PORTD6);
    toggleCount++;
  }
  ticksSinceLastUpdate++;
}

// Calculates the distance between two coordinates
double calculateDistance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Draws a triangle to calculate the direction end endpoint for the ball after a collision with a player
void drawTriangleAfterCollision(int xBall, int yBall, int xPlayer, int yPlayer, bool cosinus)
{
  if (cosinus)
  {
    int xPB = calculateDistance(xPlayer, yPlayer, xBall, yPlayer);
    int xBK = calculateDistance(xPlayer, yPlayer, 240, yPlayer);
    double scale = xBK / xPB;
    int heigthTriangle = ((yBall - yPlayer) * scale) + yBall;
    if (xPlayer < xBall && yPlayer < yBall)
    {
      tft.drawTriangle(240, heigthTriangle, 240, yBall, xBall, yBall, ILI9341_GREEN);
    }
    else
    {
      tft.drawTriangle(0, heigthTriangle, 0, yBall, xBall, yBall, ILI9341_GREEN);
    }
  }
  else
  {
    int yPB = calculateDistance(xPlayer, yPlayer, xPlayer, yBall);
    int yBK = calculateDistance(xPlayer, yPlayer, xPlayer, 320);
    double scale = yBK / yPB;
    int heigthTriangle = ((xBall - xPlayer) * scale) + xBall;
    if (xPlayer > xBall && yPlayer < yBall)
    {
      tft.drawTriangle(heigthTriangle, 320, xBall, 320, xBall, yBall, ILI9341_ORANGE);
    }
    else
    {
      tft.drawTriangle(heigthTriangle, 0, xBall, 0, xBall, yBall, ILI9341_ORANGE);
    }
  }
}

void moveBall(uint16_t *posXp, uint16_t *posYp, uint16_t *posXb, uint16_t *posYb)
{
  // midden player
  uint16_t oldPosXp = *posXp;
  uint16_t oldPosYp = *posYp;
  // midden ball
  uint16_t oldPosXb = *posXb;
  uint16_t oldPosYb = *posYb;

  double ballXplayerY = calculateDistance(oldPosXb, oldPosYb, oldPosXb, oldPosYp);
  double playerXballX = calculateDistance(oldPosXp, oldPosYp, oldPosXb, oldPosYb);

  double sinus = asin(ballXplayerY / playerXballX);
  double cosinus = acos(ballXplayerY / playerXballX);

  double angleSinus = sinus * (180 / PI);
  double angleCosinus = cosinus * (180 / PI);

  if (*posXp <= *posXb && *posYp <= *posYb)
  {
    // cos
    drawTriangleAfterCollision(*posXb, *posYb, *posXp, *posYp, true);
  }

  if (*posXp >= *posXb && *posYp <= *posYb)
  {
    // sin
    drawTriangleAfterCollision(*posXb, *posYb, *posXp, *posYp, false);
  }

  if (*posXp <= *posXb && *posYp >= *posYb)
  {
    // sin
    drawTriangleAfterCollision(*posXb, *posYb, *posXp, *posYp, false);
  }

  if (*posXp >= *posXb && *posYp >= *posYb)
  {
    // cos
    drawTriangleAfterCollision(*posXb, *posYb, *posXp, *posYp, true);
  }

  if (*posXb < RADIUS_BALL)
  {
    *posXb = RADIUS_BALL;
  }
  else if (*posXb > ILI9341_TFTWIDTH - RADIUS_BALL - 1)
  {
    *posXb = ILI9341_TFTWIDTH - RADIUS_BALL - 1;
  }

  if (*posYb < RADIUS_BALL)
  {
    *posYb = RADIUS_BALL;
  }
  else if (*posYb > ILI9341_TFTHEIGHT - RADIUS_BALL - 1)
  {
    *posYb = ILI9341_TFTHEIGHT - RADIUS_BALL - 1;
  }

  tft.fillCircle(oldPosXb, oldPosYb, RADIUS_BALL, ILI9341_WHITE);
  tft.fillCircle(*posXb, *posYb, RADIUS_BALL, ILI9341_RED);
}

void movePlayer(uint16_t *posXp, uint16_t *posYp)
{
  uint16_t oldPosXp = *posXp;
  uint16_t oldPosYp = *posYp;
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

  if (*posXp + RADIUS_PLAYER + RADIUS_BALL >= *posXb && *posXp - RADIUS_PLAYER - RADIUS_BALL <= *posXb && *posYp + RADIUS_PLAYER + RADIUS_BALL >= *posYb && *posYp - RADIUS_PLAYER - RADIUS_BALL <= *posYb)
  {
      moveBall(posXp, posYp, posXb, posYb);
  }

  tft.fillCircle(oldPosXp, oldPosYp, RADIUS_PLAYER, ILI9341_WHITE);
  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_BLUE);
}

volatile void sendSignal()
{
  if (toggleCount >= TOGGLENUMBER)
  {
    toggleCount = 0;
  }
}

ISR(PCINT1_vect)
{
  sendSignal();
}

void toggleSegmentDisplay(void)
{
  static bool currentStatus = 0;
  Wire.beginTransmission(SEVENSEGMENTADDR);
  if (currentStatus)
  {
    Wire.write(OFF);
  }
  else
  {
    Wire.write(ON);
  }
  currentStatus = !currentStatus;
  Wire.endTransmission();
}

void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

void buttonSetup(void)
{
  PORTC |= (1 << PORTC1);
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT9);
}

void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

void segmentDisplaySetup(void)
{
  Wire.begin();
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

void setup(void)
{
  timerSetup();
  buttonSetup();
  IRSetup();
  sei();
  segmentDisplaySetup();
  tft.begin();
}

void updateSegmentDisplay(void)
{
  if (segmentUpdateStatus)
  {
    toggleSegmentDisplay();
    segmentUpdateStatus = 0;
  }
}

int main(void)
{
  setup();
  // position player
  uint16_t posXPlayer = ILI9341_TFTWIDTH / 2;
  uint16_t posYPlayer = ILI9341_TFTHEIGHT / 4;
  uint16_t *posXp = &posXPlayer;
  uint16_t *posYp = &posYPlayer;

  tft.fillScreen(ILI9341_WHITE);
  tft.fillCircle(posXPlayer, posYPlayer, RADIUS_PLAYER, ILI9341_BLUE);
  tft.fillCircle(posXBall, posYBall, RADIUS_BALL, ILI9341_RED);
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate > 380) // 100FPS
    {
      movePlayer(posXp, posYp);
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}