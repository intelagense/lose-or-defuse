// DEFINES
// Macros to retrieve the fractional seconds and minute parts of a time
// supplied in ms
#define numberOfCentiseconds(_time_) ((_time_ / 10) % 60)
#define numberOfSeconds(_time_) ((_time_ / 1000) % 60)  
#define numberOfMinutes(_time_) (((_time_ / 1000) / 60) % 99) 



// INCLUDES
// https://github.com/avishorp/TM1637
// this is for the 7 segment
#include <TM1637Display.h>

// toneAC - Version: Latest 
//for Piezo sound effects
#include <toneAC.h>

//LCD display and keypad includes
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header



/*


upside down ribbon cable pinout
black   red     white   green
10      9       8       7       6
5       4       3       2       1 (green wire number 1)
larson  green  switch  ground  red

                                      +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |           GND/RST2  [ ] [ ]         |
         |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   D0
         |            5V/MISO2 [ ] [ ]  SDA[ ] |   D1
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                        13[s]~|   B7     green scanner led
         | [ ]IOREF                      12[b]~|   B6     Piezo speaker (toneAC)
         | [ ]RST                        11[b]~|   B5     Piezo speaker (toneAC)
         | [ ]3V3      +----------+      10[ ]~|   B4     
         | [ ]5v       | ARDUINO  |       9[s]~|   H6     red scanner led
         | [X]GND      |   MEGA   |       8[S]~|   H5     Scanner
         | [X]GND      +----------+            |
         | [X]Vin                         7[a]~|   H4     ambient blue LED
         |                                6[T]~|   H3     Pilot Bulb
         | [ ]A0  54                      5[T]~|   E3     Pilot + input
         | [ ]A1  55                      4[V]~|   G5     Vibration motor
         | [ ]A2  56                 INT5/3[ ]~|   E5     
         | [ ]A3  57                 INT4/2[ ]~|   E4     
         | [F]A4  58                   TX>1[ ]~|   E1     58  Flasher
         | [F]A5  59                   RX<0[ ]~|   E0     59  Flasher
         | [F]A6  60                           |          60  Flasher
         | [ ]A7  61                 TX3/14[ ] |   J1     
         |                           RX3/15[ ] |   J0     
         | [ ]A8  62                 TX2/16[ ] |   H1      
         | [K]A9  63                 RX2/17[ ] |   H0     
         | [K]A10 64            TX1/INT3/18[i] |   D3   fake I2C TM1637 7 segment
         | [K]A11 65            RX1/INT2/19[i] |   D2   fake I2C TM1637 7 segment
         | [K]A12 66        I2C-SDA/INT1/20[I] |   D1   I2C LCD
         | [K]A13 67        I2C-SCL/INT0/21[I] |   D0   I2C LCD
         | [K]A14 68                           |
         | [K]A15 69                           |   Ports:
         |                RST SCK MISO         |    22=A0  23=A1   
         |         ICSP   [ ] [ ] [ ]          |    24=A2  25=A3   
         |                [ ] [ ] [ ]          |    26=A4  27=A5   
         |                GND MOSI 5V          |    28=A6  29=A7    
         | G                                   |    30=C7  31=C6   
         | N 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 5 |    32=C5  33=C4   
         | D 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 V |    34=C3  35=C2   
         |         ~ ~                         |    36=C1  37=C0   
         | @ B B B B # # # g r # R W W W W W @ |    38=D7  39=G2   
         | @ # # # # # # # # # # G W # # # # @ |    40=G1  41=G0   
         |           ~                         |    42=L7  43=L6   
         | G 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 5 |    44=L5  45=L4   
         | N 3 1 9 7 5 3 1 9 7 5 3 1 9 7 5 3 V |    46=L3  47=L2   
         | D                                   |    48=L1  49=L0    SPI:
         |                                     |    50=B3  51=B2     50=MISO 51=MOSI
         |     2560                ____________/    52=B1  53=B0     52=SCK  53=SS 
          \_______________________/         
         

*/
//------------------------------------------------------------------ 

