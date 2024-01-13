#ifndef _IR_COMMUNICATION_
#define _IR_COMMUNICATION_

#include <avr/io.h>
#include <avr/interrupt.h>

#define TOGGLENUMBER 38           // number of times the IR emitter toggles to send one bit
#define INITIALONEDURATION 684    // 9ms
#define INITIALZERODURATION 342   // 4.5ms
#define DATALENGTH 32             // total amount of bits sent, including logical inverse
#define ALLOWEDINITIALVARIANCE 32 // allowed variance for initial one and zero

extern volatile uint16_t ticksSinceLastUpdate; // used to refresh display at a consistent rate

extern volatile uint16_t toggleCount; // keeps track of the number of toggles by IR emitter

extern volatile uint16_t dataToSend; // 16 bit integer sent via IR

/*
  used to track which bit should be sent,
  maximum value 32 unless sending new bits, in which case it is set to 64
*/
extern volatile uint8_t bitTurn;

/*
  returns either 1 or 0 depending on bitTurn, if bitTurn is 16 or above it will send the inverse of the first 16 bits
*/
volatile bool getDigitToSend();

volatile void sendZero(void);

volatile void sendOne(void);

extern volatile bool sendingIR;

extern volatile bool recievingIR;

void sendIR(void);

/*
  Returns status of IR reciever, inverted to correspond with sent bits
*/
bool getRecieverStatus(void);

extern uint16_t readCount; // number of times certain things have been read, used in different ways in recieveIR()

enum recieveStatus
{
  initialOne,
  initialZero,
  dataBits,
  inverseBits
};

extern recieveStatus currentRecieveStatus; // used to decern the current status of recieveIR()
extern uint16_t recievedBits;

void resetRecieveIR(void); // resets all values needed in recieveIR to their starting values

/*
  Function used by TIMER0 to recieve IR
*/
void recieveIR(void);

bool getNewRecievedBits(void);

void bitsRead(void);

bool sendBits(uint16_t bitsToSend);

uint16_t getTicksSinceLastUpdate(void);

void setTicksSinceLastUpdate(uint16_t ticksSinceLastUpdate);

#endif /*_IR_COMMUNICATION_*/