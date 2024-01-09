#include "IR_Communication.h"

/*
  used to track which bit should be sent,
  maximum value 32 unless sending new bits, in which case it is set to 64
*/

volatile uint16_t ticksSinceLastUpdate = 0; // used to refresh display at a consistent rate

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

ISR(INT0_vect)
{
  if (!sendingIR)
  {
    recievingIR = true;
  }
}

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

/*
  Returns status of IR reciever, inverted to correspond with sent bits
*/
bool getRecieverStatus(void)
{
  return !((PIND >> PIND2) % 2);
}

uint16_t readCount = 0;                          // number of times certain things have been read, used in different ways in recieveIR()
recieveStatus currentRecieveStatus = initialOne; // used to decern the current status of recieveIR()
uint16_t recievedBits = 0;

void resetRecieveIR(void) // resets all values needed in recieveIR to their starting values
{
  recievingIR = false;
  currentRecieveStatus = initialOne;
  readCount = 0;
}

bool newRecievedBits = false;

/*
  Function used by TIMER0 to recieve IR
*/
void recieveIR(void)
{
  static bool previousValue;
  static uint8_t bitCount;     // used to track the number of bits read so far
  static uint16_t currentBits; // temporary variable to keep track of bits

  switch (currentRecieveStatus)
  {
  case initialOne: // checks for 9ms 1
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
  case initialZero: // checks for 4.5ms 0
    if (!(readCount < INITIALZERODURATION - ALLOWEDINITIALVARIANCE))
    {
      if (readCount > INITIALZERODURATION || getRecieverStatus())
      {
        currentRecieveStatus = dataBits;
        readCount = 0;
        bitCount = 0;
        currentBits = 0;
      }
    }
    else
    {
      if (getRecieverStatus())
      {
        resetRecieveIR();
      }
    }
    readCount++;
    break;
  case dataBits: // reads data bits
    if (readCount == TOGGLENUMBER / 2)
    {
      currentBits = (currentBits << 1);
      currentBits |= previousValue;
      previousValue = getRecieverStatus();
    }
    else if (readCount == TOGGLENUMBER)
    {
      readCount = 0;
      bitCount++;
    }
    readCount++;
    if (bitCount > 16)
    {
      currentRecieveStatus = inverseBits;
      readCount = 0;
      bitCount = 0;
    }
    break;
  case inverseBits: // currently only used for resetting and setting recievedBits, might be used to check inverse later
    recievedBits = currentBits;
    newRecievedBits = true;
    resetRecieveIR();
    break;
  }
}

bool getNewRecievedBits(void)
{
  return newRecievedBits;
}

void bitsRead(void)
{
  newRecievedBits = false;
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
    if (recievingIR)
    {
      recieveIR();
    }
  }
  ticksSinceLastUpdate++;
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

uint16_t getTicksSinceLastUpdate(void)
{
  return ticksSinceLastUpdate;
}

void setTicksSinceLastUpdate(uint16_t newTicksSinceLastUpdate)
{
  ticksSinceLastUpdate = newTicksSinceLastUpdate;
}