// PINS
// [2]  s: scanner greed led
// [3]  S: Scanner 
// [4]  V: Vibrator  // Deprecated 
// [5]  T: Pilot switch +   (INPUT) requires PIN 6 OUTPUT to be active
// [6]  T: Pilot switch LED (OUTPUT)
// [7]  a: Ambient Bright Blue LED (in wire cutting bay)

// [8]  F: Flashing LED above LCD (finale)
// [9]  F: Flashing LED above LCD (finale)
// [10] F: Flashing LED above LCD (finale)
// [11] B: piezo speaker
// [12] B: piezo speaker
// [13] s: scanner greed led

// [18] i: fake I2C TM1637 7 segment
// [19] i: fake I2C TM1637 7 segment
// [20] I: i2c 7 seg Timer and Text LCD
// [21] I: i2c 7 seg Timer and Text LCD 

// [22] W: Cut wire 1
// [24] W: Cut wire 2
// [26] W: Cut wire 3
// [28] W: Cut wire 4
// [30] W: Cut wire 5
// [31] W: Cut wire 6
// [32] R: Keypad LED Red
// [33] G: Keypad LED Green
// [34] r: wire LED Red
// [36] g: wire LED Green
// [46] W: Button 4
// [48] W: Button 3
// [50] W: Button 2
// [52] W: Button 1

// [A9] K: Keypad   //A
// [A10] K: Keypad  //B
// [A11] K: Keypad  //C
// [A12] K: Keypad  //D
// [A13] K: Keypad  //E
// [A14] K: Keypad  //F
// [A15] K: Keypad  //G
  

//------------------------------------------------------------------ 



hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip
const byte LCD_COLS = 16;
const byte LCD_ROWS = 2;

//keypad inputs
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {69, 68, 67, 66,};
byte colPins[COLS] = {65, 64, 63};



Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char key = 0;
 
//clock stuff
//  AA    led layout
// F  B
// F  B
//  GG
// E  C
// E  C
//  DD
const uint8_t OFF[] = {0, 0, 0, 0};
// In this library,      .GFEDCBA   .GFEDCBA   .GFEDCBA   .GFEDCBA
const uint8_t LEFT[] = {B00111000, B01111001, B01110001, B01111000};
const uint8_t DEAD[] = {B01011110, B01111001, B01110111, B01011110};
const uint8_t STOP[] = {B01101101, B01111000, B00111111, B01110011};
const uint8_t HOLY[] = {B01110110, B00111111, B00111000, B01101111};
const uint8_t SH_T[] = {B01101101, B01110110, B00001000, B01111000};
const uint8_t ZERO[] = {B00111111, B00111111, B00111111, B00111111};
//const uint8_t DEAD[] = {B0, B0, B0, B0};
//const uint8_t DEAD[] = {B0, B0, B0, B0};
//const uint8_t DEAD[] = {B0, B0, B0, B0};
//const uint8_t DEAD[] = {B0, B0, B0, B0};
//BEAT BEST BLAH BUST BUSY BUTT CLUE CODA CODE COLD GASP HALF HALT HATE HELP LAZY LAST LATE AUTO FAST LIFE OBEY FAIL OUCH SAFE SIGH SOUL YEAH CIAO IDLE UGLY

// Create a display object, specifying parameters (Clock pin, Data pin) 
TM1637Display display(19, 18);

// countdown time in ms
unsigned long timeLimit = 0;


const int button1 = 52;   //arcade buttons
const int button2 = 50;
const int button3 = 48;
const int button4 = 46;
const int motorPin = 4;   // for vibration
const int signalPin = 33; // pilot light


byte count = 0;


char timer[3];


// game play vars
bool wireDone = false;
byte wireStrike = 0;
bool memoryDone = false;
byte memoryStrike = 0;
bool passcodeDone = false;
byte passcodeStrike = 0; 
byte strike = 0;
byte hit = 0;
const int maxStrikes = 3;  //max strikes set here
long timeStop = 0;
bool startFlag = false;



//DEBUG SETTINGS
const int wait = 600; //for keypress delays and menu reads, uses delay before timer starts
byte cutWire = 0;
char code[5] = {'1', '2', '3', '4'};
int timerMinutes = 1;


unsigned long markedTime;



