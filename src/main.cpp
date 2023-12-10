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

void moveBall(double xBall, double yBall, double xEnd, double yEnd, double angle)
{
  if (xEnd > xBall && yEnd > yBall)
  {
    // x omhoog
    // y omhoog

    if (yEnd > ILI9341_TFTHEIGHT)
    {
      // x = (y - b) / m
      double m = (yBall - yEnd) / (xBall - xEnd);
      double b = -(m * xEnd);
      yEnd = ILI9341_TFTHEIGHT;
      double ybMinus = (yEnd - b);
      xEnd = (ybMinus / m) / 2;
    }

    tft.fillCircle(xEnd, yEnd, RADIUS_BALL, ILI9341_GREEN);
  }
  else if (xEnd < xBall && yEnd > yBall)
  {
    // x omlaag
    // y omhoog

    if (xEnd < 0)
    {
      // y = mx + b
      double m = (yBall - yEnd) / (xBall - xEnd);
      double b = -(m * xEnd);
      xEnd = 0;
      yEnd = b + ILI9341_TFTHEIGHT;
    }

    tft.fillCircle(xEnd, yEnd, RADIUS_BALL, ILI9341_ORANGE);
  }
  else if (xEnd > xBall && yEnd < yBall)
  {
    // x omhoog
    // y omlaag

    if (xEnd > ILI9341_TFTWIDTH)
    {
      // y = mx + b
      double m = (yBall - yEnd) / (xBall - xEnd);
      double b = -(m * xEnd);
      xEnd = ILI9341_TFTWIDTH;
      double mx = (m * xEnd);
      yEnd = mx + b;
    }

    tft.fillCircle(xEnd, yEnd, RADIUS_BALL, ILI9341_DARKCYAN);
  }
  else if (xEnd < xBall && yEnd < yBall)
  {
    // x omlaag
    // y omlaag

    if (yEnd < 0)
    {
      // x = (y - b) / m
      double m = (yBall - yEnd) / (xBall - xEnd);
      double b = yEnd;
      yEnd = 0;
      double ybMinus = (yEnd - b);
      xEnd = (ybMinus / m);
    }

    tft.fillCircle(xEnd, yEnd, RADIUS_BALL, ILI9341_BLACK);
  }
}

// Calculates the distance between two coordinates
double calculateDistance(int x1, int y1, int x2, int y2)
{
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Draws a triangle to calculate the direction and endpoint for the ball after a collision with a player
void drawTriangleAfterCollision(int xBall, int yBall, int xPlayer, int yPlayer, bool cosinus)
{
  double ballXplayerY = calculateDistance(xBall, yBall, xBall, yPlayer);
  double playerXballX = calculateDistance(xPlayer, yPlayer, xBall, yBall);

  double sinusCalc = asin(ballXplayerY / playerXballX);
  double cosinusCalc = acos(ballXplayerY / playerXballX);

  double angleSinus = sinusCalc * (180 / PI);
  double angleCosinus = cosinusCalc * (180 / PI);

  if (cosinus)
  {
    int xPB = calculateDistance(xPlayer, yPlayer, xBall, yPlayer); // calculates distance between x-cord player and x-cord ball
    int xBK = calculateDistance(xPlayer, yPlayer, 240, yPlayer);   // calculates distance between x-cord player and wall
    double scale = xBK / xPB;                                      // calculates scale for reference
    int heigthTriangle = ((yBall - yPlayer) * scale) + yBall;      // calculates endpoint for ball to wall after collision with player
    if (xPlayer <= xBall && yPlayer <= yBall)
    {
      tft.drawTriangle(240, heigthTriangle, 240, yBall, xBall, yBall, ILI9341_GREEN);
      moveBall(xBall, yBall, 240, heigthTriangle, angleCosinus);
    }
    else
    {
      tft.drawTriangle(0, heigthTriangle, 0, yBall, xBall, yBall, ILI9341_BLACK);
      moveBall(xBall, yBall, 0, heigthTriangle, angleCosinus);
    }
  }
  else
  {
    int yPB = calculateDistance(xPlayer, yPlayer, xPlayer, yBall); // calculates distance between y-cord player and y-cord ball
    int yBK = calculateDistance(xPlayer, yPlayer, xPlayer, 320);   // calculates distance between y-cord player and wall
    double scale = yBK / yPB;                                      // calculates scale for reference
    int heigthTriangle = ((xBall - xPlayer) * scale) + xBall;      // calculates endpoint for ball to wall after collision with player
    if (xPlayer >= xBall && yPlayer <= yBall)
    {
      tft.drawTriangle(heigthTriangle, 320, xBall, 320, xBall, yBall, ILI9341_ORANGE);
      moveBall(xBall, yBall, heigthTriangle, 320, angleSinus);
    }
    else
    {
      tft.drawTriangle(heigthTriangle, 0, xBall, 0, xBall, yBall, ILI9341_DARKCYAN);
      moveBall(xBall, yBall, heigthTriangle, 0, angleSinus);
    }
  }
}

void detectCollision(uint16_t *posXp, uint16_t *posYp, uint16_t *posXb, uint16_t *posYb)
{
  //// midden player
  // uint16_t oldPosXp = *posXp;
  // uint16_t oldPosYp = *posYp;
  //// midden ball
  // uint16_t oldPosXb = *posXb;
  // uint16_t oldPosYb = *posYb;

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

  // tft.fillCircle(oldPosXb, oldPosYb, RADIUS_BALL, ILI9341_WHITE);
  // tft.fillCircle(*posXb, *posYb, RADIUS_BALL, ILI9341_RED);
}

void movePlayer(uint16_t *posXp, uint16_t *posYp)
{
  uint16_t oldPosXp = *posXp;
  uint16_t oldPosYp = *posYp;
  Nunchuk.getState(NUNCHUK_ADDRESS);
  *posXp += (Nunchuk.state.joy_y_axis - 127) / 64; // / 64 for testing
  *posYp += (Nunchuk.state.joy_x_axis - 127) / 64; // / 64 for testing

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
    detectCollision(posXp, posYp, posXb, posYb);
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