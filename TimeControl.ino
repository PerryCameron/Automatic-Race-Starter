void performTaskEachSecond () {  // syncs digital clock and all timers to update every second
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000; // 1000ms is 1 second b
    moveClockForward(340, 8);  // moves clock on TFT to next current time constant
    if (counter_enable == TIMER_COUNT) {  // set counter off of digital clock
      countDown(TIMER_SET, timer_mm, timer_ss);
      Serial.println();
    }
    if (counter_enable == WARNING_COUNT) {
      countDown(WARNING_TIME, warning_mm, warning_ss);
      Serial.println();
    }
    if (counter_enable == ROLLING_COUNT) {
      countDown(ROLLING_TIME, rolling_mm, rolling_ss);
      Serial.println();
    }
  }
}

void countDown(uint8_t counterSelected, uint8_t count_mm, uint8_t count_ss) {
  //  Serial.print("countDown(); ");

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
    case TIMER_SET: { // the timer is running
        timerCountDown(count_mm, count_ss);
        break;
      }
    case ROLLING_TIME: {
        rollingCountDown (count_mm, count_ss);
        break;
      }
    case WARNING_TIME: {
        warningCountDown (count_mm, count_ss);
        break;
      }
  }
  printTime(counterSelected);  // prints out timing info
  hornControl();  // determins if a buzzer and/or horn should sound
}

void timerCountDown (uint8_t count_mm, uint8_t count_ss) {
  timer_mm = count_mm;  /// save to global varialbles for next loop.  we uses these so function is universal to all counters
  timer_ss = count_ss;
  switch (count_ss) {
    case 60: {
        if (timer_mm != temp_timer_mm) {  // prevents from rewriting initial setup back to screen
          printDigit(154, 65, 0, TFT_BLACK, TFT_GRAY, TFT_AQUA, 2, 8); // prints 00 instead of 60
          Serial.println("Printing 00 to TFT");
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
        if (rolling_ss == 1) {   /// this resets rolling counter back to what it is set at - why did I do this here?
          //////// because i had to to make the roll counter seamlessly transition
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
  // once we reach 0, the line below it no longer runs buzzer control or anything.  Must solve this problem
  if (count_ss == 60 && count_mm == 0) {  // we have reached 0, remember count_XX is temporary
    //printTime(counterSelected);  // prints out timing info
    printTime(TIMER_COUNT);
    hornControl();  // determins if a buzzer and/or horn should sound
    printTime(); // prints time race started in console
    timer_mm = timer_set;  // This resets the timer back to what is in settings
    if (TIMER_COUNT && rolling_start) {  // if we are in timer mode and have reached 0:00 ...
      temp_rolling_mm = rolling_mm;  // save the rolling time for later   ---this looks like a hack
      temp_rolling_ss = rolling_ss;
      counter_enable = ROLLING_COUNT;
      printCounterMode();
       
    } else {  // we are not roll starting
      counter_enable = NO_COUNT;   // turn off counter since we are not roll starting
      printCounterMode();
    }
    Serial.print("timerSetup(18, 65) launched here: ");
    Serial.print(count_mm);
    Serial.print(":");
    Serial.println(count_ss);
    timer_ss = 0;  // necessary to set display correct on TFT
    timerSetup(18, 65);  // why doesn't this work for roll start?????
  }
}

void rollingCountDown (uint8_t count_mm, uint8_t count_ss) {
  rolling_mm = count_mm;  /// save to global varialbles for next loop.  we uses these so function is universal to all counters
  rolling_ss = count_ss;
  timer_mm = timer_set;  // reset counter back to what it started as in the settings.
  timer_ss = temp_timer_ss;  // reset counter back to what it started as in the settings.
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
  if (count_ss == 01 && count_mm == 0) {  /// this lets it roll seamlessly into the next timer
    counter_enable = TIMER_COUNT;
    Serial.println("counter_enable = TIMER_COUNT ");
  }
}

void warningCountDown (uint8_t count_mm, uint8_t count_ss) {
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
    counter_enable = TIMER_COUNT;   // turn off warning time and turn on counter time
    Serial.println("counter_enable = TIMER_COUNT ");
  }
}

void blowhorn(int duration) {  // starts horn and sets duration
  digitalWrite(RELAY_PIN, HIGH); // starts the horn
  horn_on = true;  // sets horn on, used in loop()
  previous_horn_millis = millis() + duration;  // this is time set for duration of horn
}

void time_horn_to_off() {  // this turns the horn off after duration set by blowhorn()
  if (previous_horn_millis < millis()) {  // if the horn has blown the elapsed time, variable set in blowhorn()
    digitalWrite(RELAY_PIN, LOW);
    horn_on = false;
  }
}

//  hornburst(numver of horns, duration of horns)  // there is a 500ms pause between each horn
void hornBurst(int number, int duration, bool dinghy130) {  // this controls number of horn bursts
  hornburst = number;
  hornburstduration = duration;
  if (dinghy130) duration += 500;
  blowhorn(duration);
  previous_burst_millis = millis() + 500 + duration; // duration at start of half second horn
  hornburst--;
}

void hornburstListener () {
  if (previous_burst_millis < millis()) {  // A second has gone by
    if (hornburst > 0) { // Do we need to do another hornburst?
      blowhorn(hornburstduration);  // blow horn for specified duration
      previous_burst_millis = millis() + 500 + hornburstduration;  // lets set another second
      hornburst--;  // this horn complete!!
    }
  }
}
