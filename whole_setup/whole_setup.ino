
/*
   GENERAL SETUP
*/

long remainingTime = 1 * 60 * 1000l; // in millis
unsigned long previousMillis = 0; // time helper
const long interval = 7; // how often should system react in millis

boolean clockTicking = true;
double clockSpeedFactor = 1;

byte strikes = 0;
byte max_strikes = 1;

#define MODULE_DISARMED 0
#define MODULE_ARMED 1
#define MODULE_FAILED_TO_DISARM 2
#define MODULE_DISARMING_IN_PROGRESS 3

#define READING_ERROR 255

#define DEBUGING

/*
   MODULES STATUS
*/

byte bio_status = MODULE_ARMED;


void setup()
{

  /*
      GENERAL SETUP
  */

#ifdef DEBUGING
  Serial.begin(9600);

  Serial.print("\nKeep Talking and Nobody Explodes REALISTIC KIT\n");
#endif

  setup_countdown_display();
  setup_buttons_in_order();

}


void addStrike() {

#ifdef DEBUGING
  Serial.println("STRIKE!");
#endif

  strikes++;
  if (strikes > max_strikes) {
    remainingTime = 0;
#ifdef DEBUGING
    Serial.println("IT IS OVER NOW!");
#endif
    return;
  }

  clockSpeedFactor *= 1.5;

#ifdef DEBUGING
  Serial.print("Current clock speed factor is ");
  Serial.println(clockSpeedFactor);
#endif

}


void settleModules() {

  if (bio_status != MODULE_DISARMED) {
    return;
  }

  clockTicking = false;

#ifdef DEBUGING
  Serial.println("ALL MODULES DISARMED");
#endif

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

