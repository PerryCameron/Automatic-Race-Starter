void hornControl(void) {
  //Serial.print("[hornControl() ");
  switch (race_type) {
    case 0: {   // Rule 26 starts
        Serial.print("[rule 26] ");
        if (counter_enable == TIMER_COUNT && active_warning_ss != 1) { // only chirp if the timer is active
          if (active_timer_mm == 5) {  // this is for the start sequence horn
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[Attention Signal] ");  // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 4) {  // this is for the 4 minute horn
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[P-flag up horn] ");  // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 1) {
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[P-flag down horn] "); // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 0) {
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[start horn] ");  // horn is blowing and only print at start of horn blow
          }
        }
        break;
      }
    case 1: {   // Dingy starts
        Serial.print("Dingy] ");
        dinghyControl();
        break;                            // needs code
      }
    case 2: {   // Match Racing
        Serial.print("Match] ");
        Serial.println();
        if (counter_enable == TIMER_COUNT && active_warning_ss != 1) { // only chirp if the counter is counting
          if (active_timer_mm == 10) {  // this is for the start sequence horn
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[Flag F Displayed] ");  // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 6) {  // this is for the 4 minute horn
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[Flag F Down] ");  // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 5) {  // this is for the 4 minute horn
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[Numeral Pennant UP] ");  // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 4) {
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[P-Flag UP] "); // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 1) {
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[P-Flag DOWN] "); // horn is blowing and only print at start of horn blow
          }
          if (active_timer_mm == 0) {
            r26_matchControl();
            if (active_timer_ss == 60) Serial.print("[Numeral Pennant Down] ");  // horn is blowing and only print at start of horn blow
          }
        }
        break;
      }
  }
}

void r26_matchControl (void) {
  switch (active_timer_ss) {
    case 30: {
        tone( buzzer, 2000, 300);
        Serial.print("[Buzzer 300ms] ");
        break;
      }
    case 20: {
        tone( buzzer, 2000, 300);
        Serial.print("[Buzzer 300ms] ");
        break;
      }
    case 10: {
        tone( buzzer, 2000, 300);
        Serial.print("[Buzzer 300ms] ");
        break;
      }
    case 5:  {
        tone( buzzer, 2000, 100);
        Serial.print("[Buzzer 100ms] ");
        break;
      }
    case 4:  {
        tone( buzzer, 2000, 100);
        Serial.print("[Buzzer 100ms] ");
        break;
      }
    case 3:  {
        tone( buzzer, 2000, 100);
        Serial.print("[Buzzer 100ms] ");
        break;
      }
    case 2:  {
        tone( buzzer, 2000, 100);
        Serial.print("[Buzzer 100ms] ");
        break;
      }
    case 1:  {
        tone( buzzer, 2000, 100);
        Serial.print("[Buzzer 100ms] ");
        break;
      }
    case 60:  {                          // Buzzer rolls to 60 which is really 0
        tone( buzzer, 2000, 1000);
        Serial.print("[Buzzer 1 second] [Horn 1 second] ");
        blowhorn(LONG);   // number - 1 = seconds blown
        //blowHorn(3);
        break;
      }
  }
}

void dinghyControl () {
  if (counter_enable == TIMER_COUNT && active_warning_ss != 1) { // only chirp if the counter is counting
    if (active_timer_mm == 3) {
      hornBurst(3, LONG, false);
      Serial.print("[Horn 3 Long 3:00] ");
    } else if (active_timer_mm == 2) {
      switch (active_timer_ss) {
        case 60: {
            hornBurst(2, LONG, false);
            Serial.print("[Horn 2 Long 2:00] ");
            break;
          }
      }
    } else if (active_timer_mm == 1) {
      switch (active_timer_ss) {
        case 30: {
            hornBurst(3, LONG, true);
            Serial.print("[Horn 1 Long 3 Short 1:30] ");
            break;
          }
        case 60: {
            hornBurst(1, LONG, false);
            Serial.print("[Horn 1 Long 1:00] ");
            break;
          }
      }
    } else if (active_timer_mm == 0) {
      switch (active_timer_ss) {
        case 60: {  // this blows the horn at 0
            blowhorn(LONG);
            Serial.print("[Horn 1 Long 0:00] ");
            break;
          }
        case 30: {
            hornBurst(3, SHORT, false);
            Serial.print("[Horn 3 Short 0:30] ");
            break;
          }
        case 20: {
            hornBurst(2, SHORT, false);
            Serial.print("[Horn 2 Short 0:20] ");
            break;
          }
        case 10: {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:10] ");
            break;
          }
        case 5:  {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:05] ");
            break;
          }
        case 4:  {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:04] ");
            break;
          }
        case 3:  {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:03] ");
            break;
          }
        case 2:  {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:02] ");
            break;
          }
        case 1:  {
            blowhorn(SHORT);
            Serial.print("[Horn 1 Short 0:01] ");
            break;
          }
      }
    }
  }
}
