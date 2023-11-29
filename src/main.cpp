#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Nunchuk.h"

// defines for IR communication
#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10

// defines for nunchuk
#define NUNCHUK_ADDRESS 0x52

volatile bool previousButtonState = 0; // hold the previous buttonstate from the Cbutton of the nunchuk

volatile bool segmentUpdateStatus = 0; // decides the 7-segment display should be updated. (AKA toggled)

/*
  Toggles SegmentUpdateStatus on falling edge IR signal.
*/
ISR(INT0_vect)
{
  segmentUpdateStatus = !segmentUpdateStatus;
}

volatile int toggleCount = TOGGLENUMBER; // initialises togglecount

/*
  Toggles PORTD6 while toggleCount < TOGGLENUMBER, this is used to send IR signals.
*/
ISR(TIMER0_COMPA_vect)
{
  if (toggleCount < TOGGLENUMBER)
  {
    PORTD ^= (1 << PORTD6);
    toggleCount++;
  }
}

/*
  Resets toggleCount if togglecount above or equal to TOGGLENUMBER
*/
volatile void sendSignal()
{
  if (toggleCount >= TOGGLENUMBER)
  {
    toggleCount = 0;
  }
}

/*
  Sends port extender signal to enable or disable
*/
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

void getNunchukState(void)
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if(Nunchuk.state.c_button != previousButtonState)
  {
    PORTD ^= (1 << PD7);
    sendSignal();
    previousButtonState = !previousButtonState;
  }
}

/*
  Starts timer in CTC-mode to be used in IR communication
*/
void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

/*
  Enables interrupts on INT0 (IR reciever) and sets pin D6 to output (IR LED)
*/
void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

/*
  Starts wire communications and clears the 7 segment display
*/
void segmentDisplaySetup(void)
{
  Wire.begin();
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

int main(void)
{
  timerSetup();
  IRSetup();
  sei(); // enable global interrupts
  DDRD |= (1 << DDD7);
  Nunchuk.begin(NUNCHUK_ADDRESS);
  segmentDisplaySetup();

  while (1)
  {
    if (segmentUpdateStatus)
    {
      toggleSegmentDisplay();
      segmentUpdateStatus = 0;
    }
    getNunchukState();
  }
  return 0;
}