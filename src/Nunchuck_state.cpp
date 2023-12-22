#include "Nunchuck_state.h"

Direction getNunchuckDirection()
{
    Nunchuk.getState(0x52);
    
}