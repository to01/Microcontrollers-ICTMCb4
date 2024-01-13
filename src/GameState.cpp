#include "GameState.h"

GameState gameState = MENU;

GameState getGameState()
{
    return gameState;
}

void setGameState(GameState newState)
{
    gameState = newState;
}