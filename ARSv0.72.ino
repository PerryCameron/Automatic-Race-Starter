
#include <TFT_HX8357.h>   // Hardware-specific library
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <TimeLib.h>
#include <Button.h>


#define BUTTON_PIN_A 8     //Connect a tactile button switch (or something similar)
#define BUTTON_PIN_B 9     //from Arduino pin 8,9,10 to ground.
#define BUTTON_PIN_C 10
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true        //Since the pullup resistor will keep the pin high unless the
//switch is closed, this is negative logic, i.e. a high state
//means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 30     //A debounce time of 20 milliseconds usually works well for tactile button switches.

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

uint8_t item_selected = 0;  // each item that can have its settings changed
uint8_t ars_initialized = 0;
enum {RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME, TIME};              //The possible states for the state machine
enum {NORMAL_MODE, SETTINGS_MODE, CLOCK_CHANGE_MODE};
enum {NO_COUNT, COUNTER_COUNT, ROLLING_COUNT, WARNING_COUNT};
enum {ITEM_SELECTED, LAST_ITEM_SELECTED};
enum {RESET, HOUR, MINUTE, SECOND};
uint8_t STATE = NORMAL_MODE;  // if 0 normal mode, if 1 settings mode

uint8_t race_type = 0;  // What race type  Rule 26 = 0, Dinghy = 1, Match = 2
uint8_t rolling_start = 0; // one enables rolling start, 0 disables
uint8_t warning_horn = 1; // one enables horn, 0 disables
uint8_t warning_mm = 0;
uint8_t warning_ss = 15;
uint8_t rolling_mm = 0;
uint8_t rolling_ss = 15;
uint8_t counter_mm = 5;
uint8_t counter_ss = 0;
uint8_t counter_enable = NO_COUNT;
uint8_t old_count_mm = 0;  // used to tell wheter minutes changed
uint8_t temp_warning_mm = 0;
uint8_t temp_warning_ss = 0;
uint8_t temp_counter_mm = 0;
uint8_t temp_counter_ss = 0;
uint8_t temp_rolling_mm = 0;
uint8_t temp_rolling_ss = 0;

uint8_t timeChange = 0;
uint8_t changeSelection = 0;
unsigned long interval = 500; // the time we need to wait
unsigned long previousMillis = 0; // millis() returns an unsigned long.

uint32_t targetTime = 0;  // for next 1 second timeout
uint8_t ss;  // initialize seconds
uint8_t hh;  // initialize hour
uint8_t mm;  // initialize minutes
uint8_t xmm = 99; // initialize minutes stored to mode
uint8_t xhh = 99; // initialize hour stored to mode

uint8_t buzzer = 13;  // pin number for buzzer

void setup() {
  Serial.begin(9600);
  tft.init();
  tft.setRotation(1);
  setSyncProvider(RTC.get);
  setupScreen();
}

void loop() {
  btnA.read();
  btnB.read();
  btnC.read();
  if (btnA.wasPressed()) {       //If the button was released,
    buttonA();
  }
  if (btnB.wasPressed()) {       //If the button was released,
    buttonB();
  }
  if (btnC.wasPressed()) {       //If the button was released,    {NO_COUNT, COUNTER_COUNT, ROLLING_COUNT, WARNING_COUNT}
    buttonC();
  }

  unsigned long currentMillis = millis();
  if (STATE == NORMAL_MODE) digitalClock(340, 8);  // everything syncs off of this only in normal mode.
}

void buttonA (void) {   /// green button
  Serial.print("Button A was pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        if (counter_enable == NO_COUNT) {
          Serial.print("Timer was started");
          Serial.println();
          if (warning_ss || warning_mm > 0) {
            temp_warning_mm = warning_mm;  // store original count for later
            temp_warning_ss = warning_ss;
            counter_enable = WARNING_COUNT;
          } else {
            counter_enable = COUNTER_COUNT;  // start the timer
          }
          temp_counter_mm = counter_mm;  // store original count for later
          temp_counter_ss = counter_ss;
        }  else {
          counter_enable = NO_COUNT;    // stop counter
          counter_mm = temp_counter_mm;
          counter_ss = temp_counter_ss;  // reset all variables
          warning_mm = temp_warning_mm;
          warning_ss = temp_warning_ss;
          counterSetup(18, 65);  // reset the display to match reset of variables
          rollTimeSetup();
          warningTimeSetup();
        }
        break;
      }
    case SETTINGS_MODE: {
        Serial.print(" Setting being changed: ");
        change(item_selected);
        break;
      }
    case CLOCK_CHANGE_MODE: {   /// HOUR, MINUTE, SECOND
       if (timeChange == HOUR) {
        tft.fillRect(48, 105, 110, 82, TFT_BLACK);
        hh++;
           if (hh == 60) hh = 0;
       } else if (timeChange == MINUTE) {
        tft.fillRect(183, 105, 110, 82, TFT_BLACK);
        mm++;
           if (mm == 60) mm = 0;
       } else if (timeChange == SECOND) { 
        ss++;
           if (ss == 60) ss = 0;
         tft.fillRect(318, 105, 110, 82, TFT_BLACK);
       } else {
        Serial.println("Error: timeChange wrong value");
       }
    timeChangeFunc();
    }
  }
}

