void printTime(uint8_t counterSelected) {
  Serial.print("[printTime()] ");
  switch (counterSelected) {
    case TIMER_SET: {
        Serial.print("Timer: ");
        addZeroIfUnderTen(timer_mm);  // adds 0 if needed and prints number to console
        Serial.print(":");
        if (timer_ss == 60) {  // will clean it up for serial display
          Serial.print("00 ");
        } else {
          addZeroIfUnderTen(timer_ss);  // adds 0 if needed and prints number to console
          Serial.print(" ");
        }
        break;
      }
    case ROLLING_TIME: {
        Serial.print("Roller: ");
        addZeroIfUnderTen(rolling_mm);  // adds 0 if needed and prints number to console
        Serial.print(":");
        if (rolling_ss == 60) {
          Serial.print("00 ");
        } else {
          addZeroIfUnderTen(rolling_ss);  // adds 0 if needed and prints number to console
          Serial.print(" ");
        }
        break;
      }
    case WARNING_TIME: {
        Serial.print("Warning: ");
        addZeroIfUnderTen(warning_mm);
        Serial.print(":");
        if (warning_ss == 60) {
          Serial.print("00 ");
        } else {
          addZeroIfUnderTen(warning_ss);
          Serial.print(" ");
        }
        break;
      }
  }
}

void addZeroIfUnderTen(int numberToCheck) {  // this just makes it pretty in serial console
  if (numberToCheck < 10) {
    Serial.print("0");
  }
  Serial.print(numberToCheck);
}

void printRaceType () {  // print to serial
  switch (race_type) {
    case 0: {
        Serial.print("Rule 26] ");
        break;
      }
    case 1: {
        Serial.print("Dinghy] ");
        break;
      }
    case 2: {
        break;
        Serial.print("Match] ");
      }
  }
}

void printTime() {
  time_t t = RTC.get();
  char buf[40];
  sprintf(buf, "%.2d:%.2d:%.2d %.2d%s%d ",
          hour(t), minute(t), second(t), day(t), monthShortStr(month(t)), year(t));
  Serial.println();
  Serial.print("Race start time: ");
  Serial.println(buf);
}
