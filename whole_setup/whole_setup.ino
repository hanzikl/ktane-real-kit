
/*
   GENERAL SETUP
*/

long remainingTime = 1 * 75 * 1000l; // in millis
unsigned long previousMillis = 0; // time helper
const long interval = 7; // how often should system react in millis

boolean clockTicking = true;
double clockSpeedFactor = 1;

boolean modules_testing = false;

byte strikes = 0;
byte max_strikes = 1;

#define MODULE_TESTING 0
#define MODULE_DISARMED 1
#define MODULE_ARMED 2
#define MODULE_FAILED_TO_DISARM 3
#define MODULE_DISARMING_IN_PROGRESS 4


#define MODULE_TYPE_MISSING 0
#define MODULE_TYPE_DISPLAY 1
#define MODULE_TYPE_SIMON 2
#define MODULE_TYPE_BIGB 3
#define MODULE_TYPE_WIRES 4
#define MODULE_TYPE_SYMBOLS 5
#define MODULE_TYPE_MEMORY 6
#define MODULE_TYPE_MAZE 7

#define READING_ERROR 255

#define DEBUGING

#define MODULE_MAX_COUNT 6
#define MODULE_DATA_SIZE 41

/*
   MODULES STATUS
*/

byte bio_status = MODULE_ARMED;

byte module_types[MODULE_MAX_COUNT + 1];
byte module_status[MODULE_MAX_COUNT + 1];
byte module_stage[MODULE_MAX_COUNT + 1];

byte module_data[MODULE_MAX_COUNT + 1][MODULE_DATA_SIZE + 1];

/*
   MODULES PROPERTIES
*/

#include <avr/pgmspace.h>

const byte modulesSRinputWidth[] PROGMEM = {0, 0, 1, 0, 0, 0, 0};
const byte modulesSRoutputWidth[] PROGMEM = {0, 0, 1, 0, 0, 0, 0};

// offsets are computed for all modules plus one virtual module at the end
byte SRoffsetsInput[MODULE_MAX_COUNT + 2];
byte SRoffsetsOutput[MODULE_MAX_COUNT + 2];

void setup()
{

  /*
      GENERAL SETUP
  */

#ifdef DEBUGING
  Serial.begin(9600);

  Serial.print("\nKeep Talking and Nobody Explodes REALISTIC KIT\n");
#endif

  initModules();

  setup_beeper();
  // setup_countdown_display();
  // setup_buttons_in_order();

}

void initModules() {
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    module_types[i] = 0;
    module_status[i] = MODULE_DISARMED;
    module_stage[i] = 0;
    for (int j = 0; j < MODULE_DATA_SIZE; j++) {
      module_data[i][j] = 0;
    }
  }

  // pozice 0 - modul Simon
  module_types[0] = MODULE_TYPE_SIMON;
  module_status[0] = MODULE_ARMED;

  // spocte offsety Shift registru vstupu a vystupu
  for (int i = 1; i < MODULE_MAX_COUNT + 1; i++) {
    SRoffsetsInput[i] = SRoffsetsInput[i - 1] + modulesSRinputWidth[module_types[i - 1]];
    SRoffsetsOutput[i] = SRoffsetsOutput[i - 1] + modulesSRoutputWidth[module_types[i - 1]];
  }


  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    call_module_setup(i);
  }

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

  /*
    if (bio_status != MODULE_DISARMED) {
      return;
    }
  */

  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    if (module_status[i] > MODULE_DISARMED) {
      return;
    }
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

    // update_countdown_display(currentMillis);
    update_beeper(remainingTime);

    for (int i = 0; i < MODULE_MAX_COUNT; i++) {
      call_module_update(i);
    }

    if (clockTicking) {
      // check_buttons_in_order();
      // settleModules();
    }

  }

}

