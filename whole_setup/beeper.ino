#ifdef DEBUGING
#define DEBUGING_BEEPER
#endif

/*
  SETUP FOR BUTTONS IN ORDER
*/

#define BEEPER_PIN 4
#define BEEPER_BEEP_LENGTH 30

void setup_beeper() {

  /*
    SETUP FUNCTION FOR BEEPER
  */

  pinMode(BEEPER_PIN, OUTPUT);

}


void update_beeper(long currentMillis) {

  int blinkMillis = currentMillis % 1000;

  if (remainingTime > 0) {

    // blink speed according to remaining time
    int blinkSpeedInversion = 1000;
    if (remainingTime < 30 * 1000) {
      blinkSpeedInversion = 500;
    }

    if (remainingTime < 10 * 1000) {
      blinkSpeedInversion = 200;
    }

    boolean beeper_on = clockTicking
                        && (blinkMillis % blinkSpeedInversion > blinkSpeedInversion / 2)
                        && (blinkMillis % blinkSpeedInversion < blinkSpeedInversion / 2 + BEEPER_BEEP_LENGTH);

    if (beeper_on) {
      digitalWrite(BEEPER_PIN, HIGH);
    } else {
      digitalWrite(BEEPER_PIN, LOW);
    }
    
  }
}

