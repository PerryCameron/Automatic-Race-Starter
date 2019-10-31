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
    if ((remainder == 9  && system_mode == CLOCK_CHANGE_MODE) || (remainder == 0 && system_mode == NORMAL_MODE)) {   /// only writes first digit if it needs to change
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

void moveClockForward(uint16_t xpos, uint8_t ypos) {
  int16_t poX = 284;  // determines where we will print the digital clock
  int16_t poY = 8;
  // printTemp();
  tft.setTextSize(1);
  hh = hour(), mm = minute(), ss = second();  // uses the internal MCU to keep track of time, (refreshes MCU with RTC every 5 minutes by defualt) assigns time to global variables
  if (hh != xhh  && STATE == NORMAL_MODE) {  // check to see if hours have changed
    printDigit(poX, poY, hh, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);  // hours have changed, print new digit
    xhh = hh;
  }
  if (ars_initialized == 0) printColon(poX + 54, poY, TFT_RED, TFT_GRAY, TFT_YELLOW, 6);
  if (mm != xmm) {  // check to see if minutes have changed
    printDigit(353, poY, mm, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);
    xmm = mm;
  }
  if (ars_initialized == 0) printColon(407, poY, TFT_RED, TFT_GRAY, TFT_YELLOW, 6);  // if starting up from first time or coming back from settings mode...
  printDigit(422, poY, ss, TFT_RED, TFT_GRAY, TFT_YELLOW, NORMAL_MODE, 6);  // only print ss to clock in normal mode
}

void addBackground () {
  Serial.print("[addBackground()]: Created background for TFT ");
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
  Serial.println("[top_Bar()]: Printed top red bar to screen ");
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
        tft.drawCentreString("Dinghy Start", poX - 2, poY, 4);
        break;
      }
    case 2: {
        tft.drawCentreString("Match Racing", poX, poY, 4);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("Match Racing", poX - 2, poY, 4);
        break;
      }
  }
  // I think I can get rid of below, keeping for now to make sure it is not a hack to fix a bug
  // if (temp_timer_mm != timer_mm)  counterSetup(18, 65);   // only if the race type changes does the counter write to the screen
  temp_timer_mm = timer_mm;
}

void bottomBar () {
  Serial.print("[bottomBar()] Setup or change status of bottom bar to ");
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
        Serial.print("normal mode");
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
        Serial.print("settings mode");
        tft.drawCentreString("Done", 70, 317, 4);
        tft.drawCentreString("Select", 240, 317, 4);
        tft.drawCentreString("Change", 400, 317, 4);
        tft.setTextColor(TFT_YELLOW);
        tft.drawCentreString("Done", 68, 315, 4);
        tft.drawCentreString("Select", 238, 315, 4);
        tft.drawCentreString("Change", 398, 315, 4);
        break;
      }
    case CLOCK_CHANGE_MODE: {   // Clock Change Mode
        Serial.print("clock change mode");
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
  Serial.println();
}

void untitledBox (int poX, int poY, int widtH, int heighT, int diV) {  // box to left  poX = 8  poY = 50 widtH = 260 heighT = 217  blackBox1(8,50,260,217); diV = dividing line
  Serial.println("[untitledBox()]: Printing untitled box (timer/status) to TFT ");
  int middle = round(heighT * 0.718);
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
  Serial.print("[titledBox()]: Printing titled box (roll/warning) to TFT ");
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
  Serial.print("[rollTimeSetup()]: Printing rolling time counter to TFT");
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
  Serial.print("[warningTimeSetup()]: Printing warning time of ");
  Serial.print(warning_mm);
  Serial.print(":");
  Serial.print(warning_ss);
  Serial.print(" to TFT");
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
  Serial.println();
}

void rollingStart (uint8_t poX, uint8_t poY) {
  Serial.print("[rollingStart()]: Updating status display for rolling start ");
  tft.setTextColor(TFT_MAROON, TFT_BLACK);
  if (rolling_start) {
    Serial.print("rollingStart is enabled");
    tft.fillCircle(poX - 25, poY + 10, 12, TFT_GREEN);
  } else {
    Serial.print("rollingStart is disabled");
    tft.fillCircle(poX - 25, poY + 10, 12, TFT_RED);
  }
tft.drawString("Rolling Start", poX, poY, 4);
tft.setTextColor(TFT_YELLOW);
tft.drawString("Rolling Start", poX - 2, poY - 2, 4);
tft.drawCircle(poX - 25, poY + 10, 12, TFT_LIGHT_GRAY);
Serial.println();
}

void warningHorn (uint8_t poX, uint8_t poY) {
  Serial.print("[warningHorn()]: updating status display for warning horn ");
  tft.setTextColor(TFT_MAROON, TFT_BLACK);
  if (warning_horn) {
    tft.fillCircle(poX - 25, poY + 10, 12, TFT_GREEN);
  } else {
    tft.fillCircle(poX - 25, poY + 10, 12, TFT_RED);
  }
  tft.drawString("Warning Horn", poX, poY, 4);
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Warning Horn", poX - 2, poY - 2, 4);
  tft.drawCircle(poX - 25, poY + 10, 12, TFT_LIGHT_GRAY);
  Serial.println();
}

