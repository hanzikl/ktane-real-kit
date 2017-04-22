/*
   SETUP FOR COUNTDOWN DISLPAY
*/

#include "LedControl.h"

LedControl countdown_display = LedControl(12, 11, 10, 1);
// pin 12 is connected to the MAX7219 pin 12
// pin 11 is connected to the CLK pin 11
// pin 10 is connected to LOAD pin 10
// 1 as we are only using 1 MAX7219

#define countdown_display_order 0

const int countdown_secondsPosition = 0;
const int countdown_minutesPosition = countdown_secondsPosition + 2;

boolean countdown_blinked = true; // helper for blinking dots

#ifdef DEBUGING
#define DEBUGING_COUNTDOWN
#endif

#ifdef DEBUGING_COUNTDOWN
int previous_second = 0;
#endif

/*
   COUNTDOWN DISPLAY methods
*/

void setup_countdown_display() {

  /*
    SETUP FOR COUNTDOWN DISLPAY
  */

  // the zero refers to the MAX7219 number, it is zero for 1 chip
  countdown_display.shutdown(0, false); // turn off power saving, enables display
  countdown_display.setIntensity(0, 8); // sets brightness (0~15 possible values)
  countdown_display.clearDisplay(0);// clear screen
}

void showCountdownNumberInPosition(int value, int pos) {
  // minutes position

  int val1 = value % 10;
  int val2 = value / 10;

  countdown_display.setDigit(countdown_display_order, pos, val1, countdown_blinked);
  countdown_display.setDigit(countdown_display_order, pos + 1, val2, false);

}

void showTime(long remainingTime) {

  long temptime = remainingTime;
  long heremillis = temptime % 1000l;

  temptime = (temptime - heremillis) / 1000;

  int seconds = temptime % 60;
  temptime = (temptime - seconds) / 60;

  int minutes = temptime % 60;
  // temptime = (temptime - minutes) / 60;

  if (remainingTime >= 60 * 1000l) {
    // cas je nad jednu minutu, ukazujeme minuty a sekundy
    showCountdownNumberInPosition(seconds, countdown_secondsPosition);
    showCountdownNumberInPosition(minutes, countdown_minutesPosition);

  } else {
    // ukazujeme sekundy a 2 cifry milisekund
    showCountdownNumberInPosition(heremillis / 10, countdown_secondsPosition);
    showCountdownNumberInPosition(seconds, countdown_minutesPosition);
  }

#ifdef DEBUGING_COUNTDOWN
  if (previous_second != seconds) {
    Serial.print("Countdown: ");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
    previous_second = seconds;
  }
#endif

}

void updateDisplayBlink(long currentMillis) {

  int blinkMillis = currentMillis % 1000;

  // blink speed according to remaining time
  int blinkSpeedInversion = 1000;
  if (remainingTime < 30 * 1000) {
    blinkSpeedInversion = 500;
  }

  if (remainingTime < 10 * 1000) {
    blinkSpeedInversion = 200;
  }

  countdown_blinked = (!clockTicking) || (blinkMillis % blinkSpeedInversion > blinkSpeedInversion / 2);
}

/*
   Helper function for COUNTDOWN DISLPAY
*/
void showEights(int segmCount) {
  for (int i = 0; i < segmCount; i++) {
    countdown_display.setDigit(countdown_display_order, i, 8, true);
  }
}

/*
   Helper function for COUNTDOWN DISLPAY
*/
void showClean(int segmCount) {
  for (int i = 0; i < segmCount; i++) {
    countdown_display.setChar(countdown_display_order, i, ' ', false);
  }
}


void update_countdown_display(long currentMillis) {

  if (remainingTime > 0) {

    updateDisplayBlink(currentMillis);
    showTime(remainingTime);

  } else {
    if (currentMillis % 250 > 100) {
      showClean(4);
    } else {
      showEights(4);
    }

  }
}