void buttonB (void) {   /// red button
  Serial.print("Button B pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        Serial.println("Horn is blowing");
        break;
      }
    case SETTINGS_MODE: {
        Serial.println("Settings mode, change selection");
        item_selected++;
        if (item_selected > 6) item_selected = RESET;       // set back to beginning
        selectItem(item_selected);
        Serial.println(item_selected);
        break;
      }
    case CLOCK_CHANGE_MODE: {
        timeChange++;
        if (timeChange > 3) timeChange = 1;
        timeChangeFunc();        
        Serial.print("timeChange = ");
        Serial.println(timeChange);
        break;
      }
      
  }
}

void buttonC(void) {     /// blue button
  Serial.print("Button C pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        Serial.print("Now in settings mode");
        if (counter_enable == NO_COUNT) {  // only let us go to settings if counter has not started.
          STATE = SETTINGS_MODE;  // turns on blinkItem();  located in void loop()
          setupSettingsScreen();
          bottomBar();  // STATE = 1 here
        }
        break;
      }
    case SETTINGS_MODE: {   // if in settings mode this saves changes and moves back to normal mode
        Serial.print("Now in normal mode");
        STATE = NORMAL_MODE;  // turns off blinkItem()  located in void loop()
        restartUnit();
        item_selected = RACE_TYPE;  ///  resets back to default
        break;
        }
    case CLOCK_CHANGE_MODE: {   // if in clock mode this saves changes and moves back to settings mode
        Serial.print("Now in clock change mode");
        // setTime(hh, mm, ss, 2, 1, 2018);   // uncomment for final program
        // RTC.set(now()); // uncomment for final program
        STATE = SETTINGS_MODE;  // turns off blinkItem()  located in void loop()
        setupSettingsScreen();
        bottomBar();
        break;
        } 
  Serial.println();
  }
}

void restartUnit () {
  ars_initialized = 0;  // reset unit back to startup
  xmm = 99;  // reset xmm back to start
  xhh = 99;
  setupScreen();
  bottomBar();  // STATE = 0 here
}

/*  #######################################################################################
    ############   Beginning of custom functions
    #######################################################################################
*/

void selectItem (int selection) {
     const char *setting_type[7];
     setting_type[0] = "Race Type";
     setting_type[1] = "Timer";
     setting_type[2] = "Rolling Start";
     setting_type[3] = "Roll Time";
     setting_type[4] = "Warning Horn";
     setting_type[5] = "Warning Time";
     setting_type[6] = "Time";
  
  int posX = 14;
  int posY[7] = {20, 55, 90, 125, 160, 195, 230 };
  int oldSelection = selection - 1;
  if (selection == 0) { oldSelection = 6; }   /// makes sure it writes over the old one and resets
        //////////// Remove preious Highlight ///////////
        tft.fillRect(posX, posY[oldSelection], 447, 28, TFT_BLACK);     
        tft.setTextColor(TFT_MAROON);
        tft.drawString(setting_type[oldSelection], 16, posY[oldSelection]+2, 4);    // setting type
        tft.setTextColor(TFT_YELLOW);
        tft.drawString(setting_type[oldSelection], 18, posY[oldSelection], 4);
        
        tft.setTextColor(TFT_MAROON);
        printSelectionValue(oldSelection, posY[oldSelection]+2);  // background of setting value
        tft.setTextColor(TFT_YELLOW);
        printSelectionValue(oldSelection, posY[oldSelection]);   // front side of setting value
        
        //////////// Highlight ///////////////////////
        tft.fillRect(posX, posY[selection], 447, 28, TFT_YELLOW);     
        tft.setTextColor(TFT_BLUE);
        tft.drawString(setting_type[selection], 16, posY[selection]+2, 4);    // setting type
        printSelectionValue(selection, posY[selection]);     // setting value
}

void printSelectionValue (int selection,int posY) {  /// prints each selection in settings mode
  
     const char *race_mode[3];
     race_mode[0] = "Rule 26";
     race_mode[1] = "Dinghy";
     race_mode[2] = "Match";

     const char *booleanAnswer[2];
     booleanAnswer[0] = "No";
     booleanAnswer[1] = "Yes";      

   switch (selection) {  //RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME, TIME
      case RACE_TYPE: {
         tft.drawString(race_mode[race_type], 300, posY+2, 4);
      break;
      }
      case TIMER_SET: {
         char miN[3];
         itoa(counter_mm, miN, 10);
         char seC[3];
         itoa(counter_ss, seC, 10);
         char all[8] = "";
         if (counter_mm < 10) strcat(all, "0");
         strcat(all, miN);
         strcat(all, ":");
         if (counter_ss < 10) strcat(all, "0");
         strcat(all, seC);
         tft.drawString(all, 300, posY+2, 4);
      break;
      }
      case ROLLING: {
         tft.drawString(booleanAnswer[rolling_start], 300, posY+2, 4);
      break;
      }
      case ROLLING_TIME: {
         char miN[3];
         itoa(rolling_mm, miN, 10);
         char seC[3];
         itoa(rolling_ss, seC, 10);
         char all[8] = "";
         if (rolling_mm < 10) strcat(all, "0");
         strcat(all, miN);
         strcat(all, ":");
         if (rolling_ss < 10) strcat(all, "0");
         strcat(all, seC);
         tft.drawString(all, 300, posY+2, 4);
      break;
      }
      case WARNING_HORN: {
         tft.drawString(booleanAnswer[warning_horn], 300, posY+2, 4);
      break;
      }
      case WARNING_TIME: {
         char miN[3];
         itoa(warning_mm, miN, 10);
         char seC[3];
         itoa(warning_ss, seC, 10);
         char all[8] = "";
         if (warning_mm < 10) strcat(all, "0");
         strcat(all, miN);
         strcat(all, ":");
         if (warning_ss < 10) strcat(all, "0");
         strcat(all, seC);
         tft.drawString(all, 300, posY+2, 4);
      break;
      }
      case TIME: {
         tft.drawString("12:00:00", 300, posY+2, 4);
      break;
      }
        
   }    

}

