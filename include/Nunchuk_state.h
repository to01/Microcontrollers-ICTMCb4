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

// returns one of 5 possible directions (fifth being None)
Direction getNunchukDirection();

// returns a direction but only if the direction has changed
// e.g. if you hold up it will only send up once
Direction getFilteredDirection();

#endif