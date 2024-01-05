#include "Menu.h"

//the first menu on boot up
  const char* singleplayerString = "Singleplayer";
  const char* multiplayerString = "Multiplayer";
  const char* startMenuString = "Mastermind!";
  MenuItem SingleplayerItem = {singleplayerString, .ButtonAction = goToSingleplayerMenu};
  MenuItem MultiplayerItem = {multiplayerString, .ButtonAction = goToGameMenu};
  Menu startMenu =  {startMenuString, false, {SingleplayerItem,MultiplayerItem}};
  
  //the menu when mulitplayer is selected
  MenuItem raceAgainstClocKItem = {"Against the clock"};
  MenuItem leastMovesItem = {"Least turns wins"};
  Menu gameModeMenu = {"Gamemode", false, {raceAgainstClocKItem,leastMovesItem}};

  //the menu when singleplayer is selected 
  MenuItem easyDifficultyItem = {"easy"};
  MenuItem mediumDifficultyItem = {"medium"};
  MenuItem hardDifficultyItem = {"hard"};
  Menu singlePlayerMenu = {"Difficulty", true, {easyDifficultyItem,mediumDifficultyItem,hardDifficultyItem}};

  //the menu holder holds the above menus and keeps track of the current menu
  MenuHolder menuHolder = {{startMenu,gameModeMenu,singlePlayerMenu},0}; //NOTE: FIND A WAY TO MAKE THE ZERO HERE UNNECESSARY (constructors?)

//prints the title of a menu followed by all the menu items in their respective positions
void drawMenu(Menu *menu)
{
    drawBackground();
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(TITLE_FONT_SIZE);
    tft.setCursor(XPOS_MIDDLE, YPOS_SPACER);
    tft.print(menu->title);

    uint8_t spacer = YPOS_SPACER;

    tft.setTextSize(ITEM_FONT_SIZE);
    tft.setTextColor(ILI9341_DARKGREEN);

    for (uint8_t i = 0; i < MENU_ARRAY_SIZE; i++) //draw each item
    {
        menu->itemArray[i].yPosition = (YPOS_START + spacer); //store the YPosition of an item to its currect YPosition on the screen
        drawMenuItem(menu->itemArray[i]); 

        if (!menu->thirdOption) // if there is no third option, then skip the middle position.
        {
            spacer = spacer * 2;
        }

        spacer += YPOS_SPACER; //increase the value of spacer
    }
    selectMenuItem(&menu->itemArray[0]); //mark the first item as selected
}

void drawBackground()
{
    tft.fillScreen(BACKGROUNDCOLOUR);
}

//draws the title of an item
void drawMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE,item.yPosition);
    tft.print(item.title);
}

//turns the title of item to white and draws the selection cirle
void selectMenuItem(MenuItem* item)
{
    tft.setCursor(XPOS_MIDDLE,item->yPosition);
    tft.fillCircle((XPOS_MIDDLE - SELECT_XPOS), (item->yPosition + SELECT_YPOS), SELECTION_RADIUS, ILI9341_WHITE);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(item->title);
}

//turns the title of item to darkgreen and removes the selection cirle
void deselectMenuItem(MenuItem* item)
{
    tft.setCursor(XPOS_MIDDLE,item->yPosition);
    tft.fillCircle((XPOS_MIDDLE - SELECT_XPOS), (item->yPosition + SELECT_YPOS), SELECTION_RADIUS, BACKGROUNDCOLOUR);
    tft.setTextColor(ILI9341_DARKGREEN);
    tft.print(item->title);
}

//check the joystick, if up or down, change what item is selected
void switchMenuItems(Menu menu, Direction direction)
{
    switch (direction)
    {
    case Up:
            deselectMenuItem(&menu.itemArray[menu.itemSelected]);
            Serial.println("up");
        break;
    case Down:
        Serial.println("down");
        break;
    default:
        break;
    }
}

//functions called by menuItems VVV
void goToStartMenu()
{
    drawMenu(&startMenu);
    menuHolder.selectedMenu = 0; //NOTE: DE-MAGIC THIS!
}

void goToGameMenu()
{
    drawMenu(&gameModeMenu);
    menuHolder.selectedMenu = 1; //NOTE: DE-MAGIC THIS!
}

void goToSingleplayerMenu()
{
    drawMenu(&singlePlayerMenu);
    menuHolder.selectedMenu = 2; //NOTE: DE-MAGIC THIS!
}