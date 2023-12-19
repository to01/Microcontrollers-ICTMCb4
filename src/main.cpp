#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include "Adafruit_ILI9341.h"
#include "Nunchuk.h"

#define ON 0
#define OFF 0b11111111
#define SEVENSEGMENTADDR 0x21
#define TOGGLENUMBER 10
#define TFT_DC 9
#define TFT_CS 10
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256
#define NUNCHUK_ADDRESS 0x52
#define RADIUS_CODEOPPONENT 25
#define RADIUS_CODEOPPONENTGLOW 27

#pragma region GameColors
// colors whit their corrisponding infraredprotocol code
#define RED 0b0000
#define ORANGE 0b0001
#define YELLOW 0b0010
#define LIGHT_GREEN 0b0011
#define GREEN 0b0100
#define LIGHT_BLUE 0b0101
#define BLUE 0b0110
#define PURPLE 0b0111

// data for the nodes in the linked list
struct GameColors
{
  uint8_t colorCode;     // every color has a code for infraredprotocol
  uint16_t ILI9341Color; // color combined to ILI9341Color
  GameColors *next;      // pointer to the next node
  GameColors *prev;      // pointer to the previous node
};

// create a new node with color and ILI9341Color
GameColors *createNode(uint8_t color, uint16_t ILI9341Color)
{
  GameColors *node = new GameColors; // create a new node
  node->colorCode = color;           // set the code for the infraredprotocol
  node->ILI9341Color = ILI9341Color; // set the color for the ILI9341
  node->next = NULL;                 // set the next pointer to NULL
  node->prev = NULL;                 // set the previous pointer to NULL
  return node;                       // return the new node
}

GameColors *Red = createNode(RED, ILI9341_RED);                  // initialize a new node
GameColors *Orange = createNode(ORANGE, ILI9341_ORANGE);         // initialize a new node
GameColors *Yellow = createNode(YELLOW, ILI9341_YELLOW);         // initialize a new node
GameColors *LightGreen = createNode(LIGHT_GREEN, ILI9341_GREEN); // initialize a new node
GameColors *Green = createNode(GREEN, ILI9341_OLIVE);            // initialize a new node
GameColors *LightBlue = createNode(LIGHT_BLUE, ILI9341_CYAN);    // initialize a new node
GameColors *Blue = createNode(BLUE, ILI9341_BLUE);               // initialize a new node
GameColors *Purple = createNode(PURPLE, ILI9341_PURPLE);         // initialize a new node

// link the nodes to their neighbor to initialize the linked list
void initializeGameColors()
{
  Red->next = Orange;
  Orange->next = Yellow;
  Yellow->next = LightGreen;
  LightGreen->next = Green;
  Green->next = LightBlue;
  LightBlue->next = Blue;
  Blue->next = Purple;
  Purple->next = Red;
  Red->prev = Purple;
  Orange->prev = Red;
  Yellow->prev = Orange;
  LightGreen->prev = Yellow;
  Green->prev = LightGreen;
  LightBlue->prev = Green;
  Blue->prev = LightBlue;
  Purple->prev = Blue;
}
#pragma endregion

#pragma region CodeOpponent
// data for the nodes in the linked list
struct CodeOpponent
{
  GameColors gameColors; // need this to combine the four colours to 1 code for infraredprotocol
  uint16_t ILI9341Color; // to display the color on the screen
  CodeOpponent *next;    // pointer to the next node
  CodeOpponent *prev;    // pointer to the previous node
};

// create a new node with ILI9341Color
CodeOpponent *createCodeOpponentNode(GameColors *gameColors, uint16_t ILI9341Color)
{
  CodeOpponent *node = new CodeOpponent; // create a new node
  node->gameColors = *gameColors;        // set the given node from the GameColors linked list
  node->ILI9341Color = ILI9341Color;     // set the color for the ILI9341
  node->next = NULL;                     // set the next pointer to NULL
  node->prev = NULL;                     // set the previous pointer to NULL
  return node;                           // return the new node
}

CodeOpponent *First = createCodeOpponentNode(Red, ILI9341_RED);  // first option for the code for the opponent
CodeOpponent *Second = createCodeOpponentNode(Red, ILI9341_RED); // second option for the code for the opponent
CodeOpponent *Third = createCodeOpponentNode(Red, ILI9341_RED);  // third option for the code for the opponent
CodeOpponent *Fourth = createCodeOpponentNode(Red, ILI9341_RED); // fourth option for the code for the opponent

// pointer to the current color
CodeOpponent *currentCodeOpponent = First;

// link the nodes to their neighbor to initialize the linked list
void initializeCodeOpponent()
{
  First->next = Second;
  Second->next = Third;
  Third->next = Fourth;
  Second->prev = First;
  Third->prev = Second;
  Fourth->prev = Third;
}
#pragma endregion

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

volatile uint16_t ticksSinceLastUpdate = 0;

volatile bool segmentUpdateStatus = 0;

ISR(INT0_vect)
{
  segmentUpdateStatus = !segmentUpdateStatus;
}

volatile int toggleCount = TOGGLENUMBER;

ISR(TIMER0_COMPA_vect)
{
  if (toggleCount < TOGGLENUMBER)
  {
    PORTD ^= (1 << PORTD6);
    toggleCount++;
  }
  ticksSinceLastUpdate++;
}

