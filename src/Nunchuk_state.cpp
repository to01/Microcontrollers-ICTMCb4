#include "Nunchuk_state.h"

Direction getNunchukDirection()
{
    Nunchuk.getState(0x52);
    uint8_t up = 0;
    uint8_t right = 0;
    uint8_t down = 0;
    uint8_t left = 0;
    bool rightNotLeft = false;
    bool upNotDown = false;
    if ((Nunchuk.state.joy_x_axis - 127) > 0)
    {
        right = Nunchuk.state.joy_x_axis - 127;
        rightNotLeft = true;
    }
    else
    {
        left = 127 - Nunchuk.state.joy_x_axis;
    }
    if ((Nunchuk.state.joy_y_axis - 127) > 0)
    {
        up = Nunchuk.state.joy_y_axis - 127;
        upNotDown = true;
    }
    else
    {
        down = 127 - Nunchuk.state.joy_y_axis;
    }
    if (right + left > up + down)
    {
        if (right + left > MINDETECTIONVAL)
        {
            if (rightNotLeft)
            {
                return Right;
            }
            return Left;
        }
    }
    else
    {
        if (up + down > MINDETECTIONVAL)
        {
            if (upNotDown)
            {
                return Up;
            }
            return Down;
        }
    }
    return None;
}

Direction getFilteredDirection()
{
    static Direction previousDirection = None;
    Direction currentDirection = getNunchukDirection();
    if (currentDirection != previousDirection)
    {
        previousDirection = currentDirection;
        return currentDirection;
    }
    return None;
}