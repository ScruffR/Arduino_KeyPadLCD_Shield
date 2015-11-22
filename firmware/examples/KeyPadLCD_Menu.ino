
/*
  Arduino KeyPad LCD Shield Library - Basic Menu
 
 Demonstrates the use a 16x2 LCD display and the five button KeyPad.
 The LiquidCrystal library works with all LCD displays that are 
 compatible with the Hitachi HD44780 driver. There are many of them out 
 there, and you can usually tell them by the 16-pin interface.
 
 This sketch provides a rudementary implementation of a four item menue 
 in the second LCD row that can be navigated with the LEFT/RIGHT/UP/DOWN 
 buttons and SELECT an item to display in the first row and expose as
 Particle.variable().
 
  Wiring for Particle Shield shield v3.0.1
 * LCD RS pin to digital pin A5
 * LCD EN pin to digital pin A4
 * LCD D4 pin to digital pin D6
 * LCD D5 pin to digital pin D0
 * LCD D6 pin to digital pin D1
 * LCD D7 pin to digital pin D7

 Copyright ScruffR 2015

 http://www.dfrobot.com/wiki/index.php/LCD_KeyPad_Shield_For_Arduino_SKU:_DFR0009
*/

#include "Arduino_KeyPadLCD_Shield/LiquidCrystal.h"

enum struct BUTTONS {
    NONE = 0,
    SELECT,
    LEFT,
    DOWN,
    UP,
    RIGHT
};

#define MENUITEMS 4
char menu[] = 
"    "     // prefix
" Item 1 "
" Item 2 "
" Item 3 "
" Item 4 "
"    "    // suffix
;

int curItem = 0;
uint32_t msScroll;
char selItem[9];

LiquidCrystal lcd(A5, A4, D6, D0, D1, D7); // Shield shield v3.0.1

void setup() 
{
  pinMode(A0, INPUT);  // pin for button array (read analog value)
  lcd.begin(16,2);
  selectItem(curItem);

  Particle.variable("SelectedItem", selItem);
}

BUTTONS  btn;
void loop() 
{
  int val = analogRead(A0);

  lcd.setCursor(0, 1);
  if (val > 3200)
  {
      if (btn != BUTTONS::NONE)
      { // after button release act on last pressed button
        switch(btn)
        {
          case BUTTONS::UP: // select first
            selectItem(0);
            break;
          case BUTTONS::LEFT: // select one left
            selectItem(curItem - 1);
            break;
          case BUTTONS::RIGHT: // select one right
            selectItem(curItem + 1);
            break;
          case BUTTONS::DOWN: // select last
            selectItem(MENUITEMS);
            break;
          case BUTTONS::SELECT: // activate selected
            strncpy(selItem, &menu[curItem * 8 + 4], sizeof(selItem));
            lcd.setCursor(4, 0);
            lcd.print(selItem);
            break;
          default:
            lcd.print((int)btn);
            break;      
        }
      }
      btn = BUTTONS::NONE;
  }
  else if (val > 3000)
    btn = BUTTONS::SELECT;
  else if (val > 2500)
    btn = BUTTONS::LEFT;
  else if (val > 1700)   
    btn = BUTTONS::DOWN;
  else if (val >  700) 
    btn = BUTTONS::UP;
  else
    btn = BUTTONS::RIGHT;
}

void selectItem(int itemNr) 
{
  char txt[17];
  itemNr = constrain(itemNr, 0, MENUITEMS - 1);
  int  targetPos = itemNr * 8;
  int  curPos    = curItem * 8;
  int  direction = (curPos < targetPos ? +1 : -1);
  
  while(curPos != targetPos)
  {
    curPos += direction;
    strncpy(txt, &menu[curPos], sizeof(txt));
    lcd.setCursor(0, 1);
    lcd.print(txt);
    delay(100);
    Particle.process();
  }
  curItem = itemNr;
  strncpy(txt, &menu[curItem * 8], sizeof(txt));
  lcd.setCursor(0, 1);
  lcd.print(txt);
}
