
#include <TFT_HX8357.h>   // Hardware-specific library
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <TimeLib.h>
#include <JC_Button.h>



#define BUTTON_PIN_A 8     //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_B 9     //from Arduino pin 8,9,10 to ground.
#define BUTTON_PIN_C 10
#define RELAY_PIN 11
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
//switch is closed, this is negative logic, i.e. a high state
//means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 40     //A debounce time of 20 milliseconds usually works well for tactile button switches.

#define TFTBLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_GRAY 0x7BEF
#define TFT_LIGHT_GRAY 0xC618
#define TFT_GREEN 0x07E0
#define TFT_LIME 0x87E0
#define TFT_BLUE 0x001F
#define TFT_RED 0xF800
#define TFT_AQUA 0x5D1C
#define TFT_YELLOW 0xFFE0
#define TFT_MAGENTA 0xF81F
#define TFT_CYAN 0x07FF
#define TFT_DARK_CYAN 0x03EF
#define TFT_ORANGE 0xFCA0
#define TFT_PINK 0xF97F
#define TFT_BROWN 0x8200
#define TFT_VIOLET 0x9199
#define TFT_SILVER 0xA510
#define TFT_GOLD 0xA508
#define TFT_NAVY 0x000F
#define TFT_MAROON 0x7800
#define TFT_PURPLE 0x780F
#define TFT_OLIVE 0x7BE0

#define FF18 FreeSans12pt7b

TFT_HX8357 tft = TFT_HX8357();

Button btnA(BUTTON_PIN_A, PULLUP, INVERT, DEBOUNCE_MS);
Button btnB(BUTTON_PIN_B, PULLUP, INVERT, DEBOUNCE_MS);
Button btnC(BUTTON_PIN_C, PULLUP, INVERT, DEBOUNCE_MS);
//Button btnA(BUTTON_PIN_A);
//Button btnB(BUTTON_PIN_B);
//Button btnC(BUTTON_PIN_C);

uint8_t item_selected = 0;  // each item that can have its settings changed
uint8_t ars_initialized = 0;
enum {RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME, TIME};              //The possible states for the state machine
enum {NORMAL_MODE, SETTINGS_MODE, CLOCK_CHANGE_MODE};
enum {NO_COUNT, TIMER_COUNT, ROLLING_COUNT, WARNING_COUNT};
enum {ITEM_SELECTED, LAST_ITEM_SELECTED};
enum {RESET, HOUR, MINUTE, SECOND};
enum {RULE26, DINGHY, MATCH};
uint8_t STATE = NORMAL_MODE;  // if 0 normal mode, if 1 settings mode

uint8_t SHORT = 500;
uint8_t LONG = 1000;
uint8_t EXTRALONG = 2000;

uint8_t race_type = 0;  // What race type  Rule 26 = 0, Dinghy = 1, Match = 2
uint8_t timer_set = 5; // this is the number of minutes the counter is set to
bool rolling_start = true; // one enables rolling start, 0 disables
bool warning_horn = true; // one enables horn, 0 disables
bool horn_on = false;
int8_t warning_mm = 0;
int8_t warning_ss = 15;
uint8_t rolling_mm = 0;
uint8_t rolling_ss = 15;
uint8_t timer_mm = 5;
uint8_t timer_ss = 0;
uint8_t counter_enable = NO_COUNT;
uint8_t old_count_mm = 0;  // used to tell wheter minutes changed
uint8_t temp_warning_mm = 0;
uint8_t temp_warning_ss = 0;
uint8_t temp_timer_mm = 0;
uint8_t temp_timer_ss = 0;
uint8_t temp_rolling_mm = 0;
uint8_t temp_rolling_ss = 0;
uint8_t hornburst = 0; // number of hornbursts
uint8_t hornburstduration = 500;  // how long the hornburst duration

uint8_t timeChange = 0;
uint8_t changeSelection = 0;
unsigned long interval = 500; // the time we need to wait
//*************** timing variables *******************//
unsigned long previousMillis = 0; // millis() returns an unsigned long.
unsigned long previous_horn_millis = 0;
unsigned long previous_burst_millis = 0;

uint32_t targetTime = 0;  // for next 1 second timeout

uint8_t ss;  // initialize seconds
uint8_t hh;  // initialize hour
uint8_t mm;  // initialize minutes
uint8_t xmm = 99; // initialize minutes stored to mode
uint8_t xhh = 99; // initialize hour stored to mode


uint8_t buzzer = 13;  // pin number for buzzer

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  btnA.begin();  // added for new version of button
  btnB.begin();  // added for new version of button
  btnC.begin();  // added for new version of button
  tft.init();
  tft.setRotation(1);
  setSyncProvider(RTC.get);  // the function to get the time from the RTC
  setupScreen();
}

void loop() {
  btnA.read();
  btnB.read();
  btnC.read();
  if (btnA.wasPressed()) {
    buttonA();
  }
  if (btnB.wasPressed()) {
    buttonB();
  }
  if (btnB.wasReleased()) {      //If the button was released, turn off horn
    digitalWrite(RELAY_PIN, LOW);
  }
  if (btnC.wasPressed()) {       //If the button was released,    {NO_COUNT, TIMER_COUNT, ROLLING_COUNT, WARNING_COUNT}
    buttonC();
  }

  // unsigned long currentMillis = millis();
  if (STATE == NORMAL_MODE) performTaskEachSecond();  // everything syncs off of this only in normal mode.
  if (horn_on) time_horn_to_off(); // Blows horn if horntime is set through blowhorn()
  hornburstListener();
}



void restartUnit () {
  Serial.println("[restartUnit()]: signal sent to restart unit ");
  ars_initialized = 0;  // reset unit back to startup
  xmm = 99;  // reset xmm back to start
  xhh = 99;
  setupScreen();
  bottomBar();  // STATE = 0 here
}

void setDefaultMinutes() {
  Serial.print("[setDefaultMinutes()]: ");  // WTF?? why won't it let me use a print statement here???
  switch (race_type) {
    case RULE26: {
        Serial.println("5:00");
        timer_set = 5;
        break;
      }
    case DINGHY: {
        Serial.println("3:00");
        timer_set = 3;
        break;
      }
    case MATCH: {
        Serial.println("10:00");
        timer_set = 10;
        break;
      }
  }
  timer_mm = timer_set;  /// actually changes timer to set position
}


void setupScreen() {  // sets up main screen
  Serial.println("[setupScreen()]: Setting up main screen... ");
  addBackground();  // Add a background if unit stated up.
  untitledBox(8, 50, 260, 217, 1); // x, y, width, height, number of dividing lines
  titledBox(285, 70, 190, 202, 1, TFT_MAROON);
  topBar(150, 8, TFT_RED);
  bottomBar();
  timerSetup(18, 65);  // called from topBar()
  rollingStart(65, 180);
  warningHorn(65, 220);
  rollTimeSetup();
  warningTimeSetup();
  moveClockForward(340, 8);
  ars_initialized = 1;  // the unit has started up and initalized
  Serial.println("[setupScreen()]: Automatic Race Starter now initilaized ");
}
