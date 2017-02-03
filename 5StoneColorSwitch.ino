#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN   2
#define BUTTON0    3
#define BUTTON1    4
#define BUTTON2    5
#define BUTTON3    6
#define BUTTON4    7
#define INTERLOCK  8  
#define LOCK_PINL 12
#define LOCK_PINH 13
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    25
CRGB leds[NUM_LEDS];
#define BRIGHTNESS  96
#define FRAMES_PER_SECOND  40
int delta = 1;
int y = 0;
int gameWon = false;
int INTERLOCK_STATE = 0;

unsigned long interval = 5000; // the time we need to wait until game resets after win
unsigned long previousMillis = 0; //millis of when win occurred


CRGBPalette16 currentPalette;
TBlendType    currentBlending;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


//----------- LED Strip Allocation Definitions -------------

//--- TEST ALLOCATIONS --
int BUTTON0_LEDS_START = 0; //For testing on small 5 led strip
int BUTTON0_LEDS_LENGTH = 1; //For testing on small 5 led strip
int BUTTON1_LEDS_START = 1; //For testing on small 5 led strip
int BUTTON1_LEDS_LENGTH = 1; //For testing on small 5 led strip
int BUTTON2_LEDS_START = 2; //For testing on small 5 led strip
int BUTTON2_LEDS_LENGTH = 1; //For testing on small 5 led strip
int BUTTON3_LEDS_START = 3; //For testing on small 5 led strip
int BUTTON3_LEDS_LENGTH = 1; //For testing on small 5 led strip
int BUTTON4_LEDS_START = 4; //For testing on small 5 led strip
int BUTTON4_LEDS_LENGTH = 1; //For testing on small 5 led strip
//-----------------------
//---25 LED String Settings Settings ----
//int BUTTON0_LEDS_START = 0; //First LED to illuminate this button
//int BUTTON0_LEDS_LENGTH = 5; //How many LEDS for this button?

//int BUTTON1_LEDS_START = 5; //First LED to illuminate this button
//int BUTTON1_LEDS_LENGTH = 5; //How many LEDS for this button?

//int BUTTON2_LEDS_START = 10; //First LED to illuminate this button
//int BUTTON2_LEDS_LENGTH = 5; //How many LEDS for this button?

//int BUTTON3_LEDS_START = 15; //First LED to illuminate this button
//int BUTTON3_LEDS_LENGTH = 5; //How many LEDS for this button?

//int BUTTON4_LEDS_START = 20; //First LED to illuminate this button
//int BUTTON4_LEDS_LENGTH = 4; //How many LEDS for this button?

//---------------------------------------------------------------------
//----- Button Color Config ---------------------------------
int B0_COLOR = 0; //Current color selected for this button
int B0_CORRECT_COLOR = 4; //Set this to the color you want to match at this location (0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)

int B1_COLOR = 0; //Current color selected for this button
int B1_CORRECT_COLOR = 3; //Set this to the color you want to match at this location (0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)

int B2_COLOR = 0; //Current color selected for this button
int B2_CORRECT_COLOR = 2; //Set this to the color you want to match at this location (0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)

int B3_COLOR = 0; //Current color selected for this button
int B3_CORRECT_COLOR = 1; //Set this to the color you want to match at this location (0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)

int B4_COLOR = 0; //Current color selected for this button
int B4_CORRECT_COLOR = 0; //Set this to the color you want to match at this location (0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)

//------------------------------------------------------------

//------Needed for button debounce----------------
int button0State;             // the current reading from the input pin
int lastButton0State = HIGH;   // the previous reading from the input pin
int button1State;             // the current reading from the input pin
int lastButton1State = HIGH;   // the previous reading from the input pin
int button2State;             // the current reading from the input pin
int lastButton2State = HIGH;   // the previous reading from the input pin
int button3State;             // the current reading from the input pin
int lastButton3State = HIGH;   // the previous reading from the input pin
int button4State;             // the current reading from the input pin
int lastButton4State = HIGH;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
//------------------------------------------------

