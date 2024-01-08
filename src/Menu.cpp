#include "Menu.h"
#include "Make_Code_Opponent.h"

//start menu is the first menu on boot up, it holds the option between singleplayer or multiplayer
MenuItem SingleplayerItem = {"Singleplayer", goToSingleplayerMenu};
MenuItem MultiplayerItem = {"Multiplayer", goToGameMenu};
Menu startMenu = {"Mastermind!", false, {SingleplayerItem, MultiplayerItem}};

// the menu when mulitplayer is selected, this menu holds the option between gamemodes
MenuItem raceAgainstClocKItem = {"Against the clock", drawCodeOpponent};
MenuItem leastMovesItem = {"Least turns wins", goToStartMenu};
Menu gameModeMenu = {"Gamemode", false, {raceAgainstClocKItem, leastMovesItem}};

// the menu when singleplayer is selected, this menu holds the option between difficulty settings
MenuItem easyDifficultyItem = {"easy", goToStartMenu};
MenuItem mediumDifficultyItem = {"medium", goToStartMenu};
MenuItem hardDifficultyItem = {"hard", goToStartMenu};
Menu singlePlayerMenu = {"Difficulty", true, {easyDifficultyItem, mediumDifficultyItem, hardDifficultyItem}};

// the menu holder holds the above menus and keeps track of the current menu
MenuHolder menuHolder = {{startMenu, gameModeMenu, singlePlayerMenu}, 0}; // NOTE: FIND A WAY TO MAKE THE ZERO HERE UNNECESSARY (constructors?)

// prints the title of a menu followed by all the menu items in their respective positions
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

    for (uint8_t i = 0; i < MENU_ARRAY_SIZE; i++) // draw each item
    {
        menu->itemArray[i].yPosition = (YPOS_START + spacer); // store the YPosition of an item to its currect YPosition on the screen;
        drawMenuItem(menu->itemArray[i]);

        spacer += YPOS_SPACER; // increase the value of spacer
    }
    selectMenuItem(menu->itemArray[0]); // mark the first item as selected
    menu->itemSelected = 0;             //select the item at the top of the menu
}

void drawBackground()  //this can be safely removed
{
    tft.fillScreen(BACKGROUNDCOLOUR);
}

// draws the title of an item
void drawMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE, item.yPosition);
    tft.print(item.title);
}

// turns the title of item to white and draws the selection cirle
void selectMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE, item.yPosition);
    tft.fillCircle((XPOS_MIDDLE - SELECT_XPOS), (item.yPosition + SELECT_YPOS), SELECTION_RADIUS, ILI9341_WHITE);
    tft.setTextColor(ILI9341_WHITE);
    tft.print(item.title);
}

// turns the title of item to darkgreen and removes the selection cirle
void deselectMenuItem(MenuItem item)
{
    tft.setCursor(XPOS_MIDDLE, item.yPosition);
    tft.fillCircle((XPOS_MIDDLE - SELECT_XPOS), (item.yPosition + SELECT_YPOS), SELECTION_RADIUS, BACKGROUNDCOLOUR);
    tft.setTextColor(ILI9341_DARKGREEN);
    tft.print(item.title);
}

// check the joystick, if up or down, change what item is selected
void switchMenuItems(Menu *menu, Direction direction)
{
    switch (direction) 
    {
    case Up:
        if (menu->itemSelected > 0)
        {
            deselectMenuItem(menu->itemArray[menu->itemSelected]);
            menu->itemSelected--;
            selectMenuItem(menu->itemArray[menu->itemSelected]);
        }
        break;
    case Down:
        if (menu->itemSelected < 2 && menu->thirdOption)
        {
            deselectMenuItem(menu->itemArray[menu->itemSelected]);
            menu->itemSelected++;
            selectMenuItem(menu->itemArray[menu->itemSelected]);
        }
        else if (menu->itemSelected < 1)
        {
            deselectMenuItem(menu->itemArray[menu->itemSelected]);
            menu->itemSelected++;
            selectMenuItem(menu->itemArray[menu->itemSelected]);
        }
        break;
    default:
        break;
    }
}


// functions called by menuItems VVV
void goToStartMenu()
{
    menuHolder.selectedMenu = 0; // NOTE: DE-MAGIC THIS!
    drawMenu(&menuHolder.MenuArray[menuHolder.selectedMenu]);
}

void goToGameMenu()
{
    menuHolder.selectedMenu = 1; // NOTE: DE-MAGIC THIS!
    drawMenu(&menuHolder.MenuArray[menuHolder.selectedMenu]);
}

void goToSingleplayerMenu()
{
    menuHolder.selectedMenu = 2; // NOTE: DE-MAGIC THIS!
    drawMenu(&menuHolder.MenuArray[menuHolder.selectedMenu]);
}

void mainMenu()
{
    // get the current state of the nunchuk buttons
    static bool previousZ = Nunchuk.state.z_button;
    static bool previousC = Nunchuk.state.c_button;
    if (Nunchuk.state.z_button && Nunchuk.state.z_button != previousZ) // if the Z-button went from released to pressed
    {
        menuHolder.MenuArray[menuHolder.selectedMenu].itemArray[menuHolder.MenuArray[menuHolder.selectedMenu].itemSelected].ButtonAction();
        // take currently selected menu, take the currently selected item of that menu and run that item's buttonaction
    }
    if (Nunchuk.state.c_button && Nunchuk.state.c_button != previousC)
    {
        goToStartMenu();
    }
    previousC = Nunchuk.state.c_button;
    previousZ = Nunchuk.state.z_button;
    switchMenuItems(&menuHolder.MenuArray[menuHolder.selectedMenu], getFilteredDirection());
}