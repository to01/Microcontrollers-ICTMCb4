#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Nunchuk.h"

//defines for IR communication
#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10

//defines for nunchuk
#define NUNCHUK_ADDRESS 0x52

volatile bool segmentUpdateStatus = 0;
volatile bool previousButtonState = 0;

// ISR for the IR reciever 
ISR(INT0_vect) 
{
  segmentUpdateStatus = !segmentUpdateStatus;
}

//add comment here and for the ISR beneath -Ruben
volatile int toggleCount = TOGGLENUMBER;

ISR(TIMER0_COMPA_vect) 
{
  if (toggleCount < TOGGLENUMBER) {
    PORTD ^= (1<<PORTD6);
    toggleCount++;
  }
}

volatile void sendSignal() 
{
  if (toggleCount >= TOGGLENUMBER) {
    toggleCount = 0;
  }
}

/* for gameshield button. can be deleted
ISR(PCINT1_vect) 
{
  sendSignal();
}
*/

void toggleSegmentDisplay(void) 
{
  static bool currentStatus = 0;
  Wire.beginTransmission(SEVENSEGMENTADDR);
  if (currentStatus) {
    Wire.write(OFF);
  } else {
    Wire.write(ON);
  }
  currentStatus = !currentStatus;
  Wire.endTransmission();
}

void timerSetup(void) 
{
  TIMSK0 |= (1<<OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1<<WGM01); // CTC-mode
  OCR0A = 210; // set TOP to 210
  TCCR0B |= (1<<CS00); // no prescaler
}

void nunchukSetup(void)
{
  NunChuk nunchuk;
  nunchuk.begin(NUNCHUK_ADDRESS);
}

/* setup for gameshield button. can be deleted
void buttonSetup(void) {
  PORTD |= (1<<PORTD1);
  PCICR |= (1<<PCIE1);
  PCMSK1 |= (1<<PCINT9);
}
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
  nunchukSetup();
  timerSetup();
 // buttonSetup(); to be deleted

 //what does this do specifically? does it detect incoming IR? -Ruben
  EIMSK |= (1<<INT0); // enable external INT0 interrupts
  EICRA |= (1<<ISC01); // interrupt on falling edge
  DDRD |= (1<<DDD6); // set IR pin output
  sei(); //enable global interrupts

  segmentDisplaySetup();

  while (1) {
    Nunchuk.getState(NUNCHUK_ADDRESS);
    if(previousButtonState != Nunchuk.state.c_button)
    {
      sendSignal();
    }
    if (segmentUpdateStatus) 
    {
      toggleSegmentDisplay();
      segmentUpdateStatus = 0;
    }
  }
  return 0;
}