void change(uint8_t item) {  ///{RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME}
  Serial.print("change(); item = ");
     int posY[7] = {20, 55, 90, 125, 160, 195, 230 };
     tft.fillRect(300, posY[item], 90, 28, TFT_YELLOW);  // makes a clean transition when changing options in settings mode
  switch (item) {
    case RACE_TYPE: {  ////////////////// This Changes race type setting  {RULE_26, DINGHY, MATCH}
        Serial.print("Race Type");
        race_type++;
        if (race_type > 2) race_type = 0; /// roll it over if nuber goes past number of race types
        tft.setTextColor(TFT_BLUE);
        printSelectionValue(RACE_TYPE, 20);
        Serial.println(race_type);
        break;
      }
    case TIMER_SET: {
        Serial.print("Counter");
        counter_mm++;
        if (counter_mm > 10) counter_mm = 1;  // allows you to select between 1-9 minutes for timer.
        printSelectionValue(TIMER_SET, 55);
        Serial.println(counter_mm);
        break;
      }
    case ROLLING: {
        Serial.print("Rolling start");
        rolling_start++;
        if (rolling_start > 1) rolling_start = 0;
        printSelectionValue(ROLLING, 90);
        break;
      }
    case ROLLING_TIME: {
        Serial.print("Rolling Time");
        if (rolling_mm < 1) {
          rolling_ss += 15;
        } else {
          rolling_mm++;
        }
        if (rolling_mm > 10) rolling_mm = 0;
        if (rolling_ss > 45) {
          rolling_ss = 0;
          rolling_mm = 1;
        }
        printSelectionValue(ROLLING_TIME, 125);
        break;
      }
    case WARNING_HORN: {
        Serial.print("Warning Horn");
        warning_horn++;
        if (warning_horn > 1) warning_horn = 0;
        printSelectionValue(WARNING_HORN, 160);
        break;
      }
    case WARNING_TIME: {
        Serial.print("Warning Time");
        if (warning_mm < 1) {
          warning_ss += 30;
        } else {
          warning_mm++;
        }
        if (warning_mm > 10) warning_mm = 0;
        if (warning_ss > 45) {
          warning_ss = 0;
          warning_mm = 1;
        }
        // warningTimeSetup();
        printSelectionValue(WARNING_TIME, 195);
        break;
      }
     case TIME: {
      Serial.println("change time");
      setupTimeChange();
      break;
      }
  }
  Serial.println();
}

void setupScreen() {  // sets up main screen
  Serial.print("setupScreen() ");
  Serial.println(counter_mm);
  addBackground();  // Add a background if unit stated up.
  untitledBox(8, 50, 260, 217, 1); // x, y, width, height, number of dividing lines
  titledBox(285, 70, 190, 202, 1, TFT_MAROON);
  topBar(150, 8, TFT_RED);
  bottomBar();
  counterSetup(18, 65);
  rollingStart(65, 180);
  warningHorn(65, 220);
  rollTimeSetup();
  warningTimeSetup();
  digitalClock(340, 8);
  ars_initialized = 1;  // the unit has started up and initalized
}

void timeChangeFunc() {
   switch (timeChange) {
    case HOUR: {
         Serial.println("Hour selected");
         writeSetupDigit (ss, 320, TFT_YELLOW, TFT_MAROON);
         //tft.fillRect(48, 105, 110, 82, TFT_YELLOW);
         writeSetupDigit (hh, 50, TFT_BLUE, TFT_GRAY);
        break;
      } 
    case MINUTE: {
        Serial.println("Minute selected");
         writeSetupDigit (hh, 50, TFT_YELLOW, TFT_MAROON);
         writeSetupDigit (mm, 185, TFT_BLUE, TFT_GRAY);
        break;
      } 
    case SECOND: {
         Serial.println("Second selected");
         writeSetupDigit (mm, 185, TFT_YELLOW, TFT_MAROON);
         writeSetupDigit (ss, 320, TFT_BLUE, TFT_GRAY);
        break;
      } 
   }        
}

