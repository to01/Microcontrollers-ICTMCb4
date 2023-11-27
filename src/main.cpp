#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <Wire.h>

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10

volatile bool segmentUpdateStatus = 0;

ISR(INT0_vect) {
  segmentUpdateStatus = 1;
}

volatile int toggleCount = TOGGLENUMBER;

ISR(TIMER0_COMPA_vect) {
  if (toggleCount < TOGGLENUMBER) {
    PORTD ^= (1<<PORTD6);
    toggleCount++;
  }
}

volatile void sendSignal() {
  if (toggleCount >= TOGGLENUMBER) {
    toggleCount = 0;
  }
}

ISR(PCINT1_vect) {
  sendSignal();
}

void toggleSegmentDisplay(void) {
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

void timerSetup(void) {
  TIMSK0 |= (1<<OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1<<WGM01); // CTC-mode
  OCR0A = 210; // set TOP to 210
  TCCR0B |= (1<<CS00); // no prescaler
}

void buttonSetup(void) {
  PORTD |= (1<<PORTC1);
  PCICR |= (1<<PCIE1);
  PCMSK1 |= (1<<PCINT9);
}

void segmentDisplaySetup(void) {
  Wire.begin();
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

int main(void) {
  timerSetup();
  buttonSetup();
  EIMSK |= (1<<INT0); // enable external INT0 interrupts
  EICRA |= (1<<ISC01); // interrupt on falling edge
  DDRD |= (1<<DDD6); // set IR pin output
  sei();
  segmentDisplaySetup();
  while (1) {
    if (segmentUpdateStatus) {
      toggleSegmentDisplay();
      segmentUpdateStatus = 0;
    }
  }
  return 0;
}