int LockDelay = 5000; // How long to stay unlocked (in milliseconds) before game reset
int GameStart = 0;
int fadeOutDone = 0;
int fadeInDone = 0;
void setup() {
  Serial.begin(9600);
  Serial.println("5 Stone LED Switch Puzzle");
  Serial.println("");
  Serial.println("---PINOUTS--- ");
  Serial.println("LED DATA -- Pin D2");
  Serial.println("Buttons 0 - 4 on Pins D3-D7");
  Serial.println("INTERLOCK D8 (Pull LOW to disable gameplay)");
  Serial.println("Lock_Pin0 (low when locked) - Pin 12");
  Serial.println("Lock_Pin1 (high when locked) - Pin 13");
  Serial.println("-------------");
  Serial.println("Pausing 3 Seconds for recovery");
  delay(3000); // 3 second delay for recovery
  Serial.println("Starting Game.");
  // tell FastLED about the LED strip configuration i.e FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Lime);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::White);
  delay(1000);
  FastLED.clear(true);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  pinMode (BUTTON0, INPUT_PULLUP); // set BUTTON as input (pulled high)
  pinMode (BUTTON1, INPUT_PULLUP); // set BUTTON as input (pulled high)
  pinMode (BUTTON2, INPUT_PULLUP); // set BUTTON as input (pulled high)
  pinMode (BUTTON3, INPUT_PULLUP); // set BUTTON as input (pulled high)
  pinMode (BUTTON4, INPUT_PULLUP); // set BUTTON as input (pulled high)
  pinMode (INTERLOCK, INPUT_PULLUP); // set INTERLOCK as input (pulled high) used to disable game play when pulled low
  pinMode(LOCK_PINL, OUTPUT);      // sets the digital pin as normally low output (LOCK_PINx states invert on correct PW)
  pinMode(LOCK_PINH, OUTPUT);      // sets the digital pin as normally high output

  currentPalette = RainbowStripeColors_p;
  currentBlending = LINEARBLEND;

}


void READ_BUTTON0()
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON0);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton0State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button0State) {
      button0State = reading;

      if (button0State == LOW) {
        B0_COLOR = B0_COLOR + 1;
        if (gameWon == false) {
          GameStart = 1;
        }
      }
      if (B0_COLOR >= 5) {
        B0_COLOR = 0;
      }
    }
  }


  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton0State = reading;

}

void READ_BUTTON1()
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON1);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton1State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button1State) {
      button1State = reading;

      if (button1State == LOW) {
        B1_COLOR = B1_COLOR + 1;
        if (gameWon == false) {
          GameStart = 1;
        }
      }
      if (B1_COLOR >= 5) {
        B1_COLOR = 0;
      }
    }
  }


  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton1State = reading;

}

void READ_BUTTON2()
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON2);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton2State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button2State) {
      button2State = reading;

      if (button2State == LOW) {
        B2_COLOR = B2_COLOR + 1;
        if (gameWon == false) {
          GameStart = 1;
        }
      }
      if (B2_COLOR >= 5) {
        B2_COLOR = 0;
      }
    }
  }


  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton2State = reading;

}
void READ_BUTTON3()
{

  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON3);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton3State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button3State) {
      button3State = reading;

      if (button3State == LOW) {
        B3_COLOR = B3_COLOR + 1;
        if (gameWon == false) {
          GameStart = 1;
        }
      }
      if (B3_COLOR >= 5) {
        B3_COLOR = 0;
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton3State = reading;

}
void READ_BUTTON4()
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON4);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButton4State) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button4State) {
      button4State = reading;

      if (button4State == LOW) {
        B4_COLOR = B4_COLOR + 1;
        if (gameWon == false) {
          GameStart = 1;
        }
      }
      if (B4_COLOR >= 5) {
        B4_COLOR = 0;
      }
    }
  }


  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton4State = reading;

}

void CollectButtons()
{
  READ_BUTTON0();
  READ_BUTTON1();
  READ_BUTTON2();
  READ_BUTTON3();
  READ_BUTTON4();
}