void setupTimeChange() {   // sets up time change screen
  STATE = CLOCK_CHANGE_MODE;
  addBackground();
  bottomBar ();
  untitledBox(30, 95, 410, 100, 0);
  timeChange = 1;  // allows to move to minutes upon first hit of select button.
 //        tft.fillRect(48, 105, 110, 82, TFT_YELLOW);  // expieramental yellow highlighting
         writeSetupDigit (hh, 50, TFT_BLUE, TFT_GRAY);
         writeSetupDigit (mm, 185, TFT_YELLOW, TFT_MAROON);
         writeSetupDigit (ss, 320, TFT_YELLOW, TFT_MAROON);
         
tft.setTextColor(TFT_MAROON);
tft.drawChar(':', 159, 105, 8);
tft.drawChar(':', 289, 105, 8);
tft.setTextColor(TFT_YELLOW);
tft.drawChar(':', 156, 102, 8);
tft.drawChar(':', 286, 102, 8);
}

void writeSetupDigit(int digit, int posX, uint32_t text_color, uint32_t back_text_color) {  // turns number to string and then prints to screen.
         char hoR[3];
         char tmp[3];
         if (digit < 10) strcat(hoR, "0");
         itoa(digit, tmp, 10);
         strcat(hoR, tmp);
         tft.setTextColor(back_text_color);
         tft.drawString(hoR, posX, 110, 8);
         tft.setTextColor(text_color);
         tft.drawString(hoR, posX-3, 107, 8);
}


void setupSettingsScreen() {
     const char *race_mode[3];
     race_mode[0] = "Rule 26";
     race_mode[1] = "Dinghy";
     race_mode[2] = "Match";

  addBackground();
  untitledBox(4, 4, 467, 265, 0);

  tft.setTextColor(TFT_MAROON);
  tft.drawString("Race Type", 16, 22, 4);
  tft.drawString("Timer", 16, 57, 4);
  tft.drawString("Rolling Start", 16, 92, 4);
  tft.drawString("Roll Time", 16, 127, 4);
  tft.drawString("Warning Horn", 16, 162, 4);
  tft.drawString("Warning Time", 16, 197, 4);
  tft.drawString("Time", 16, 232, 4);

int posY[7] = {20, 55, 90, 125, 160, 195, 230 };
for(int a = 0; a < 7; a++ ){
      printSelectionValue(a, posY[a]+2);
   }    
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Race Type", 18, 20, 4);
  tft.drawString("Timer", 18, 55, 4);
  tft.drawString("Rolling Start", 18, 90, 4);
  tft.drawString("Roll Time", 18, 125, 4);
  tft.drawString("Warning Horn", 18, 160, 4);
  tft.drawString("Warning Time", 18, 195, 4);
  tft.drawString("Time", 18, 230, 4);
   
for(int a = 0; a < 7; a++ ){
      printSelectionValue(a, posY[a]);
   }
selectItem(item_selected);  /// highlights row that is selected
}
/*
void radioButton(int poX, int poY, int radio_state) {
  switch (radio_state) {
    case 0: {
        tft.fillCircle(poX, poY, 12, TFT_RED);
        break;
      }
    case 1: {
        tft.fillCircle(poX, poY, 12, TFT_GREEN);
        break;
      }
  }
  tft.drawCircle(poX, poY, 12, TFT_LIGHT_GRAY);
}
*/
void addBackground () {
  Serial.print("addBackground() called, mode = ");
  Serial.println();
  tft.fillScreen(TFT_BLUE);
  for (int i = 0; i < 80; i++)  /// Makes cool background
  {
    int rx = random(60);
    int ry = random(60);
    int x = rx + random(480 - rx - rx);
    int y = ry + random(320 - ry - ry);
    tft.drawEllipse(x, y, rx, ry, random(0xFFFF));
  }
}