// cutable wire stuff
typedef struct{
const int wirePin;          //22,24,26,28,30,31
  int wireState;        //HIGH LOW
  bool wireTriggered;   //has a change already occured? detects state change
}wireData;

//pin state target triggered
wireData wires[] = {
{22, HIGH, false},    //wire 1
{24, HIGH, false},    //wire 1
{26, HIGH, false},    //wire 1
{28, HIGH, false},    //wire 1
{30, HIGH, false},    //wire 1
{31, HIGH, false},    //wire 1
};    



unsigned long previousMillis;  //some global variables available anywhere in the program
const long interval = 250;  //the value is a number of milliseconds








//------------------------------------SETUP--------------------------------------------------------------  

void setup() 
{
  
 





  //------------------------------------SERIAL-------------------------------------------------------------  
  Serial.begin(9600); 
  delay(50);
  Serial.println("and so i started");

  //clock
  display.setBrightness(0x0c); //mid brightness
  // Clear the display
  display.setSegments(OFF);
  
  
  pinMode(4, OUTPUT);   //PWM - Vibrator PWM 0-255
  //
  pinMode(6, OUTPUT);   //PWM - LED for rocker switch
  pinMode(7, OUTPUT);   //PWM - Ambient Bright Blue LED (in wire cutting bay)
  
  pinMode(8, OUTPUT);   //PWM - larson scanner
  pinMode(9, OUTPUT);   //PWM - scanner red led
  

  pinMode(13, OUTPUT);  //PWM - scanner green led
  
  
  
  pinMode(22, INPUT_PULLUP);  //wire 1  LOW if connected
  pinMode(24, INPUT_PULLUP);  //wire 2  HIGH if cut
  pinMode(26, INPUT_PULLUP);  //wire 3
  pinMode(28, INPUT_PULLUP);  //wire 4
  pinMode(30, INPUT_PULLUP);  //wire 5
  pinMode(31, INPUT_PULLUP);  //wire 6
  
  pinMode(32, OUTPUT);  //Keypad LED Red
  pinMode(33, OUTPUT);  //Keypad LED Green
  pinMode(36, OUTPUT);  // wire bay LED Red
  pinMode(38, OUTPUT);  // wire bay LED Green
  
  pinMode(58, OUTPUT);   //PWM - Flashing LED above LCD (finale)
  pinMode(59, OUTPUT);   //PWM - Flashing LED above LCD (finale)
  pinMode(60, OUTPUT);  //PWM - Flashing LED above LCD (finale)
  
  pinMode(button4, INPUT_PULLUP);  //Button 4
  pinMode(button3, INPUT_PULLUP);  //Button 3
  pinMode(button2, INPUT_PULLUP);  //Button 2
  pinMode(button1, INPUT_PULLUP);  //Button 1
  


  //LCD setup
  int status;

  // initialize LCD with number of columns and rows: 
  // hd44780 returns a status from begin() that can be used
  // to determine if initalization failed.
  // the actual status codes are defined in <hd44780.h>
  // See the values RV_XXXX
  //
  // looking at the return status from begin() is optional
  // it is being done here to provide feedback should there be an issue
  //
  // note:
  //  begin() will automatically turn on the backlight
  //
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if(status) // non zero status means it was unsuccesful
  {
    status = -status; // convert negative status value to positive number
    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }
  // initalization was successful, the backlight should be on now
  //end LCD setup

//begin instructions-----------------------------------------------------------




  /* test

 while(customKeypad.getKey() != '*') {
   // digitalWrite(3, HIGH);
  
  }
 
// */
initial();

timeLimit = ((unsigned long)timerMinutes * 60000);
markedTime = millis() + 2000;
lcd.clear();
lcd.home();
lcd.print("welcome doctor,");
lcd.setCursor(0,1);
lcd.print("please press '#'");
display.setSegments(LEFT);
delay(1000);
countdown();

}





void loop()
{
  
  //code goes here sometimes
  
}



