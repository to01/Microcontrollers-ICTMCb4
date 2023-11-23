#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

ISR(INT0_vect) {
  PORTB ^= (1<<PORTB4);
}

ISR(TIMER0_COMPA_vect) {
  PORTD ^= (1<<PORTD6);
}

void timerSetup(void) {
  TIMSK0 |= (1<<OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1<<WGM01); // CTC-mode
  OCR0A = 210; // set TOP to 210
  TCCR0B |= (1<<CS00); // no prescaler
}

int main(void) {
  timerSetup();
  EIMSK |= (1<<INT0); // enable external INT0 interrupts
  EICRA |= (1<<ISC00); // interrupt on any logical change
  DDRB |= (1<<DDB4); // set LED pin output
  DDRD |= (1<<DDD6); // set IR pin output
  sei();
  int i = 0;
  while (1) {
    _delay_ms(10);
    DDRD ^= (1<<DDD6);
    i++;
    if (i == 10) {
      TCCR0B &= ~(1<<CS00); // stop timer0
      _delay_ms(100);
      i = 0;
      TCCR0B |= (1<<CS00); // start timer0
    }
  }
  return 0;
}