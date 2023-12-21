#ifndef PLAYER_MENU_H_
#define PLAYER_MENU_H_

/*  DESCRIPTION
    the sccreen where players pick whether to play singleplayer or multiplayer
*/

#include "Menu_Headers/Super_Menu_Screen.h"

class Player_Menu : public Super_Menu_Screen
{
public:
    Player_Menu(Adafruit_ILI9341* tft);  //con
    ~Player_Menu(); //dec
private:


};



#endif