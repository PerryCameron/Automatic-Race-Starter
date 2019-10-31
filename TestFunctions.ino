void printCounterMode () {  //NO_COUNT, TIMER_COUNT, ROLLING_COUNT, WARNING_COUNT
  switch (counter_enable) {
    case NO_COUNT: {
      Serial.print("system_enable = NO_COUNT");
        break;
      }
    case TIMER_COUNT: {
      Serial.print("system_enable = TIMER_COUNT");
        break;
      }
    case ROLLING_COUNT: {
      Serial.print("system_enable = ROLLING_COUNT");
        break;
      }
    case WARNING_COUNT: {
      Serial.print("system_enable = WARNING_COUNT");
        break;
      }

  }
}
