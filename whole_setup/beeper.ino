#ifdef DEBUGING
//#define DEBUGING_BEEPER
#endif

/*
  SETUP FOR BEEPER
*/

#define BEEPER_PIN 12
#define BEEPER_BEEP_LENGTH 30
#define BEEPER_SPEED_LEVEL_ONE_SECONDS 10
#define BEEPER_SPEED_LEVEL_ONE_INV 500 // more is slower
#define BEEPER_SPEED_LEVEL_TWO_SECONDS 5
#define BEEPER_SPEED_LEVEL_TWO_INV 200 // more is slower

void setup_beeper() {

  /*
    SETUP FUNCTION FOR BEEPER
  */

  pinMode(BEEPER_PIN, OUTPUT);

}


void beep_here() {


#ifdef DEBUGING_BEEPER
  Serial.println(F("BEEP"));
#endif
  digitalWrite(BEEPER_PIN, HIGH);
  delay(BEEPER_BEEP_LENGTH);
  digitalWrite(BEEPER_PIN, LOW);
}

void update_beeper(long currentMillis) {

  int blinkMillis = currentMillis % 1000;

  boolean beeper_on = false;

  if (remainingTime > 0) {

    // blink speed according to remaining time
    int blinkSpeedInversion = 1000;
    if (remainingTime < BEEPER_SPEED_LEVEL_ONE_SECONDS * 1000) {
      blinkSpeedInversion = BEEPER_SPEED_LEVEL_ONE_INV;
    }

    if (remainingTime < BEEPER_SPEED_LEVEL_TWO_SECONDS * 1000) {
      blinkSpeedInversion = BEEPER_SPEED_LEVEL_TWO_INV;
    }

    beeper_on = clockTicking && (blinkMillis % blinkSpeedInversion < BEEPER_BEEP_LENGTH);
  }

  if (beeper_on) {
    digitalWrite(BEEPER_PIN, HIGH);
#ifdef DEBUGING_BEEPER
    Serial.println(F("BEEP"));
#endif
  } else {
    digitalWrite(BEEPER_PIN, LOW);
  }

}

