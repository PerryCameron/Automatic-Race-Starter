void buttonA (void) {   /// green button
  Serial.println();
  Serial.print("Button A pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        if (counter_enable == NO_COUNT) {   // no counter is running
          Serial.print("Timer was started");
          Serial.println();
          if (warning_horn) {  // warning horn is enabled
            hornBurst(5, LONG, false);  // blow 5 quick 500ms horns, 500ms in beteen horns
            Serial.println("Warning timer started: [5 horn burst]");
            temp_warning_mm = warning_mm;  // store original count for later
            temp_warning_ss = warning_ss;
            counter_enable = WARNING_COUNT;
          } else {
            counter_enable = TIMER_COUNT;  // start the timer
          }
          temp_timer_mm = timer_mm;  // store original count for later
          temp_timer_ss = timer_ss;
        }  else {   // a counter is running
          Serial.println("Timer was stopped and reset");
          counter_enable = NO_COUNT;    // stop counter
          timer_mm = temp_timer_mm;
          timer_ss = temp_timer_ss;  // reset all variables
          warning_mm = temp_warning_mm;
          warning_ss = temp_warning_ss;
          timerSetup(18, 65);  // reset the display to match reset of variables
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
  Serial.println();
  Serial.print("Button B pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Signal sent to blow horn");
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
  Serial.println();
  Serial.print("Button C pressed: ");
  switch (STATE) {
    case NORMAL_MODE: {
        Serial.println("Now in settings mode");
        if (counter_enable == NO_COUNT) {  // only let us go to settings if counter has not started.
          STATE = SETTINGS_MODE;  //   located in void loop()
          setupSettingsScreen();
          bottomBar();  // STATE = 1 here
        }
        break;
      }
    case SETTINGS_MODE: {   // if in settings mode this saves changes and moves back to normal mode
        Serial.println("Now in normal mode");
        STATE = NORMAL_MODE;  //  located in void loop()
        restartUnit();
        item_selected = RACE_TYPE;  ///  resets back to default
        break;
      }
    case CLOCK_CHANGE_MODE: {   // if in clock mode this saves changes and moves back to settings mode
        Serial.println("Now in clock change mode");
        setTime(hh, mm, ss, 2, 1, 2018);   // uncomment for final program
        RTC.set(now()); // uncomment for final program
        STATE = SETTINGS_MODE;  // turns off blinkItem()  located in void loop()
        setupSettingsScreen();
        bottomBar();
        break;
      }
      Serial.println();
  }
}

void printSelectionValue (int selection, int posY) { /// prints each selection in settings mode
  Serial.print("[printSelectionValue()] ");
  const char *race_mode[3];
  race_mode[RULE26] = "Rule 26";
  race_mode[DINGHY] = "Dinghy";
  race_mode[MATCH] = "Match";
  // uint8_t race_type = 0;  // What race type  Rule 26 = 0, Dinghy = 1, Match = 2
  const char *booleanAnswer[2];
  booleanAnswer[0] = "No";
  booleanAnswer[1] = "Yes";

  switch (selection) {  //RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME, TIME
    case RACE_TYPE: {
        Serial.print("Printing race type to TFT ");
        tft.drawString(race_mode[race_type], 300, posY + 2, 4);  // draws type of race string to TFT
        Serial.print(race_mode[race_type]);
        break;
      }
    case TIMER_SET: {
        Serial.print("Printing timer set to TFT ");
        char miN[3];
        itoa(timer_mm, miN, 10);
        char seC[3];
        itoa(timer_ss, seC, 10);
        char all[8] = "";
        if (timer_mm < 10) strcat(all, "0");
        strcat(all, miN);
        strcat(all, ":");
        if (timer_ss < 10) strcat(all, "0");
        strcat(all, seC);
        tft.drawString(all, 300, posY + 2, 4);
        Serial.print(all);
        break;
      }
    case ROLLING: {
        Serial.print("Printing rolling start (bool)");
        tft.drawString(booleanAnswer[rolling_start], 300, posY + 2, 4);
        Serial.print(booleanAnswer[rolling_start]);
        break;
      }
    case ROLLING_TIME: {
        Serial.print("Printing rolling time to TFT ");
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
        tft.drawString(all, 300, posY + 2, 4);
        Serial.print(all);
        break;
      }
    case WARNING_HORN: {
        Serial.print("Printing warning horn (bool) to TFT ");
        tft.drawString(booleanAnswer[warning_horn], 300, posY + 2, 4);
        Serial.print(booleanAnswer[warning_horn]);
        break;
      }
    case WARNING_TIME: {
        Serial.print("Printing warning time to TFT ");
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
        tft.drawString(all, 300, posY + 2, 4);
        Serial.print(all);
        break;
      }
    case TIME: {
        Serial.print("Printing clock to TFT ");
        tft.drawString("12:00:00", 300, posY + 2, 4);
        Serial.print("12:00:00 ");
        break;
      }
  }
  Serial.println();
}

void change(uint8_t item) {  ///{RACE_TYPE, TIMER_SET, ROLLING, ROLLING_TIME, WARNING_HORN, WARNING_TIME}
  Serial.print("[change()]: ");
  int posY[7] = {20, 55, 90, 125, 160, 195, 230 };
  tft.fillRect(300, posY[item], 90, 28, TFT_YELLOW);  // makes a clean transition when changing options in settings mode
  switch (item) {
    case RACE_TYPE: {  ////////////////// This Changes race type setting  {RULE_26, DINGHY, MATCH}
        Serial.print("[Race Type: ");
        race_type++;
        if (race_type > 2) race_type = 0; /// roll it over if nuber goes past number of race types
        printRaceType ();  // prints type to Serial
        tft.setTextColor(TFT_BLUE);
        printSelectionValue(RACE_TYPE, 20);  // prints type to TFT
        setDefaultMinutes();  // sets Minutes
        Serial.println(race_type);
        break;
      }
    case TIMER_SET: {
        Serial.print("Counter");
        timer_mm++;
        if (timer_mm > 10) timer_mm = 1;  // allows you to select between 1-9 minutes for timer.
        printSelectionValue(TIMER_SET, 55);
        timer_set = timer_mm;
        Serial.println(timer_mm);
        break;
      }
    case ROLLING: {
        Serial.print("Rolling start");
        if (rolling_start) {
          rolling_start = false;
        } else {
          rolling_start = true;
        }
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
        if (warning_horn) {
          warning_horn = false;
        } else {
          warning_horn = true;
        }
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
