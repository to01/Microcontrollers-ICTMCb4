#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

enum GameState
{
  MENU,
  GAMECODEOPPONENT,
  GAMEMULTIPLAYER,
  ENDGAME
};

GameState getGameState();

void setGameState(GameState newState);

#endif // _GAMESTATE_H_