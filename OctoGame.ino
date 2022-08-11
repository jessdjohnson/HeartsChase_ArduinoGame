/*
Hearts Chase Game Code
Player controls little octopus character that chases after hearts.  
If you catch a heart, you get a point. If you hit a bomb, you lose a point.
Game end conditions - win if you get to 10 points, lose if you go below 0 points.

Made to run on arduino uno (or hero board) with LCD shield.

Written By: Jess D. Johnson, 08/09/2022
Octopus character design by Calvin Johnson
*/

#include <LiquidCrystal.h>

 //Set up pins for the LCD display
const int RS = 8;
const int EN = 9;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
const int pin_BL = 10; // arduino pin wired to LCD backlight circuit

// Initialize LCD
LiquidCrystal lcd( RS,  EN,  d4,  d5,  d6,  d7);

// define some values used by the panel and buttons
int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

// octopus position counters
int o_x = random(16);
int o_y = random(2);

//heart position counters
int h_x = random(16);
int h_y = random(2);

//bomb1 position counters
int b_x = random(16);
int b_y = random(2);

//score counter
int score = 0;

//time counter
int timecount = 0;

// random value storage
int randval = 0;

// Creating custom character icons for the game
// smily face - used when octopus catches heart
byte smiley[8] = {
  B00000,
  B01010,
  B00000,
  B10001,
  B10001,
  B01110,
  B00000,
};

// sad face - used when octopus hits bomb
byte sadFace[8] = {
  B00000,
  B01010,
  B00000,
  B01110,
  B10001,
  B10001,
  B00000, 
  B00000
};
 
// Heart
byte fullHeart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

byte octopus[8] = {
  B00100,
  B01110,
  B10101,
  B11111,
  B11011,
  B01110,
  B11011,
  B10101
};

byte bomb[8] = {
  B10101,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B10101
};

int i = 0;

void setup() {
  //initialize heart character
  lcd.createChar(1, fullHeart);

  //start-up game, displaying welcome screen
  lcd.begin(16, 2); 
  lcd.setCursor(0,0);
  lcd.print("Welcome to Heart");
  for (int i=0; i<4; i++){
    lcd.setCursor(i,1);
    lcd.write(byte(1));
  }
  lcd.setCursor(5,1);
  lcd.print("Chase!");
  for (int i=15; i>11; i--){
    lcd.setCursor(i,1);
    lcd.write(byte(1));
  }
  delay(1500);

  // start-up game, display game instructions
  lcd.setCursor(0,0);
  lcd.print("Catch the Heart!");
  lcd.setCursor(0,1);
  lcd.print("Avoid the Bomb! ");
  delay(1500);

  //initialize game characters
  lcd.createChar(0, octopus);
  lcd.createChar(1, fullHeart);
  lcd.createChar(2, smiley);
  lcd.createChar(3, sadFace);
  lcd.createChar(4, bomb);
}

// function for reading the LCD puttons -- taken from LCD shield test code written by Mark Bramwell, July 2010
int read_LCD_buttons()
{
adc_key_in = analogRead(0); // read the value from the sensor
// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
// we add approx 50 to those values and check to see if we are close
if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
// For V1.1 us this threshold
if (adc_key_in < 50) return btnRIGHT;
if (adc_key_in < 250) return btnUP;
if (adc_key_in < 450) return btnDOWN;
if (adc_key_in < 650) return btnLEFT;
if (adc_key_in < 850) return btnSELECT;
 
return btnNONE; // when all others fail, return this...
}

