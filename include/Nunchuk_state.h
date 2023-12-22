#ifndef NUNCHUCK_STATE_H_
#define NUNCHUCK_STATE_H_

#define MINDETECTIONVAL 64

#include "Nunchuk.h"

enum Direction
{
    Up,
    Right,
    Down,
    Left,
    None
};

Direction getNunchukDirection();


#endif