// PROBLEM! not looing through the linked list
void changeColor(GameColors node, CodeOpponent *pin)
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if (Nunchuk.state.joy_y_axis > 200)
  {
    node = *node.prev;
  }

  if (Nunchuk.state.joy_y_axis < 50)
  {
    node = *node.next;
  }
  pin->gameColors = node;

  // // code om te kijken of de kleuren uit de linked list goed worden opgehaald
  // tft.setCursor(0, 0);
  // tft.setTextSize(1);
  // tft.setTextColor(ILI9341_BLACK);
  // tft.fillRect(0, 0, 240, 20, ILI9341_WHITE);
  // tft.print("Color: ");
  // tft.print(pin->gameColors.colorCode);
  // tft.print(" ");
  // tft.print(pin->gameColors.ILI9341Color);
  // tft.print(" ");
  // tft.print(node.colorCode);
  // tft.print(" ");
  // tft.print(node.ILI9341Color);

  // tft.fillScreen(pin->ILI9341Color); // deze functie werkt soms wel en soms niet met het wisselen van de gedefinieerde kleuren.
}

void codeOpponent()
{
  Nunchuk.getState(NUNCHUK_ADDRESS);
  if (Nunchuk.state.joy_x_axis > 200)
  {
    if (currentCodeOpponent->next == NULL)
    {
      currentCodeOpponent = Fourth;
    }
    else
    {
      currentCodeOpponent = currentCodeOpponent->next;
    }
  }

  if (Nunchuk.state.joy_x_axis < 50)
  {
    if (currentCodeOpponent->prev == NULL)
    {
      currentCodeOpponent = First;
    }
    else
    {
      currentCodeOpponent = currentCodeOpponent->prev;
    }
  }
}

void changeColorCodeOpponent()
{
  if (currentCodeOpponent == First)
  {
    changeColor(First->gameColors, First);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, First->gameColors.ILI9341Color);
  }
  else if (currentCodeOpponent == Second)
  {
    changeColor(Second->gameColors, Second);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Second->gameColors.ILI9341Color);
  }
  else if (currentCodeOpponent == Third)
  {
    changeColor(Third->gameColors, Third);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Third->gameColors.ILI9341Color);
  }
  else if (currentCodeOpponent == Fourth)
  {
    changeColor(Fourth->gameColors, Fourth);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, Fourth->gameColors.ILI9341Color);
  }
}

void makeCodeOpponent()
{
  codeOpponent();

  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, First->gameColors.ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Second->gameColors.ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Third->gameColors.ILI9341Color);
  tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, Fourth->gameColors.ILI9341Color);

  if (currentCodeOpponent == First)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, First->gameColors.ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentCodeOpponent == Second)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Second->gameColors.ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentCodeOpponent == Third)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 2 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENT, Third->gameColors.ILI9341Color);
    changeColorCodeOpponent();
  }
  else if (currentCodeOpponent == Fourth)
  {
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3 - RADIUS_CODEOPPONENT, RADIUS_CODEOPPONENTGLOW, ILI9341_WHITE);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENTGLOW, ILI9341_CASET);
    tft.fillCircle(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 4 * 3.5, RADIUS_CODEOPPONENT, Fourth->gameColors.ILI9341Color);
    changeColorCodeOpponent();
  }
}

volatile void sendSignal()
{
  if (toggleCount >= TOGGLENUMBER)
  {
    toggleCount = 0;
  }
}

ISR(PCINT1_vect)
{
  sendSignal();
}

void toggleSegmentDisplay(void)
{
  static bool currentStatus = 0;
  Wire.beginTransmission(SEVENSEGMENTADDR);
  if (currentStatus)
  {
    Wire.write(OFF);
  }
  else
  {
    Wire.write(ON);
  }
  currentStatus = !currentStatus;
  Wire.endTransmission();
}

void timerSetup(void)
{
  TIMSK0 |= (1 << OCIE0A); // enable comp match a interrupt
  TCCR0A |= (1 << WGM01);  // CTC-mode
  OCR0A = 210;             // set TOP to 210
  TCCR0B |= (1 << CS00);   // no prescaler
}

void buttonSetup(void)
{
  PORTC |= (1 << PORTC1);
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT9);
}

void IRSetup(void)
{
  EIMSK |= (1 << INT0);  // enable external INT0 interrupts
  EICRA |= (1 << ISC01); // interrupt on falling edge
  DDRD |= (1 << DDD6);   // set IR pin output
}

void segmentDisplaySetup(void)
{
  Wire.begin();
  Wire.beginTransmission(SEVENSEGMENTADDR);
  Wire.write(OFF);
  Wire.endTransmission();
}

void setup(void)
{
  timerSetup();
  buttonSetup();
  IRSetup();
  sei();
  segmentDisplaySetup();
  tft.begin();
}

void updateSegmentDisplay(void)
{
  if (segmentUpdateStatus)
  {
    toggleSegmentDisplay();
    segmentUpdateStatus = 0;
  }
}

int main(void)
{
  setup();

  initializeGameColors();
  initializeCodeOpponent();
  tft.fillScreen(ILI9341_WHITE);
  while (1)
  {
    updateSegmentDisplay();
    if (ticksSinceLastUpdate > 380) // 100FPS
    {
      // updateDisplay(posXp, posYp);
      // changeColor(currentColor);
      makeCodeOpponent();
      ticksSinceLastUpdate = 0;
    }
  }
  return 0;
}