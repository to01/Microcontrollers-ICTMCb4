#include <Arduino.h>
#include "Nunchuk.h"
#include "Wire.h"

#define BAUDRATE	9600
#define CHUNKSIZE	32
#define BUFFERLEN	256
#define NUNCHUK_ADDRESS 0x52

volatile bool previousButtonState = 0;

int main(void)
{
  init();
  NunChuk Nunchuk;
  Wire.begin();
  Nunchuk.begin(NUNCHUK_ADDRESS);
  Serial.begin(9600);
  while(1)
  {
    Nunchuk.getState(NUNCHUK_ADDRESS);
    if(previousButtonState != Nunchuk.state.c_button)
    {
      //execute something when c is pressed here
    }
    previousButtonState = Nunchuk.state.c_button;
  }
}