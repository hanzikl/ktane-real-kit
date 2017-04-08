#include "LedControl.h"

/*
   GENERAL SETUP
*/

long remainingTime = 5 * 60 * 1000l; // in millis
unsigned long previousMillis = 0; // time helper
const long interval = 7; // how often should system react in millis

boolean clockTicking = true;
double clockSpeedFactor = 1;

byte strikes = 0;
byte max_strikes = 1;

#define MODULE_DISARMED 0
#define MODULE_ARMED 1
#define MODULE_FAILED_TO_DISARM 2

#define READING_ERROR 255

/*
   SETUP FOR COUNTDOWN DISLPAY
*/

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
  SETUP FOR BUTTONS IN ORDER
*/

#define BIO_PIN_1 2
#define BIO_PIN_2 3

byte bio_previous_reading = 0x00;
byte bio_status = MODULE_ARMED;


void setup()
{

  /*
      GENERAL SETUP
  */

  Serial.begin(9600);

  Serial.print("\nKeep Talking and Nobody Explodes REALISTIC KIT\n");

  setup_countdown_display();
  setup_buttons_in_order();

}


void addStrike() {

  Serial.println("STRIKE!");

  strikes++;
  if (strikes > max_strikes) {
    remainingTime = 0;
    return;
  }

  clockSpeedFactor *= 1.5;

  Serial.print("Current clock speed factor is ");
  Serial.println(clockSpeedFactor);

}


void settleModules() {

  if (bio_status != MODULE_DISARMED) {
    return;
  }

  clockTicking = false;

  Serial.println("ALL MODULES DISARMED");
  
}


void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {

    if (clockTicking) {
      remainingTime -= (long) round((currentMillis - previousMillis) * clockSpeedFactor);
    }

    previousMillis = currentMillis;

    clockTicking = clockTicking && (remainingTime > 0);

    /*
       Here comes the updates of device
    */

    update_countdown_display(currentMillis);

    if (clockTicking) {
      check_buttons_in_order();
      settleModules();
    }

  }

}

