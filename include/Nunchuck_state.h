#ifndef NUNCHUCK_STATE_H_
#define NUNCHUCK_STATE_H_

#include "Nunchuk.h"

enum Direction
{
    Up,
    Right,
    Down,
    Left
};

Direction getNunchuckDirection();


#endif