//Main game loop
void loop() {
  // reinitialize characters - sometimes they seem to drift and pixels change if not reinitialized??
  lcd.begin(16, 2);
  lcd.createChar(0, octopus);
  lcd.createChar(1, fullHeart);
  lcd.createChar(2, smiley);
  lcd.createChar(3, sadFace);
  lcd.createChar(4, bomb);

  // check position of octopus to see if it has caught the heart
  if ((o_x == h_x)&&(o_y == h_y)){
    //change to smiley when it catches the heart
    lcd.setCursor(o_x,o_y);
    lcd.write(byte(2)); //smiley

    //update score
    score++; //increment score by 1
    //check for game end conditons
    if (score < 10){
      //if not game end, display score in upper right corner of screen
      lcd.setCursor(15,0);
      lcd.print(score);
      delay(200);
    }
    else{
      //if game end triggered, display game win message
      lcd.begin(16,2);

      //cute characters around win message
      lcd.setCursor(0,0);
      lcd.write(byte(0));
      lcd.setCursor(1,0);
      lcd.write(byte(1));
      lcd.setCursor(2,0);
      lcd.write(byte(0));

      // game win message
      lcd.setCursor(4,0);      
      lcd.print("YOU WIN!!");

      //cute characters around win message
      lcd.setCursor(13,0);
      lcd.write(byte(0));
      lcd.setCursor(14,0);
      lcd.write(byte(1));
      lcd.setCursor(15,0);
      lcd.write(byte(0));

      // second row of cute characters
      for (int i=0; i<16; i++){
        lcd.setCursor(i,1);
        lcd.write(byte(1));
      }

      // delay so that message stays on screen for a while
      delay(1500);

      //reset score, so user can continue playing
      score = 0;  
    }
    
    //Update heart position once caught, being sure not to reinitialize it on top of the octopus
    while ((h_x == o_x) && (h_y == o_y)){
      h_x = random(16);
      h_y = random(2);
    }

    //Every other point, reset the bomb position - so that you can't get complacent!
    // Get new position for bomb that is not the same as the octopus or the heart
    if (score%2 == 1){
      b_x = random(16);
      b_y = random(2);
      //check that new position isn't heart or octopus position!
      while (((b_x == o_x) && (b_y == o_y)) || ((b_x == h_x) && (b_y == h_y))){
        b_x = random(16);
        b_y = random(2);
      }
    }
  }
  //if did not catch the heart, checks to see if you caught the bomb
  else if ((o_x == b_x)&&(o_y == b_y)){
    //if you caught the bomb, writes a sad face
    lcd.setCursor(o_x,o_y);
    lcd.write(byte(3)); // sadFace
    delay(500);

    //update score, taking away a point for hitting the bomb
    score--;
    //checks game lose conditions
    if (score >= 0){
      //if score not less than zero, shows lowered score in upper right corner
      lcd.setCursor(15,0);
      lcd.print(score);
      delay(200);
    }
    else{
      //if score less than zero, you lose - display game lose message and reset.
      lcd.begin(16,2);

      //display cute sad characters
      lcd.setCursor(0,0);
      lcd.write(byte(4));
      lcd.setCursor(1,0);
      lcd.write(byte(3));
      lcd.setCursor(2,0);
      lcd.write(byte(4));

      //display lose message
      lcd.setCursor(4,0);
      lcd.print("YOU LOSE!");

      //display cute sad characters
      lcd.setCursor(13,0);
      lcd.write(byte(4));
      lcd.setCursor(14,0);
      lcd.write(byte(3));
      lcd.setCursor(15,0);
      lcd.write(byte(4));

      //display second row full of BOMBS
      for (int i=0; i<16; i++){
        lcd.setCursor(i,1);
        lcd.write(byte(4));
      }

      //hold message on screen for user to read.
      delay(1000);

      //Reset score to zero to allow user to play again
      score = 0;  
    }    
    
    // Get new position for bomb
    b_x = random(16);
    b_y = random(2);
    //check that new position isn't same as a heart or octopus position! (this seems to have a bug sometimes...?)
    while (((b_x == o_x) && (b_y == o_y)) || ((b_x == h_x) && (b_y == h_y))){
      b_x = random(16);
      b_y = random(2);
    }
  }
  //if octopus position does not match heart or bomb, then update positions of both heart and bomb
  else{
    //only update the positions every 4th time step (this decreases difficulty - could change the 4 to 2-3 to increase difficulty - or 7-8 for younger kids...)
    //#todo: possible easy improvement here - start the game at a lower difficulty level and level up, changing this position refresh number and/or the delay between loops
    //and showing what level you reached when you win/lose.
    
    //checks if timecount is divisible by 4 - if so, updates positions.
    if (timecount%4 == 0){
      //Update bomb x (horizontal) position. Possible values [0-15]
      //pick a random step for the bomb's x position change: -1, 0, +1
      randval = random(-1,2);
      //update value
      b_x = b_x + randval;
      //double check if new value carried bomb off the left side of the screen
      if (b_x < 0){
        //if it did, wrap bomb around, having it reaappear on the right hand side
        b_x = 15;
      }
      //check if updated value carried bomb off the right hand side of the screen
      if (b_x > 15){
        //if it did, wrap bomb around, having it reappear on the left hand side
        b_x = 0;
      }
      //if bomb x position was not updated then update the y position
      //note: because I did x and then y, the bomb is more likely to move around in the x direction than the y direction....making the game easier.
      //#todo: this could be updated, so that it alternates or picks y direction first as the "difficulty" level of the game increases?
      //Experientially, y direction changes tend to be more difficult for the game player to control
      if (randval == 0){
        //if x direction did not change, get new y (vertical) bomb position. (random(2) will generate [0-1] values.)
        b_y = b_y + random(2);
        //double check that updated value didn't go off the bottom of the screen, if it did, wrap it around, having bomb reappear on the top line
        if (b_y > 1){
          b_y = 0;
        }
      }

      //update heart position
      //get random x value for heart to move - options: [-1,0,1]
      randval = random(-1,2);
      //update heart position with random step
      h_x = h_x + randval;
      //check if heart went off left side of screen
      if (h_x < 0){
        //if it did, have reappear on the rhs
        h_x = 15;
      }
      //check if heart went of rhs of screen
      if (h_x > 15){
        //if it did, have reappear on the lhs
        h_x = 0;
      }
      //if heart did not move in the horizontal/x direction, than heart can move in y.
      if (randval == 0){
        //update heart's y position, adding [0,1]
        h_y = h_y + random(2);
        //check if heart went off the bottom of the screen, if it did, have it reappear on the top line
        if (h_y > 1){
          h_y = 0;
        }
      }
    }
  }

  //Read user inputs from the keyboard
  lcd_key = read_LCD_buttons(); // read the buttons

  //Depending on button pressed, determine octopus motion to perform
  switch (lcd_key)
  { 
    case btnRIGHT:
    {
      //if pressed right button, move octopus to the right
      o_x = o_x + 1;
      //check if update sends octopus off lcd rhs, if so reappear on lhs
      if (o_x > 15){
        o_x = 0;
      }
      break;
    }
    case btnLEFT:
    {
      //if pressed left button, move octopus to the left
      o_x = o_x - 1;
      //check if update sends octopus off lhs, if so reappear on the rhs
      if (o_x < 0){
        o_x = 15;
      }
      break;
    }
    case btnUP:
    {
      //check if up button was pressed, if so move octopus up
      o_y = o_y + 1;
      // if moving up sent octopus off the top of the screen, reappears in the bottom row
      if (o_y > 1){
        o_y = 0;
      }
      break;
    }
    case btnDOWN:
    {
      //check if down button was pressed and move octopus down
      o_y = o_y - 1;
      //if moving octopus down sent it off the screen, reappear in the top row
      if (o_y < 0 ){
        o_y = 1;
      }
      break;
    }
    case btnSELECT:
    { 
      //Pause functionality
      lcd.begin(16,2);
      //display pause on the screen
      lcd.print("Pause");
      //read key presses - to ensure that pause still pressed.
      lcd_key = read_LCD_buttons();
      //while select button is held down, game stays paused.
      //#todo: could be improved to stay paused until the button is pressed a second time, allowing the user to walk away while game is paused.
      while (lcd_key == btnSELECT){
        lcd_key = read_LCD_buttons();
      }
      break;
    }
    case btnNONE:
    {
      //if no button was pressed, do nothing
      break;
    }
  }

  // Update the game characters displayed
  lcd.setCursor(o_x, o_y);
  lcd.write(byte(0));
  lcd.setCursor(h_x, h_y);
  lcd.write(byte(1));
  lcd.setCursor(b_x, b_y);
  lcd.write(byte(4));

  //delay - changing this delay changes the pacing of the game! could be used to toggle the difficulty level of the game
  delay(100);

  //increment time counter used for updated bomb and heart motions
  timecount++;
}
