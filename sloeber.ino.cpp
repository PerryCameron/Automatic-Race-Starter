#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-06-21 15:56:33

#include "Arduino.h"
#include <TFT_HX8357.h>
#include <DS3232RTC.h>
#include <Wire.h>
#include <TimeLib.h>
#include <JC_Button.h>
#include <string.h>

void setup() ;
void loop() ;
void readButtons() ;
void restartUnit () ;
void setDefaultMinutes() ;
void setupScreen() ;
void buttonA (void) ;
void buttonB (void) ;
void buttonC(void) ;
void printSelectionValue (int selection, int posY) ;
void change(uint8_t item) ;
void hornControl(void) ;
void r26_matchControl (void) ;
void dinghyControl () ;
void printTime(uint8_t counterSelected) ;
void addZeroIfUnderTen(int numberToCheck) ;
void printRaceType () ;
void printTime() ;
void printCounterMode() ;
void printRollingStartBool() ;
void printWarningHornBool() ;
void printSettingsVariables() ;
void printDigit(uint16_t poX, uint16_t poY, uint16_t digit, uint32_t back_color, uint32_t back_text_color, uint32_t text_color, uint8_t system_mode, uint8_t font) ;
void moveClockForward(uint16_t xpos, uint8_t ypos) ;
void addBackground () ;
void setupTopBar (int poX, int poY, uint32_t back_text_color) ;
void setupBottomBar () ;
void setupUntitledBox (int poX, int poY, int widtH, int heighT, int diV) ;
void setupTitledBox (int poX, int poY, int widtH, int heighT, int diV, uint16_t tColor) ;
void setupRollingTime() ;
void setupWarningTime() ;
void disableWarningTime() ;
void setupRollingStartIndicator (uint8_t poX, uint8_t poY) ;
void setupWarningHornIndicator (uint8_t poX, uint8_t poY) ;
void printColon(uint16_t poX, uint16_t poY, uint32_t back_color, uint32_t back_text_color, uint32_t text_color, uint8_t font) ;
void timeChangeFunc() ;
void setupTimeChange() ;
void writeSetupDigit(int digit, int posX, uint32_t text_color, uint32_t back_text_color) ;
void setupSettingsScreen() ;
void selectItem (int selection) ;
void setupTimer(uint16_t poX, uint16_t poY) ;
void performTaskEachSecond () ;
void countDown(uint8_t counterSelected, uint8_t count_mm, uint8_t count_ss) ;
void timerCountDown (uint8_t count_mm, uint8_t count_ss) ;
void rollingCountDown (uint8_t count_mm, uint8_t count_ss) ;
void warningCountDown (uint8_t count_mm, uint8_t count_ss) ;
void blowhorn(int duration) ;
void time_horn_to_off() ;
void hornBurst(int number, int duration, bool dinghy130) ;
void hornburstListener () ;

#include "Automatic_Race_Starter.ino"

#include "ButtonControl.ino"
#include "HornControl.ino"
#include "README.ino"
#include "SerialControl.ino"
#include "TestFunctions.ino"
#include "TftControl.ino"
#include "TimeControl.ino"

#endif