void initial()  //set up game function
{

  lcd.clear();
  lcd.home();
  lcd.print("initial setup");
  lcd.setCursor(0,1);
  lcd.print("loading...");
  delay(wait); //just letting it warm up a bit
  
  lcd.clear();
  lcd.home();
  //display "0123456789012345"
  lcd.print("read the screen");
  lcd.setCursor(0,1);
  lcd.print("'#' continues  #");
  lcd.setCursor(15,1);
  lcd.blink();

  while(customKeypad.getKey() != '#') { }
  
  lcd.clear();
  lcd.home();
  //display "0123456789012345"
  lcd.print("wire scan mode");
  lcd.setCursor(0,1);
  lcd.print("to check wires #");
  lcd.setCursor(15,1);
  lcd.blink();

  while(customKeypad.getKey() != '#') { }
  
  setWire();
  
  lcd.clear();
  lcd.home();
  lcd.print("wire   is saved");
  lcd.setCursor(5,0);
  lcd.print(cutWire+1);
  lcd.setCursor(0,1);
  lcd.print("please wait...");
  delay(wait);
  




//keypad
  lcd.clear();
  lcd.home(); 
  lcd.print("loading...");
  delay(wait); //to avoid multiple button presses
  lcd.home(); 
  //display "0123456789012345"
  lcd.print("type 4 digit pin");
  lcd.setCursor(7,1);
  lcd.print("then '#'");
  lcd.setCursor(0,1);
  lcd.blink();
  
  setPassCode();
  
  lcd.clear();
  lcd.home();
  //display "0123456789012345"
  lcd.print("pin      saved");
  lcd.setCursor(4,0);
  lcd.print(code);
  lcd.setCursor(0,1);
  lcd.print("please wait...");
  delay(wait);
  

  lcd.clear();
  lcd.home();
  //display "0123456789012345"
  lcd.print("it is now time");
  lcd.setCursor(0,1);
  lcd.print("to set the timer");
  delay(wait);
  lcd.clear();
  lcd.home();
  
  //display "0123456789012345"
  lcd.print("1-99 minutes");
  lcd.setCursor(7,1);
  lcd.print("then '#'");
  lcd.setCursor(0,1);
  lcd.blink();
  
  setTimer();
  


  lcd.noBlink();
  lcd.clear();
  lcd.home();
  lcd.print("pin:");
  lcd.write(code);
  lcd.print("  wire:");
  lcd.print(cutWire+1);
  lcd.setCursor(0,1);
  lcd.print("time:");
  lcd.write(timer);
  lcd.setCursor(9,1);
  lcd.print("start:#");
  lcd.setCursor(15,1);
  lcd.blink();

  while(customKeypad.getKey() != '#') { }
}





void countdown() 
{
  
  // Calculate the time remaining           5 - 5
  
  long timeRemaining = timeLimit + markedTime - millis();

  while(timeRemaining > 0) 
  {
    
    // To display the countdown in mm:ss format, separate the parts
    int centiseconds = numberOfCentiseconds(timeRemaining);
    int seconds = numberOfSeconds(timeRemaining);
    int minutes = numberOfMinutes(timeRemaining); 
    
        if (timeRemaining > 60000)
        { 
          // This displays the seconds in the last two places
          display.showNumberDecEx(seconds, 0, true, 2, 2);
          // Display the minutes in the first two places, with colon
          display.showNumberDecEx(minutes, 0x80>>3, true, 2, 0);
          
        }else {
          
          // This displays the centiseconds in the last two places
          display.showNumberDecEx(centiseconds, 0, true, 2, 2);
          // Display the minutes in the first two places, with colon
          display.showNumberDecEx(seconds, 0x80>>3, true, 2, 0);
        }
        
    if (hit>=2 || strike >= 2) // win or lose game condition
    {//stops timer, cast judgement
      timeStop = timeRemaining; 
      seconds = numberOfSeconds(timeRemaining);
      minutes = numberOfMinutes(timeRemaining); 
      judgement();
    }
    
    if (wireDone==false){wireCheck();}
    
    char key = customKeypad.getKey();
    switch(key)
    {
      case '#':
      {
        lcd.noBlink();
        lcd.clear();
        lcd.home();
        //display "0123456789012345"
        lcd.print("system press '1'");
        lcd.setCursor(0,1);
        lcd.print("defrag press '2'");
      }  
      case '1':{passCode();}
      case '2':{memory();}
    }
    
    timeRemaining = timeLimit + markedTime - millis(); //updates time
    
    
  }
  //display zero
  display.showNumberDecEx(0, 0, true, 2, 2);
  display.showNumberDecEx(0, 0x80>>3, true, 2, 0);
  judgement();
}



