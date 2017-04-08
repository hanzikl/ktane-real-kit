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

const int countdown_millisPosition = - 2; // do not show last two digits from millis
const int countdown_secondsPosition = countdown_millisPosition + 3;
const int countdown_minutesPosition = countdown_secondsPosition + 2;

boolean countdown_blinked = true; // helper for blinking dots

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


/*
   Helper function for COUNTDOWN DISLPAY
*/
void showSeconds(int seconds) {
  int seconds0 = seconds % 10;
  int seconds1 = seconds / 10;
  countdown_display.setDigit(countdown_display_order, countdown_secondsPosition, seconds0, countdown_blinked);
  countdown_display.setDigit(countdown_display_order, countdown_secondsPosition + 1, seconds1, false);
}

/*
   Helper function for COUNTDOWN DISLPAY
*/
void showMinutes(int minutes) {

  int minutes0 = minutes % 10;
  //int minutes1 = minutes / 10;
  countdown_display.setDigit(countdown_display_order, countdown_minutesPosition, minutes0, countdown_blinked);
  //countdown_display.setDigit(countdown_display_order, countdown_minutesPosition + 1, minutes1, false);

}

/*
   Helper function for COUNTDOWN DISLPAY
*/
void showMillis(int milliseconds) {

  //int millis0 = milliseconds % 100;
  //int millis1 = (milliseconds / 10) % 10;
  int millis2 = milliseconds / 100;

  //countdown_display.setDigit(countdown_display_order, countdown_millisPosition + 0, millis0, false);
  //countdown_display.setDigit(countdown_display_order, countdown_millisPosition + 1, millis1, false);
  countdown_display.setDigit(countdown_display_order, countdown_millisPosition + 2, millis2, false);

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

  int blinkMillis = currentMillis % 1000;

  if (remainingTime > 0) {

    long temptime = remainingTime;
    long heremillis = temptime % 1000l;

    temptime = (temptime - heremillis) / 1000;

    int seconds = temptime % 60;
    temptime = (temptime - seconds) / 60;

    int minutes = temptime % 60;
    temptime = (temptime - minutes) / 60;

    // blink speed according to remaining time
    int blinkSpeedInversion = 1000;
    if (remainingTime < 30 * 1000) {
      blinkSpeedInversion = 500;
    }

    if (remainingTime < 10 * 1000) {
      blinkSpeedInversion = 200;
    }

    countdown_blinked = (!clockTicking) || (blinkMillis % blinkSpeedInversion > blinkSpeedInversion / 2);

    showMillis(heremillis);
    showSeconds(seconds);
    showMinutes(minutes);

  } else {
    if (currentMillis % 250 > 100) {
      showClean(4);
    } else {
      showEights(4);
    }

  }
}