void printColon(uint16_t poX, uint16_t poY, uint32_t back_color, uint32_t back_text_color, uint32_t text_color, uint8_t font) {
  tft.setTextColor(back_text_color, back_color);
  tft.drawChar(':', poX, poY, font);
  tft.setTextColor(text_color);
  tft.drawChar(':', poX - 2, poY - 2, font);
}

void timeChangeFunc() {
  Serial.print("[timeChangeFunc()]: ");
  switch (timeChange) {
    case HOUR: {
        Serial.print("Hour selected ");
        writeSetupDigit (ss, 320, TFT_YELLOW, TFT_MAROON);
        //tft.fillRect(48, 105, 110, 82, TFT_YELLOW);
        writeSetupDigit (hh, 50, TFT_BLUE, TFT_GRAY);
        break;
      }
    case MINUTE: {
        Serial.print("Minute selected ");
        writeSetupDigit (hh, 50, TFT_YELLOW, TFT_MAROON);
        writeSetupDigit (mm, 185, TFT_BLUE, TFT_GRAY);
        break;
      }
    case SECOND: {
        Serial.print("Second selected ");
        writeSetupDigit (mm, 185, TFT_YELLOW, TFT_MAROON);
        writeSetupDigit (ss, 320, TFT_BLUE, TFT_GRAY);
        break;
      }
  }
  Serial.print("Time is now: ");  // testing testing testing
  Serial.print(hh);
  Serial.print(":");
  Serial.print(mm);
  Serial.print(":");
  Serial.print(ss);
  Serial.println();
}

void setupTimeChange() {   // sets up time change screen
  Serial.println("[setupTimeChange(): Set mode to clock change" );
  STATE = CLOCK_CHANGE_MODE;
  addBackground();
  bottomBar ();
  untitledBox(30, 95, 410, 100, 0);
  Serial.println("[setupTimeChange(): Printed clock change screen to TFT " );
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
  if (digit < 10) strcat(hoR, "0");  // takes it back to 0
  itoa(digit, tmp, 10);
  strcat(hoR, tmp);
  tft.setTextColor(back_text_color);
  tft.drawString(hoR, posX, 110, 8);
  tft.setTextColor(text_color);
  tft.drawString(hoR, posX - 3, 107, 8);
}

void setupSettingsScreen() {
  Serial.println("[setupSettingScreen()]: Printed settings screen to TFT ");
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
  for (int a = 0; a < 7; a++ ) {
    printSelectionValue(a, posY[a] + 2);
  }
  tft.setTextColor(TFT_YELLOW);
  tft.drawString("Race Type", 18, 20, 4);
  tft.drawString("Timer", 18, 55, 4);
  tft.drawString("Rolling Start", 18, 90, 4);
  tft.drawString("Roll Time", 18, 125, 4);
  tft.drawString("Warning Horn", 18, 160, 4);
  tft.drawString("Warning Time", 18, 195, 4);
  tft.drawString("Time", 18, 230, 4);

  for (int a = 0; a < 7; a++ ) {
    printSelectionValue(a, posY[a]);
  }
  selectItem(item_selected);  /// highlights row that is selected
}

void selectItem (int selection) {
  Serial.println("[selectItem()] ");
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
  if (selection == 0) {
    oldSelection = 6;  /// makes sure it writes over the old one and resets
  }
  //////////// Remove preious Highlight ///////////
  tft.fillRect(posX, posY[oldSelection], 447, 28, TFT_BLACK);
  tft.setTextColor(TFT_MAROON);
  tft.drawString(setting_type[oldSelection], 16, posY[oldSelection] + 2, 4);  // setting type
  tft.setTextColor(TFT_YELLOW);
  tft.drawString(setting_type[oldSelection], 18, posY[oldSelection], 4);

  tft.setTextColor(TFT_MAROON);
  printSelectionValue(oldSelection, posY[oldSelection] + 2); // background of setting value
  tft.setTextColor(TFT_YELLOW);
  printSelectionValue(oldSelection, posY[oldSelection]);   // front side of setting value

  //////////// Highlight ///////////////////////
  tft.fillRect(posX, posY[selection], 447, 28, TFT_YELLOW);
  tft.setTextColor(TFT_BLUE);
  tft.drawString(setting_type[selection], 16, posY[selection] + 2, 4);  // setting type
  printSelectionValue(selection, posY[selection]);     // setting value
}

void timerSetup(uint16_t poX, uint16_t poY) {   // Writes the initial countdown to mode
  Serial.print("[timerSetup()] Printing digits ");
  addZeroIfUnderTen(timer_mm);  // adds 0 if needed and prints number to console
  Serial.print(":");
  addZeroIfUnderTen(timer_ss);  // adds 0 if needed and prints number to console
  Serial.println(" to TFT ");
  printDigit(18, 65, timer_mm, TFT_BLACK, TFT_GRAY, TFT_AQUA, SETTINGS_MODE, 8);  /// why are we using settings mode?  this is confusing
  poX += 108;
  printColon(poX + 2, poY - 6, TFT_BLACK, TFT_GRAY, TFT_AQUA, 8);
  poX += 28;
  printDigit(poX, 65, timer_ss, TFT_BLACK, TFT_GRAY, TFT_AQUA, SETTINGS_MODE, 8);  /// why are we using settings mode??
}