void topBar (int poX, int poY, uint32_t back_text_color) { // poX = 150  poY = 8 topBar(150, 8);
  Serial.println("Called top_Bar()");
  tft.setTextColor(TFT_GRAY, back_text_color);

  if (ars_initialized == 0) { // If true this is the first time starting up
    tft.fillCircle(290, 38, 20, TFT_RED);
       for (uint8_t i = 19; i < 22; i++) {
         tft.drawCircle(290, 38, i, TFT_BLACK);
       }
    tft.drawPixel(274, 49, TFT_BLACK);  // drawPixel(uint16_t x, uint16_t y, uint16_t color)
    tft.fillRect(0, 0, 480, 38, TFT_RED); // Make red bar at top
    tft.fillRect(290, 38, 190, 19, TFT_RED);
       for (uint8_t i = 37; i < 41; i++) { // draws 3 lines one above the other
         tft.drawFastHLine(0, i, 270, TFT_BLACK);
         tft.drawFastHLine(285, i + 19, 195, TFT_BLACK);
       }
  }  
    switch (race_type) {
      case 0: {
          if (STATE == NORMAL_MODE) {
            tft.drawCentreString("Rule 26 start", poX, poY, 4);
            tft.setTextColor(TFT_YELLOW);
            tft.drawCentreString("Rule 26 start", poX - 2, poY - 2, 4);
          }
          break;
        }
      case 1: {
          tft.drawCentreString("Dinghy Start", poX, poY, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString("Dinghy Start", poX-2, poY, 4);
          break;
        }
      case 2: {
          tft.drawCentreString("Match Racing", poX, poY, 4);
          tft.setTextColor(TFT_YELLOW);
          tft.drawCentreString("Match Racing", poX-2, poY, 4);
          break;
        }
    } 
    if (temp_counter_mm != counter_mm)  counterSetup(18, 65);   // only if the race type changes does the counter write to the screen
    temp_counter_mm = counter_mm;
}

void bottomBar () {
  Serial.print("bottomBar() Called");
  Serial.println();

  if (ars_initialized == 0 ) {
    tft.drawFastHLine(0, 280, 480, TFT_YELLOW);
    tft.drawFastHLine(0, 281, 480, TFT_OLIVE);
    tft.drawFastHLine(0, 282, 480, TFT_OLIVE);
  }

  tft.fillRect(0, 283, 480, 37, TFT_BLACK); // Make bar at bottom
  tft.setTextSize(1);
  tft.setTextColor(TFT_MAROON, TFT_BLACK);
  switch (STATE) {
    case NORMAL_MODE: {   // Normal Mode
        tft.drawCentreString("Settings", 70, 317, 4);
        tft.drawCentreString("Horn", 240, 317, 4);
        tft.drawCentreString("Start/Reset", 400, 317, 4);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("Settings", 68, 315, 4);
        tft.drawCentreString("Horn", 238, 315, 4);
        tft.drawCentreString("Start/Reset", 398, 315, 4);
        break;
      }
    case SETTINGS_MODE: {   // Settings Mode
        tft.drawCentreString("Done", 70, 317, 4);
        tft.drawCentreString("Select", 240, 317, 4);
        tft.drawCentreString("Change", 400, 317, 4);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("Done", 68, 315, 4);
        tft.drawCentreString("Select", 238, 315, 4);
        tft.drawCentreString("Change", 398, 315, 4);
        break;
      }
    case CLOCK_CHANGE_MODE: {   // Settings Mode
        tft.drawCentreString("Done", 70, 317, 4);
        tft.drawCentreString("Select", 240, 317, 4);
        tft.drawCentreString("Change", 400, 317, 4);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("Done", 68, 315, 4);
        tft.drawCentreString("Select", 238, 315, 4);
        tft.drawCentreString("Change", 398, 315, 4);
        break;
      }      
  }
}

void untitledBox (int poX, int poY, int widtH, int heighT, int diV) {  // box to left  poX = 8  poY = 50 widtH = 260 heighT = 217  blackBox1(8,50,260,217); diV = dividing line
  Serial.print("untitledBox()");
  int middle = round(heighT * 0.718);
  Serial.println(middle);
  int topH = round(heighT * 0.562);
  tft.fillRoundRect(poX + 3, poY + 2, widtH + 3, heighT + 3, 12, TFT_OLIVE);
  tft.fillRoundRect(poX, poY, widtH, heighT, 12, TFT_BLACK);
  tft.drawRoundRect(poX, poY, widtH, heighT, 12, TFT_YELLOW);
  tft.drawRoundRect(poX + 1, poY + 1, widtH - 2, 20, 12, TFT_OLIVE);
  tft.fillRect(poX + 1, poY + 10, widtH - 2, 20, TFT_BLACK);
  if (diV == 1) {
    tft.drawFastVLine(poX + 1, poY + 10, topH, TFT_OLIVE); // heighT-122  9  60
    tft.drawFastVLine(poX + 1, poY + 107, 100, TFT_OLIVE); // heighT-117
    tft.drawFastHLine(poX + 1, middle - 1, widtH - 2, TFT_YELLOW);
    tft.drawFastHLine(poX + 1, middle, widtH - 2, TFT_OLIVE); // 156
  } else {
    tft.drawFastVLine(poX + 1, poY + 10, heighT - 20, TFT_OLIVE); // if no dividers just puts in one vertical trim line 86% of total height
  }
}

void titledBox (int poX, int poY, int widtH, int heighT, int diV, uint16_t tColor) {  // box to right  drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
  Serial.print("blackBox() called, void");
  Serial.println();
  tft.fillRoundRect(poX + 1, poY, widtH, heighT, 12, TFT_OLIVE);
  tft.fillRoundRect(poX - 2, poY - 2, widtH - 3, heighT - 3, 12, TFT_BLACK);
  tft.drawRoundRect(poX - 2, poY - 2, widtH - 3, heighT - 3, 12, TFT_YELLOW);
  tft.fillRoundRect(poX, poY, widtH - 6, 38, 10, tColor); // top header
  tft.drawRoundRect(poX, poY, widtH - 6, 38, 10, TFT_OLIVE); // top header
  tft.fillRect(poX + 1, poY + 30, widtH - 7, 10, tColor); // top header
  tft.drawFastHLine(poX, poY + 40, widtH - 6, TFT_YELLOW);
  tft.drawFastHLine(poX, poY + 41, widtH - 6, TFT_OLIVE);
  tft.drawFastVLine(poX, poY + 31, heighT - 19, TFT_OLIVE);
  if (diV == 1) {
    tft.drawFastHLine(poX, poY + 95, widtH - 6, TFT_YELLOW); // div header
    tft.drawFastHLine(poX, poY + 96, widtH - 6, TFT_OLIVE); // div header
    tft.fillRect(poX + 1, poY + 97, widtH - 7, 38, TFT_DARKGREEN); // bottom header
    tft.drawFastHLine(poX, poY + 135, widtH - 6, TFT_YELLOW); // div header
    tft.drawFastHLine(poX, poY + 136, widtH - 7, TFT_OLIVE); // div header
  }
}

void rollTimeSetup (void) {
  Serial.print("rollTimeSetup();");
  int16_t poX = 320;
  int16_t poY = 117;  // rolling_mm and rolling_ss
  if (ars_initialized == 0 ) {
    tft.setTextColor(TFT_RED, TFT_MAROON);
    tft.drawString("Roll Time", 326, 80, 4);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Roll Time", 324, 78, 4);
  }
    printDigit(poX, poY, rolling_mm, TFT_BLACK, TFT_GRAY, TFT_YELLOW, SETTINGS_MODE, 6);
    poX += 54;
    printColon(poX , poY, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 6);
    poX += 16;
    printDigit(poX, poY, rolling_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, SETTINGS_MODE, 6);
  Serial.println();
}

void warningTimeSetup (void) {
  int16_t poX = 320;
  int16_t poY = 214;
  if (ars_initialized == 0 ) {
    tft.setTextColor(TFT_RED, TFT_DARKGREEN);
    tft.drawString("Warning", 332, 175, 4);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Warning", 330, 173, 4);
  }
    printDigit(poX, poY, warning_mm, TFT_BLACK, TFT_GRAY, TFT_YELLOW, SETTINGS_MODE, 6);
    poX += 54;
    printColon(poX, poY, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 6);
    poX += 16;
    printDigit(poX, poY, warning_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, SETTINGS_MODE, 6);
}

void rollingStart (uint8_t poX, uint8_t poY) {
  Serial.print("rollingStart() : ");
    tft.setTextColor(TFT_MAROON, TFT_BLACK);
    switch (rolling_start) {
      case 0: {
          Serial.print("rollingStart is disabled");
          tft.fillCircle(poX - 25, poY + 10, 12, TFT_RED);
          break;
        }
      case 1: {
          Serial.print("rollingStart is enabled");
          tft.fillCircle(poX - 25, poY + 10, 12, TFT_GREEN);
          break;
        }
    }
    tft.drawString("Rolling Start", poX, poY, 4);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Rolling Start", poX - 2, poY - 2, 4);
    tft.drawCircle(poX - 25, poY + 10, 12, TFT_LIGHT_GRAY);
  Serial.println();
}

void warningHorn (uint8_t poX, uint8_t poY) {
  Serial.print("warningHorn() : ");
    tft.setTextColor(TFT_MAROON, TFT_BLACK);
    switch (warning_horn) {
      case 0: {
          tft.fillCircle(poX - 25, poY + 10, 12, TFT_RED);
          break;
        }
      case 1: {
          tft.fillCircle(poX - 25, poY + 10, 12, TFT_GREEN);
          break;
        }
    }
    tft.drawString("Warning Horn", poX, poY, 4);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("Warning Horn", poX - 2, poY - 2, 4);
    tft.drawCircle(poX - 25, poY + 10, 12, TFT_LIGHT_GRAY);
  Serial.println();
}

void digitalClock(uint16_t xpos, uint8_t ypos) {
  int16_t poX = 284;
  int16_t poY = 8;
  tft.setTextSize(1);
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    hh = hour(), mm = minute(), ss = second();  // uses the internal RCT to keep track of time
    if (hh != xhh  && STATE == NORMAL_MODE) {  // check to see if hours have changed
      printDigit(poX, poY, hh, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);
      xhh = hh;
    }
    if (ars_initialized == 0) printColon(poX + 54, poY, TFT_RED, TFT_GRAY, TFT_YELLOW, 6);
    if (mm != xmm) {  // check to see if minutes have changed
      printDigit(353, poY, mm, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);
      xmm = mm;
    }
    if (ars_initialized == 0) printColon(407, poY, TFT_RED, TFT_GRAY, TFT_YELLOW, 6);  // if starting up from first time or coming back from settings mode...
    printDigit(422, poY, ss, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);  // only print ss to clock in normal mode
    if (counter_enable == COUNTER_COUNT) {  // set counter off of digital clock
      countdownFunction(TIMER_SET, counter_mm, counter_ss, 18, 65);
      Serial.println("Counter time");
    }
    if (counter_enable == WARNING_COUNT) {
      countdownFunction(WARNING_TIME, warning_mm, warning_ss, 320, 214);
      Serial.println("Warning time");
    }
    if (counter_enable == ROLLING_COUNT) {
      countdownFunction(ROLLING_TIME, rolling_mm, rolling_ss, 320, 214);
      Serial.println("Rolling time");
    }

  }
}