void passCode() //passcode check
{
  
}

void memory()     //memory module
{
  
}

void wireCheck() 
{
  for (int i=0; i <= 5 && wireDone == false; i++)
  {

    if (wires[i].wireTriggered == false)
    {
      if (wires[i].wireState != digitalRead(wires[i].wirePin))
      {
        startFlag = true;
        wires[i].wireTriggered = true;
        if (i == cutWire) //cutWire = this is the wire that is supposed to be cut
        {
          hit++;
          wireDone = true;
          //needs function
        } else {
          strike++;
          wireStrike++;
          //needs function
        }
      }  
    }
    
  }
  
}



//------------------------------------------------------------------
//setTimer() starts
void setTimer()
{
  
  count = 0;
  
  while (count < 3)
  {
    
    char key = customKeypad.getKey();
    
    if (key == '*') 
    {
      lcd.setCursor(0,1);
      lcd.print("  ");
      lcd.setCursor(0,1);
      count = 0;
    }
    
    else if (key)
    {
      
      if (key != '#')
      {
        if (count == 2){}
        else if (count < 2)
        { 
          lcd.home();
          //display "0123456789012345"
          lcd.print("1-99  * to reset");
          lcd.setCursor(count,1);
          lcd.print(key);
          timer[count]=key;
          count++;
        }
      }
      else if (key == '#') 
      {
        timerMinutes = atoi(timer);
        if (timerMinutes < 1)
        {
          lcd.setCursor(0,1);
          lcd.print("  ");
          lcd.setCursor(0,1);
          count = 0;
        }
        else if(timerMinutes >= 1)
        {
          count = 3;
        }
      }
    }
  }
}


//------------------------------------------------------------------
//setPasscode() starts
void setPassCode()
{
  count = 0;
  while (count < 5)
  {
    char key = customKeypad.getKey();

    if (key == '*') 
    {
      lcd.setCursor(0,1);
      lcd.print("    ");
      lcd.setCursor(0,1);
      count = 0;
    } else if (key)
    {
      if (key == '#') 
      {
        if (count < 4)
        { }else if (count == 4)
        {count++;}
      } else if (key != '#')
      {
        if (count == 4){}
        else if (count < 4)
        { 
          lcd.home();
          //display "0123456789012345"
          lcd.print("  * to restart  ");
          lcd.setCursor(count,1);
          lcd.print(key);
          code[count]=(key);
          count++;
        }
      }
    }
  }
}


void scanWire() //during setup, checks to see which wires are installed
{
  
  delay(wait);
  
  lcd.clear();
  lcd.home();
  for (int i=0; i <= 5; i++)
  {
  wires[i].wireState = digitalRead(wires[i].wirePin);
    if (wires[i].wireState == LOW)
    {
      lcd.print(i+1);
    } else {
      lcd.print(" ");
    }
  }
  //display "0123456789012345"
  
  lcd.print(" detected");
  lcd.setCursor (0,1);
  lcd.print("'*' to rescan  #");
  lcd.setCursor (15,1);
}

