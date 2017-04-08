#include "LedControl.h"

/*
   GENERAL SETUP
*/

long remainingTime = 5 * 60 * 1000l; // in millis
unsigned long previousMillis = 0; // time helper
const long interval = 7; // how often should system react in millis

boolean clockTicking = true;
double clockSpeedFactor = 2.5;

/*
   SETUP FOR COUNTDOWN DISLPAY
*/

LedControl countdown_display = LedControl(12, 11, 10, 1);
// pin 12 is connected to the MAX7219 pin 12
// pin 11 is connected to the CLK pin 11
// pin 10 is connected to LOAD pin 10
// 1 as we are only using 1 MAX7219

const int countdown_display_order = 0;

const int countdown_millisPosition = - 2; // do not show last two digits from millis
const int countdown_secondsPosition = countdown_millisPosition + 3;
const int countdown_minutesPosition = countdown_secondsPosition + 2;

boolean countdown_blinked = true; // helper for blinking dots


void setup()
{

  /*
      GENERAL SETUP
  */

  Serial.begin(9600);


  /*
      SETUP FOR COUNTDOWN DISLPAY
  */

  // the zero refers to the MAX7219 number, it is zero for 1 chip
  countdown_display.shutdown(0, false); // turn off power saving, enables display
  countdown_display.setIntensity(0, 8); // sets brightness (0~15 possible values)
  countdown_display.clearDisplay(0);// clear screen

}


void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    if (clockTicking) {
      remainingTime -= (long) round((currentMillis - previousMillis) * clockSpeedFactor);
    }

    previousMillis = currentMillis;

    clockTicking = (remainingTime > 0);

    /*
       Here comes the updates of device
    */

    update_countdown_display(currentMillis);

  }

}

