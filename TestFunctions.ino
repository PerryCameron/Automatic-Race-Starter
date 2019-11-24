void printCounterMode() {  //NO_COUNT, TIMER_COUNT, ROLLING_COUNT, WARNING_COUNT
  switch (counter_enable) {
    case NO_COUNT: {
        Serial.println("system_enable = NO_COUNT");
        break;
      }
    case TIMER_COUNT: {
        Serial.println("system_enable = TIMER_COUNT");
        break;
      }
    case ROLLING_COUNT: {
        Serial.println("system_enable = ROLLING_COUNT");
        break;
      }
    case WARNING_COUNT: {
        Serial.println("system_enable = WARNING_COUNT");
        break;
      }

  }
}

void printRollingStartBool() {
  Serial.print("rolling_start = ");
  if (rolling_start) {
    Serial.println("True");
  } else {
    Serial.println("False");
  }
}

void printWarningHornBool() {
  Serial.print("warning_horn = ");
  if (warning_horn) {
    Serial.println("True");
  } else {
    Serial.println("False");
  }
}

void printSettingsVariables() {
  Serial.println();
  Serial.println("=======Variable Settings========");
  // Serial.print("race_type =");
  // printRaceType();
  // Serial.println();
  printCounterMode();
  printRollingStartBool();
  printWarningHornBool();
  Serial.print("warning_mm = ");
  Serial.println(warning_mm);
  Serial.print("warning_ss = ");
  Serial.println(warning_ss);
  Serial.print("rolling_mm = ");
  Serial.println(rolling_mm);
  Serial.print("rolling_ss = ");
  Serial.println(rolling_ss);
  Serial.print("timer_mm = ");
  Serial.println(timer_mm);
  Serial.print("timer_ss = ");
  Serial.println(timer_ss);
}
