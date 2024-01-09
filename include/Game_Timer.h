#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include "Multiplayer.h"

#define FRAMESTOSECONDS 180 // 180 frames is 1 second

extern uint8_t fpsToSeconds; // variable to convert the fps to seconds

extern uint8_t gameSeconds; // variable to store the seconds

void updateTimeMultiplayer(uint8_t secondsLeft); // function to update the time

#endif // GAME_TIMER_H