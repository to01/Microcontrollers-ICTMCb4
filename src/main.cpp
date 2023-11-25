#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <Wire.h>

#define ON 0
#define OFF 0b11111111

volatile bool segmentUpdateStatus = 0;

ISR(INT0_vect) {
  segmentUpdateStatus = 1;
}

volatile int toggleCount = 0;
// volatile int waitCount = 0;

ISR(TIMER0_COMPA_vect) {
  if (toggleCount < 1000) {
    PORTD ^= (1<<PORTD6);
    toggleCount++;
  // } else {
  //   PORTD &= ~(1<<PORTD6);
  //   waitCount++;
  //   if (waitCount > 30000) {
  //     toggleCount = 0;
  //     waitCount = 0;
  //   }
  }
}

void sendSignal() {
  if (toggleCount >= 1000) {
    toggleCount = 0;
  } else {
    sendSignal(); // not sure if this is a good idea but it's fine for now
  }
}

void toggleSegmentDisplay(void) {
  static bool currentStatus = 0;
  Wire.beginTransmission(0x21);
  if (currentStatus) {
    Wire.write(OFF);
  } else {
    Wire.write(ON);
  }
  currentStatus = !currentStatus;
  Wire.endTransmission();
}

void segmentDisplaySetup(void) {
  Wire.begin();
  Wire.write(OFF);
  Wire.endTransmission();
}

void timerSetup(void) {
  TIMSK0 |= (1<<OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1<<WGM01); // CTC-mode
  OCR0A = 210; // set TOP to 210
  TCCR0B |= (1<<CS00); // no prescaler
}

int main(void) {
  segmentDisplaySetup();
  timerSetup();
  EIMSK |= (1<<INT0); // enable external INT0 interrupts
  // EICRA |= (1<<ISC00); // interrupt on any logical change
  EICRA |= (1<<ISC01); // interrupt on falling edge
  DDRD |= (1<<DDD6); // set IR pin output
  sei();
  while (1) {
    if (segmentUpdateStatus) {
      toggleSegmentDisplay();
      segmentUpdateStatus = 0;
    }
  }
  return 0;
}