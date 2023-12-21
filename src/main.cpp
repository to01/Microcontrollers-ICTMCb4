#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define TOGGLENUMBER 38 // number of times the IR emitter toggles to send one bit
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_PLAYER 5
#define INITIALONEDURATION 684    // 9ms
#define INITIALZERODURATION 342   // 4.5ms
#define DATALENGTH 32             // total amount of bits sent, including logical inverse
#define ALLOWEDINITIALVARIANCE 32 // allowed variance for initial one and zero

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0; // used to refresh display at a consistent rate

ISR(INT0_vect)
{
}

volatile uint16_t toggleCount = 0; // keeps track of the number of toggles by IR emitter

volatile uint16_t dataToSend; // 16 bit integer sent via IR

/*
  used to track which bit should be sent,
  maximum value 32 unless sending new bits, in which case it is set to 64
*/
volatile uint8_t bitTurn;

/*
  returns either 1 or 0 depending on bitTurn, if bitTurn is 16 or above it will send the inverse of the first 16 bits
*/
volatile bool getDigitToSend()
{
  if (bitTurn >= 16)
  {
    return !((dataToSend >> (15 - (bitTurn - 16))) % 2);
  }
  return ((dataToSend >> (15 - bitTurn)) % 2);
}

volatile void sendZero(void)
{
  PORTD &= ~(1 << PORTD6);
}

volatile void sendOne(void)
{
  PORTD ^= (1 << PORTD6);
}

volatile bool sendingIR = false;

volatile bool recievingIR = false;

void sendIR(void)
{
  if (bitTurn < DATALENGTH || bitTurn == 64)
  {
    if (bitTurn == 64) // if communication start
    {
      if (toggleCount < INITIALONEDURATION) // send 1 for 9ms
      {
        sendOne();
      }
      else if (toggleCount < INITIALONEDURATION + INITIALZERODURATION) // then send 0 for 4.5ms
      {
        sendZero();
      }
      else // then start sending data
      {
        bitTurn = 0;
        toggleCount = 0;
      }
    }
    else
    {
      if (toggleCount >= TOGGLENUMBER) // reset togglecount and go to next bit if togglecount reaches TOGGLENUMBER
      {
        toggleCount = 0;
        bitTurn++;
      }
      if (getDigitToSend())
      {
        sendOne();
      }
      else
      {
        sendZero();
      }
    }
    toggleCount++;
  }
  else
  { // When done sending
    sendingIR = false;
  }
}

bool getRecieverStatus(void)
{
  return !((PIND >> PIND2) % 2);
}

uint16_t readCount = 0;
enum recieveStatus
{
  initialOne,
  initialZero,
  dataBits,
  inverseBits
};

recieveStatus currentRecieveStatus = initialOne;
uint16_t recievedBits = 0;

void resetRecieveIR(void)
{
  recievingIR = false;
  currentRecieveStatus = initialOne;
  readCount = 0;
}

void recieveIR(void)
{
  static bool previousValue;
  static uint8_t bitCount;
  static uint16_t currentBits;

  switch (currentRecieveStatus)
  {
  case initialOne:
    if (readCount < INITIALONEDURATION - ALLOWEDINITIALVARIANCE)
    {
      if (getRecieverStatus())
      {
        readCount++;
      }
      else
      {
        resetRecieveIR();
      }
    }
    else
    {
      if (readCount > INITIALONEDURATION)
      {
        resetRecieveIR();
      }
      else if (!getRecieverStatus())
      {
        currentRecieveStatus = initialZero;
        readCount = 0;
      }
    }
    break;
  case initialZero:
    if (readCount < INITIALZERODURATION - ALLOWEDINITIALVARIANCE)
    {
      if (!getRecieverStatus())
      {
        readCount++;
      }
      else
      {
        resetRecieveIR();
      }
    }
    else
    {
      if (readCount > INITIALZERODURATION || getRecieverStatus())
      {
        currentRecieveStatus = dataBits;
        readCount = 0;
        // previousValue = getRecieverStatus();
        bitCount = 0;
        currentBits = 0; 
      }
    }
    break;
  case dataBits:
    if (readCount == TOGGLENUMBER/2)
    {
        currentBits = (currentBits << 1);
        currentBits |= previousValue;
        previousValue = getRecieverStatus();
        if (previousValue == 1)
        {
          PORTD |= (1<<PORTD4);
        }
        else
        {
          PORTD &= ~(1<<PORTD4);
        }
    }
    else if (readCount == TOGGLENUMBER)
    {
      readCount = 0;
      bitCount++;
    }
    readCount++;
    // PORTD &= ~(1<<PORTD4);
    // if (readCount >= TOGGLENUMBER)
    // {
    //   currentBits = (currentBits << 1);
    //   currentBits |= previousValue;
    //   bitCount++;
    //   readCount = 0;
    //   previousValue = getRecieverStatus();
    //   if (previousValue == 1)
    //   {
    //     PORTD |= (1<<PORTD4);
    //   }
    //   else
    //   {
    //     PORTD &= ~(1<<PORTD4);
    //   }
    // }
    // else if (previousValue != getRecieverStatus())
    // {
    //   // resetRecieveIR(); // INFO this might be changed later to a more packet-loss friendly alternative
    // }
    // readCount++;
    if (bitCount > 16) {
      currentRecieveStatus = inverseBits;
      readCount = 0;
      bitCount = 0;
    }
    break;
  case inverseBits:
    recievedBits = currentBits;
    resetRecieveIR();
    break;
  }
}

ISR(TIMER0_COMPA_vect)
{
  if (sendingIR)
  {
    sendIR();
  }
  else if (bitTurn >= DATALENGTH) // makes sure the IR emitter is off while idle
  {
    sendZero();
    recievingIR = true; // INFO THIS IS TEMPORARY
    if (recievingIR)
    {
      recieveIR();
    }
  }
  ticksSinceLastUpdate++;
}

void updateDisplay(uint16_t *posXp, uint16_t *posYp)
{
  uint16_t oldPosX = *posXp;
  uint16_t oldPosY = *posYp;
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

  tft.fillCircle(oldPosX, oldPosY, RADIUS_PLAYER, ILI9341_WHITE);
  tft.fillCircle(*posXp, *posYp, RADIUS_PLAYER, ILI9341_BLUE);
}

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
}

/*
  Sets the variables needed to send bits, returns false if not possible (e.g. when sending or recieving IR)
*/
bool sendBits(uint16_t bitsToSend)
{
  if (!sendingIR && !recievingIR)
  {
    dataToSend = bitsToSend;
    bitTurn = 64;
    sendingIR = true;
    return true;
  }
  return false;
}

int main(void)
{
  setup();
  uint16_t posX = ILI9341_TFTWIDTH / 2;
  uint16_t posY = ILI9341_TFTHEIGHT / 2;
  uint16_t *posXp = &posX;
  uint16_t *posYp = &posY;
  tft.fillRect(0, 0, ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, ILI9341_WHITE);
  tft.fillCircle(posX, posY, RADIUS_PLAYER, ILI9341_BLUE);
  sendBits(0b1010101010101010);
  DDRD |= (1 << DDD4);
  Serial.begin(9600);
  while (1)
  {
    if (ticksSinceLastUpdate > 380*1000) // 100FPS
    {
      Serial.println(recievedBits);
      // updateDisplay(posXp, posYp);
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}