void printColon(uint16_t poX, uint16_t poY, uint32_t back_color, uint32_t back_text_color, uint32_t text_color, uint8_t font) {
  tft.setTextColor(back_text_color, back_color);
  tft.drawChar(':', poX, poY, font);
  tft.setTextColor(text_color);
  tft.drawChar(':', poX - 2, poY - 2, font);
}



void counterSetup(uint16_t poX, uint16_t poY) {   // Writes the initial countdown to mode
  Serial.print("counterSetup() : ");
  Serial.println(counter_mm);
    printDigit(18, 65, counter_mm, TFT_BLACK, TFT_GRAY, TFT_AQUA, SETTINGS_MODE, 8);
    poX += 108;
    printColon(poX + 2, poY - 6, TFT_BLACK, TFT_GRAY, TFT_AQUA, 8);
    poX += 28;
    printDigit(poX, 65, counter_ss, TFT_BLACK, TFT_GRAY, TFT_AQUA, SETTINGS_MODE, 8);
  Serial.println();
}



void countdownFunction(uint8_t counterSelected, uint8_t count_mm, uint8_t count_ss, uint8_t poX, uint8_t poY) {
  Serial.println("countdownFunction(); ");

  if (count_ss == 0) {  // this creates delay to sync counter to clock // this will never be true after inital start
    count_ss = 60;
  } else if (count_ss == 60) { // since this is 'else if' it waits until next loop to begin, this is how it syncs to clock
    count_mm--;
    count_ss--;
  } else {
    count_ss--;
  }
  if (count_ss == 0) count_ss = 60;  // when counting time 0 really equals 60 before minute rolls.{RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME};
  switch (counterSelected) {
    case TIMER_SET: {
        counter_mm = count_mm;  /// save to global varialbles for next loop.  we uses these so function is universal to all counters
        counter_ss = count_ss;
        switch (count_ss) {
          case 60: {
              if (counter_mm != temp_counter_mm) {  // prevents from rewriting initial setup back to screen
                printDigit(154, 65, 0, TFT_BLACK, TFT_GRAY, TFT_AQUA, 2, 8); // prints 00 instead of 60
              }
              if (warning_ss == 1) {   // since rolling the warning counter into regular counter we start process with 1 second left on warning counter
                printDigit(390, 214, 0, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6); // this prints the 0 in sequence after the  warning counter has stopped.
                warning_ss = 0;  // gets warning_ss unstuck off of 1  and allows chirper to work correctly
              }
              if (rolling_ss == 1) {   // since rolling the rolling counter into regular counter we start process with 1 second left on rolling counter
                printDigit(390, 117, 0, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6); // this prints the 0 in sequence after the  rolling counter has stopped.
              }
              break;
            }
          case 59: {
              if (rolling_ss == 1) {   /// this resets rolling counter back to what it is set at
                rolling_mm = temp_rolling_mm;
                rolling_ss = temp_rolling_ss;
                rollTimeSetup();
              }
              printDigit(18, 65, count_mm, TFT_BLACK, TFT_GRAY, TFT_AQUA, 2, 8);
              printDigit(154, 65, count_ss, TFT_BLACK, TFT_GRAY, TFT_AQUA, 2, 8);
              break;
            }
          default: {
              printDigit(154, 65, count_ss, TFT_BLACK, TFT_GRAY, TFT_AQUA, 2, 8);
              break;
            }
        }
        if (count_ss == 60 && count_mm == 0) {
          if (rolling_mm || rolling_ss > 0) { // do we have a rolling time?
            temp_rolling_mm = rolling_mm;  // save the rolling time for later
            temp_rolling_ss = rolling_ss;
            counter_enable = ROLLING_COUNT;
          } else {
            counter_enable = NO_COUNT;   // turn off counter since we are not roll starting
          }
          //               counter_mm = temp_counter_mm;
          //               counter_ss = temp_counter_ss;  // reset counter back to start
          counterSetup(18, 65);
        }
      }
      break;
    case ROLLING_TIME: {
        rolling_mm = count_mm;  /// save to global varialbles for next loop.  we uses these so function is universal to all counters
        rolling_ss = count_ss;
        counter_mm = temp_counter_mm;
        counter_ss = temp_counter_ss;  // reset counter back to start

        switch (count_ss) {
          case 60: {
              printDigit(390, 117, 0, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6); // prints 00 instead of 60
              break;
            }
          case 59: {
              printDigit(320, 117, count_mm, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              printDigit(390, 117, count_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              break;
            }
          default: {
              printDigit(390, 117, count_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              break;
            }
        }
        if (count_ss == 01 && count_mm == 0) {
          counter_enable = COUNTER_COUNT;
        }

        break;
      }
    case WARNING_TIME: {

        warning_mm = count_mm;  /// save to global varialbles for next loop.  we uses these so function is universal to all counters
        warning_ss = count_ss;
        switch (count_ss) {
          case 60: {
              printDigit(390, 214, 0, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6); // prints 00 instead of 60
              break;
            }
          case 59: {
              printDigit(320, 214, count_mm, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              printDigit(390, 214, count_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              break;
            }
          default: {
              printDigit(390, 214, count_ss, TFT_BLACK, TFT_GRAY, TFT_YELLOW, 2, 6);
              break;
            }
        }
        if (count_ss == 01 && count_mm == 0) { // in order to roll into regular counter we stop warning counter and start regular with one second left
          counter_enable = COUNTER_COUNT;   // turn off warning time and turn on counter time
        }
        break;
      }
  }
  Serial.print("Counter: ");
  Serial.print(counter_mm);
  Serial.print(":");
  Serial.print(counter_ss);
  Serial.println();
  buzzerControl();
}

void printDigit(uint16_t poX, uint16_t poY, uint16_t digit, uint32_t back_color, uint32_t back_text_color, uint32_t text_color, uint8_t system_mode, uint8_t font) {
  // Serial.print("printDigitDown();");
  uint8_t quotient = digit / 10;   /// split number in two so we can write by the digit to tft each second
  uint8_t remainder = digit % 10;
  if (ars_initialized == 0 || system_mode == SETTINGS_MODE) {  /// this is for setting it up for the first time or settings change
    if (font == 6) {
      tft.fillRect(poX - 1, poY - 2 , 20, 40, back_color);
    } else {  // else font is 8
      tft.fillRect(poX - 2, poY - 2 , 10, 60, TFT_BLACK);
    }
    tft.setTextColor(back_text_color, back_color);
    tft.drawNumber(quotient, poX, poY, font);
    tft.setTextColor(text_color);
    tft.drawNumber(quotient, poX - 2 , poY - 2, font);
  } else {
    if ((remainder == 9  && system_mode == 2) || (remainder == 0 && system_mode == NORMAL_MODE)) {   /// only writes first digit if it needs to change
      if (font == 6) {
        tft.fillRect(poX - 1, poY - 2 , 20, 40, back_color);
      } else {  // else font is 8
        tft.fillRect(poX - 2, poY - 2 , 10, 60, TFT_BLACK);
      }
      tft.setTextColor(back_text_color, back_color);
      tft.drawNumber(quotient, poX, poY, font);
      tft.setTextColor(text_color);
      tft.drawNumber(quotient, poX - 2 , poY - 2, font);
    }

  }
  if (font == 6) {
    poX += 27;
    tft.fillRect(poX - 1, poY - 2 , 20, 40, back_color);
  } else {
    poX += 55;  // else font is 8
    tft.fillRect(poX - 2, poY - 2 , 10, 60, TFT_BLACK);
  }

  tft.setTextColor(back_text_color, back_color);
  tft.drawNumber(remainder, poX, poY, font);
  tft.setTextColor(text_color);
  tft.drawNumber(remainder, poX - 2, poY - 2, font);
}

/*############################################################################################
     offset subroutine corrects where minutes roll over so that the buzzer will go off at correct time
     without a lot of ugly hacks to fix all the problems
  ############################################################################################ */
uint8_t offset (void) {     //// subrouting to correct buzzer resempretations
  uint8_t mm_ofset = counter_mm;
  uint8_t testing = counter_mm + counter_ss;
  if (counter_ss == 60)  {   // if the seconds are at 00
    switch (testing) {
      case 65: {  // 5:60 this is really 5:00
          mm_ofset = 5;
        }
      case 64: {  // 4:60 this is really 4:00
          mm_ofset = 4;
          break;
        }
      case 63: {  // 3:60 this is really 3:00
          mm_ofset = 3;
          break;
        }
      case 62: {  // 2:60 this is really 2:00
          mm_ofset = 2;
          break;
        }
      case 61: { // 1:60  this is really 1:00
          mm_ofset = 1;
          break;
        }
    }
  }
  if (counter_ss == 0) mm_ofset = 0;  // lets it buzz at 0
  return mm_ofset;
}

void buzzerControl(void) {
  Serial.println("buzzerControl()");
  uint8_t newmmTimer = offset();  /// fixes mmtimer at rollover for buzzer.
  switch (race_type) {
    case 0: {   // Rule 26 starts
        if (newmmTimer == 5 || newmmTimer == 4 || newmmTimer == 1 ||  newmmTimer == 0) {
          if (counter_enable == COUNTER_COUNT && warning_ss != 1) { // only chirp if the counter is counting
            chirpBuzzer();
          }
        }
        break;
      }
    case 1: {   // Dighny starts
        Serial.print("Dighny");
        Serial.println();
        break;                            // needs code
      }
    case 2: {   // Match Racing
        Serial.print("Match");
        Serial.println();                  // needs code
        break;
      }
  }
}

void chirpBuzzer (void) {
  Serial.println("chirpBuzzer()");
  switch (counter_ss) {
    case 30: {
        tone( buzzer, 2000, 300);
        Serial.print("Chuuuuuuurp!  300ms ");
        break;
      }
    case 20: {
        tone( buzzer, 2000, 300);
        Serial.print("Chuuuuuuurp!  300ms ");
        break;
      }
    case 10: {
        tone( buzzer, 2000, 300);
        Serial.print("Chuuuuuuurp!  300ms ");
        break;
      }
    case 5:  {
        tone( buzzer, 2000, 100);
        Serial.print("Chuuuuuuurp!  100ms ");
        break;
      }
    case 4:  {
        tone( buzzer, 2000, 100);
        Serial.print("Chuuuuuuurp!  100ms ");
        break;
      }
    case 3:  {
        tone( buzzer, 2000, 100);
        Serial.print("Chuuuuuuurp!  100ms ");
        break;
      }
    case 2:  {
        tone( buzzer, 2000, 100);
        Serial.print("Chuuuuuuurp!  100ms ");
        break;
      }
    case 1:  {
        tone( buzzer, 2000, 100);
        Serial.print("Chuuuuuuurp!  100ms ");
        break;
      }
    case 60:  {                          // Buzzer rolls to 60 which is really 0
        tone( buzzer, 2000, 1000);
        Serial.print("Chuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuurp!");
        break;
      }
    case 0: {
        tone( buzzer, 2000, 2000);
        Serial.print("Chuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuurp!");
        break;
      }
  }
}