void LockIt()
{
  digitalWrite(LOCK_PINL, LOW);  // sets the LOCK_PINL low
  digitalWrite(LOCK_PINH, HIGH);  // sets the LOCK_PINH high
}

void Unlock()
{
  digitalWrite(LOCK_PINL, HIGH);  // sets the LOCK_PINL high
  digitalWrite(LOCK_PINH, LOW);  // sets the LOCK_PINH low
}

void UpdateButtonColors()
{
  //Example of how to send a color to a defined section of the string->  fill_solid(leds + BUTTONx_LEDS_START, BUTTONx_LEDS_LENGTH, CRGB::Red);

  if (B0_COLOR == 0) {
    fill_solid(leds + BUTTON0_LEDS_START, BUTTON0_LEDS_LENGTH, CRGB::Red); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B0_COLOR == 1) {
    fill_solid(leds + BUTTON0_LEDS_START, BUTTON0_LEDS_LENGTH, CRGB::Green); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B0_COLOR == 2) {
    fill_solid(leds + BUTTON0_LEDS_START, BUTTON0_LEDS_LENGTH, CRGB::Blue); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B0_COLOR == 3) {
    fill_solid(leds + BUTTON0_LEDS_START, BUTTON0_LEDS_LENGTH, CRGB::Purple); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B0_COLOR == 4) {
    fill_solid(leds + BUTTON0_LEDS_START, BUTTON0_LEDS_LENGTH, CRGB::White); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }

  if (B1_COLOR == 0) {
    fill_solid(leds + BUTTON1_LEDS_START, BUTTON1_LEDS_LENGTH, CRGB::Red); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B1_COLOR == 1) {
    fill_solid(leds + BUTTON1_LEDS_START, BUTTON1_LEDS_LENGTH, CRGB::Green); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B1_COLOR == 2) {
    fill_solid(leds + BUTTON1_LEDS_START, BUTTON1_LEDS_LENGTH, CRGB::Blue); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B1_COLOR == 3) {
    fill_solid(leds + BUTTON1_LEDS_START, BUTTON1_LEDS_LENGTH, CRGB::Purple); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B1_COLOR == 4) {
    fill_solid(leds + BUTTON1_LEDS_START, BUTTON1_LEDS_LENGTH, CRGB::White); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }

  if (B2_COLOR == 0) {
    fill_solid(leds + BUTTON2_LEDS_START, BUTTON2_LEDS_LENGTH, CRGB::Red); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B2_COLOR == 1) {
    fill_solid(leds + BUTTON2_LEDS_START, BUTTON2_LEDS_LENGTH, CRGB::Green); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B2_COLOR == 2) {
    fill_solid(leds + BUTTON2_LEDS_START, BUTTON2_LEDS_LENGTH, CRGB::Blue); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B2_COLOR == 3) {
    fill_solid(leds + BUTTON2_LEDS_START, BUTTON2_LEDS_LENGTH, CRGB::Purple); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B2_COLOR == 4) {
    fill_solid(leds + BUTTON2_LEDS_START, BUTTON2_LEDS_LENGTH, CRGB::White); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }

  if (B3_COLOR == 0) {
    fill_solid(leds + BUTTON3_LEDS_START, BUTTON3_LEDS_LENGTH, CRGB::Red); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B3_COLOR == 1) {
    fill_solid(leds + BUTTON3_LEDS_START, BUTTON3_LEDS_LENGTH, CRGB::Green); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B3_COLOR == 2) {
    fill_solid(leds + BUTTON3_LEDS_START, BUTTON3_LEDS_LENGTH, CRGB::Blue); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B3_COLOR == 3) {
    fill_solid(leds + BUTTON3_LEDS_START, BUTTON3_LEDS_LENGTH, CRGB::Purple); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B3_COLOR == 4) {
    fill_solid(leds + BUTTON3_LEDS_START, BUTTON3_LEDS_LENGTH, CRGB::White); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }

  if (B4_COLOR == 0) {
    fill_solid(leds + BUTTON4_LEDS_START, BUTTON4_LEDS_LENGTH, CRGB::Red); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B4_COLOR == 1) {
    fill_solid(leds + BUTTON4_LEDS_START, BUTTON4_LEDS_LENGTH, CRGB::Green); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B4_COLOR == 2) {
    fill_solid(leds + BUTTON4_LEDS_START, BUTTON4_LEDS_LENGTH, CRGB::Blue); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B4_COLOR == 3) {
    fill_solid(leds + BUTTON4_LEDS_START, BUTTON4_LEDS_LENGTH, CRGB::Purple); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
  if (B4_COLOR == 4) {
    fill_solid(leds + BUTTON4_LEDS_START, BUTTON4_LEDS_LENGTH, CRGB::White); //(0 = Red, 1 = Green, 2 = Blue, 3 = Purple, 4 = White)
  }
}

void CheckLock()
{
  if (B0_COLOR == B0_CORRECT_COLOR && B1_COLOR == B1_CORRECT_COLOR && B2_COLOR == B2_CORRECT_COLOR && B3_COLOR == B3_CORRECT_COLOR && B4_COLOR == B4_CORRECT_COLOR)
  {
    //Serial.println("UNLOCKED!!");
    if (gameWon == false) { //Slight delay after entering the last color before switching to the win.
      delay(1000);
      FastLED.clear(true);
      for (int i = 0; i < 4; i++)
      {
        FastLED.setBrightness(254);
        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
        delay (500);
        FastLED.setBrightness(25);
        FastLED.show();
        delay(500);
      //  Serial.println(i);
      }



    }
    gameWon = true;
  } else {
    gameWon = false;
  }
}
void LEDMode()
{
  if (gameWon == true) {
    FastLED.setBrightness(180);
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    FillLEDsFromPaletteColors( startIndex);
    addGlitter(80);
    GameStart = 0;
  }

  if (GameStart == 0 && gameWon == false) {
    FastLED.setBrightness(y);
    B0_COLOR = 0;
    B1_COLOR = 0;
    B2_COLOR = 0;
    B3_COLOR = 0;
    B4_COLOR = 0;
    brightnessFade();
    fadeOutDone = 0;
    fadeInDone = 0;
    fill_solid(leds, NUM_LEDS, CRGB::Yellow);
  } else {
    if (gameWon == false) {
      if (fadeOutDone == 0) {
        fadeOut();
        fill_solid(leds, NUM_LEDS, CRGB::Yellow);
      }
      FastLED.setBrightness(y);
      if (fadeOutDone == 1) {
        fadeIn();
        UpdateButtonColors();
      }

    }
  }

}



void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 64;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void checkInterlock()
{
INTERLOCK_STATE = digitalRead(INTERLOCK);
if (INTERLOCK_STATE == LOW){
  
}else{
  CollectButtons();
}
  
}
void loop()
{
  
  checkInterlock();
  //CollectButtons();
  LEDMode();
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  CheckLock();

  if (gameWon == true) {
    Unlock();
  } else {
    LockIt();
  }

  /*
    Serial.print("Colors: ");
    Serial.print ("B0-");
    Serial.print(B0_COLOR);
    Serial.print (" B1-");
    Serial.print(B1_COLOR);
    Serial.print (" B2-");
    Serial.print(B2_COLOR);
    Serial.print (" B3-");
    Serial.print(B3_COLOR);
    Serial.print (" B4-");
    Serial.print(B4_COLOR);
    Serial.print (" GameStart-");
    Serial.print (GameStart);
    Serial.println();
  */

}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}


void fadeIn()
{
  if (fadeInDone == 0) {
    y++;
    if (y >= 254)
    {
      fadeInDone = 1;
     // Serial.print("Fade in done");
    }
  }
}

void fadeOut()
{

  y--;
  if (y <= 0)
  {
    fadeOutDone = 1;
   // Serial.println("Fade out done");
  }

}
void brightnessFade()
{
  if (delta > 0)
  {
    y++;
    if (y >= 80)
    {
      delta = -1;
    }
  }
  else
  {
    y--;
    if (y <= 30)
    {
      delta = 1;
    }

  }
}