void setWire() // during setup, set which wire you want to cut
{
  bool scanPass = false;
  scanWire();
  
  while(scanPass == false)
  {  
  key = customKeypad.getKey();

    if (key == '#') 
    {
      scanPass=true;
    } else if (key == '*')
    {
      lcd.clear();
      lcd.noBlink();
      delay(wait);
      lcd.blink();
      scanWire();
    }
  }
  
  lcd.clear();
  lcd.home();
  //display "0123456789012345"
  lcd.print("select cut wire?");
  lcd.setCursor(7,1);
  lcd.print("then '#'");
  lcd.setCursor(0,1);
  lcd.noBlink();
  
  /* oop?
  if(key > 0 &&)
  key = customKeypad.getKey();
  switch(key)
{
 case '#':{
 } break;

 case '1':
 case '2':
 case '3':
 case '4':
 case '5':{
 } break;
 
 case '6':{
 } break;
}
  */
  count=0;
  
  
  while (count != 2)
  {
    key = customKeypad.getKey();
    if (key == '1' && (wires[0].wireState==LOW)) 
    {
      cutWire = 0;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(0,1);
      lcd.print(cutWire+1);
      count = 1;
    } else if (key == '2' && (wires[1].wireState==LOW))
    {
      cutWire = 1;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(1,1);
      lcd.print(cutWire+1);
      count = 1;
    } else if (key == '3' && (wires[2].wireState==LOW))
    {
      cutWire = 2;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(2,1);
      lcd.print(cutWire+1);
      count = 1;
    } else if (key == '4' && (wires[3].wireState==LOW))
    {
      cutWire = 3;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(3,1);
      lcd.print(cutWire+1);
      count = 1;
    } else if (key == '5' && (wires[4].wireState==LOW))
    {
      cutWire = 4;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(4,1);
      lcd.print(cutWire+1);
      count = 1;
    } else if (key == '6' && (wires[5].wireState==LOW))
    {
      cutWire = 5;
      lcd.setCursor(0,1);
      lcd.print("      ");
      lcd.setCursor(5,1);
      lcd.print(cutWire+1);
      count = 1;
    }
    else if (key == '#') 
    {
      if (count == 1) 
      {
        count = 2;
      }
    }  
  }
  
}

void alert(){
  //beep
}

void finale(){
  //buzzz
}


void judgement()
{
  lcd.blink();
  lcd.clear();
  lcd.home();
  if (hit == 2)//win
  {
    //victory() tune then
    
    if (timeStop > 30)
    {
      //display "0123456789012345"
      lcd.print("geez it's like i");
      lcd.setCursor(0,1);
      lcd.print("am not trying  #");
      lcd.setCursor(15,1);
    }else if (timeStop >15){
      //display "0123456789012345"
      lcd.print("eh, well you had");
      lcd.setCursor(0,1);
      lcd.print("plenty of time #");
      lcd.setCursor(15,1);
    }else if (timeStop >5){
      //display "0123456789012345"
      lcd.print("AGHHHHH i wanted");
      lcd.setCursor(0,1);
      lcd.print("to blow up!!   #");
      lcd.setCursor(15,1);
    }else{
      //display "0123456789012345"
      lcd.print("well, i changed");
      lcd.setCursor(0,1);
      lcd.print("my mind anyway #");
      lcd.setCursor(15,1);
    }
  }else if (strike == 2) {// active lose
      //display "0123456789012345"
      lcd.print("oh did you need ");
      lcd.setCursor(0,1);
      lcd.print("more time? lol #");
      lcd.setCursor(15,1);
    if (timeStop >15)
    {
      //display "0123456789012345"
      lcd.print("how embarassing,");
      lcd.setCursor(0,1);
      lcd.print("grow a brain! #");
      lcd.setCursor(15,1);
    }else if (timeStop >5){
      //display "0123456789012345"
      lcd.print("i got a lot done");
      lcd.setCursor(0,1);
      lcd.print("today, and you?#");
      lcd.setCursor(15,1);
    }else {
      //display "0123456789012345"
      lcd.print("DON'T THROW ME!!");
      lcd.setCursor(0,1);
      lcd.print("it's your fault#");
      lcd.setCursor(15,1);
    }
  } else { //time out
    if ((strike == 0) && (hit == 0) && (startFlag == false))
    { 
      //alert()
      //display "0123456789012345"
      lcd.print("yawn, oh hey uh.");
      lcd.setCursor(0,1);
      lcd.print("just press '#'");
      lcd.setCursor(15,1);
      
      while(customKeypad.getKey() != '#') { }
      //finale() then
      lcd.clear();
      lcd.home();
      //display "0123456789012345"
      lcd.print("i waited for you");
      lcd.setCursor(0,1);
      lcd.print("cold and alone #");
      lcd.setCursor(15,1);
    
    } else {
      //finale() then
      //display "0123456789012345"
      lcd.print("well i guess you");
      lcd.setCursor(0,1);
      lcd.print("sort of tried..#");
      lcd.setCursor(15,1);
    }
  }
}
  
