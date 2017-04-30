#include <avr/pgmspace.h>

/*
   GENERAL SETUP
*/

long boomTime = 0;
long remainingTime = 1 * 75 * 1000l; // in millis
unsigned long previousMillis = 0; // time helper
unsigned long previousDisplayMillis = 0; // time helper
const long interval = 7; // how often should system react in millis
const long display_interval = 4; // how often should the output be refreshed

boolean clockTicking = true;
double clockSpeedFactor = 1;
double clockSpeedUp = 1.2;

boolean modules_testing = false;

byte strikes = 0;
byte max_strikes = 2;

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
#define MODULE_TYPE_TEST_OUTPUT 8
#define MODULE_TYPE_TEST_INPUT 9


// pocet bajtu na modul podle typu:
const byte modulesSRinputWidth[] = {0, 0, 1, 1, 1, 1, 1, 1, 0, 1};
const byte modulesSRoutputWidth[] = {0, 3, 1, 1, 1, 1, 6, 1, 2, 0};

#define READING_ERROR 255

#define DEBUGING

#ifdef DEBUGING
#define DEBUGING_INIT_MODULES
#endif


#define MODULE_MAX_COUNT 6
#define MODULE_DATA_SIZE 41
#define MODULE_MAX_OUTPUT_SIZE 6

// digits definition
const byte DIGIT_DEFINITION[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
  0b10000000, // dot
};

// 4byte 7segment current digit
byte segment_digit = 0;

/*
   MODULES STATUS
*/

byte bio_status = MODULE_ARMED;

byte module_types[MODULE_MAX_COUNT + 1];
byte module_status[MODULE_MAX_COUNT + 1];
byte module_stage[MODULE_MAX_COUNT + 1];

byte module_data[MODULE_MAX_COUNT + 1][MODULE_DATA_SIZE + 1];

// each module can have only one byte wide input
byte shift_register_input[MODULE_MAX_COUNT + 1];
byte shift_register_previous_input[MODULE_MAX_COUNT + 1];

// modules can have together only limited bytes wide output
byte shift_register_output[(MODULE_MAX_COUNT * MODULE_MAX_OUTPUT_SIZE) + 1];

/*
   MODULES PROPERTIES
*/


// offsets are computed for all modules plus one virtual module at the end
byte SRoffsetsInput[MODULE_MAX_COUNT + 2];
byte SRoffsetsOutput[MODULE_MAX_COUNT + 2];

void setup()
{

  /*
      GENERAL SETUP
  */

  // init random seed
  randomSeed(analogRead(A0));

#ifdef DEBUGING
  Serial.begin(9600);

  Serial.print(F("\nKeep Talking and Nobody Explodes REALISTIC KIT\n"));
#endif

  initModules();

  setup_beeper();
  setup_shift_registers();

  // initModulesTest();

}

void initModules() {
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    module_types[i] = MODULE_TYPE_MISSING;
    module_status[i] = MODULE_DISARMED;
    module_stage[i] = 0;
    for (int j = 0; j < MODULE_DATA_SIZE; j++) {
      module_data[i][j] = 0;
    }
  }


  // pozice 0 - modul Simon
  module_types[0] = MODULE_TYPE_SIMON;
  module_status[0] = MODULE_ARMED;

  // pozice 0 - modul Symbols
  //  module_types[0] = MODULE_TYPE_SYMBOLS;
  //  module_status[0] = MODULE_ARMED;

  // pozice 0 - modul test output
  // module_types[0] = MODULE_TYPE_TEST_OUTPUT;
  // module_status[0] = MODULE_ARMED;

  // pozice 1 - modul Display
  module_types[1] = MODULE_TYPE_DISPLAY;
  module_status[1] = MODULE_DISARMED;


#ifdef DEBUGING_INIT_MODULES
  Serial.print(F("Modules:"));
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    Serial.print(" #");
    Serial.print(i);
    Serial.print(":m");
    Serial.print(module_types[i]);
  }
  Serial.println();
  delay(500);
#endif

  // spocte offsety Shift registru vstupu a vystupu
  for (int i = 1; i < MODULE_MAX_COUNT + 2; i++) {
#ifdef DEBUGING_INIT_MODULES
    Serial.print(i);
    Serial.print(" w:");
    Serial.print(modulesSRoutputWidth[module_types[i - 1]]);
    Serial.println();
#endif
    // TODO: vypocet offsetu jeste neni uplne promysleny !!!
    SRoffsetsInput[MODULE_MAX_COUNT + 1 - i] =
      SRoffsetsInput[MODULE_MAX_COUNT + 2 - i] + modulesSRinputWidth[module_types[i - 1]];
    SRoffsetsOutput[i] = SRoffsetsOutput[i - 1] + modulesSRoutputWidth[module_types[i - 1]];
  }

  // snizi o jedna offsety vstupnich registru
  for (int i = 1; i < MODULE_MAX_COUNT + 2; i++) {

    if (SRoffsetsInput[i] > 0) {
      SRoffsetsInput[i]--;
    }
  }

#ifdef DEBUGING_INIT_MODULES
  Serial.print(F("InpOffs: "));
  for (int i = 0; i < MODULE_MAX_COUNT + 2; i++) {
    Serial.print(SRoffsetsInput[i]);
    Serial.print(" ");
  }
  Serial.print(F("\nOutOffs: "));
  for (int i = 0; i < MODULE_MAX_COUNT + 2; i++) {
    Serial.print(SRoffsetsOutput[i]);
    Serial.print(" ");
  }
  Serial.println();
#endif

  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    call_module_setup(i);
#ifdef DEBUGING_INIT_MODULES
    Serial.print(F("InitOk:"));
    Serial.println(i);
#endif

  }

}

void addStrike() {

#ifdef DEBUGING
  Serial.println(F("STRIKE!"));
#endif

  strikes++;
  if (strikes > max_strikes) {
    boomTime = remainingTime;
    remainingTime = 0;
#ifdef DEBUGING
    Serial.println(F("IT IS OVER NOW!"));
#endif
    return;
  }

  clockSpeedFactor *= clockSpeedUp;

#ifdef DEBUGING
  Serial.print(F("Current clock speed factor is "));
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
  Serial.println(F("ALL MODULES DISARMED"));
#endif

}

void loop() {

  if (modules_testing) {
    administerTests();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousDisplayMillis >= display_interval) {

    previousDisplayMillis = currentMillis;

    if (currentMillis - previousMillis >= interval) {
      // update modules completely

      if (clockTicking) {
        remainingTime -= (long) round((currentMillis - previousMillis) * clockSpeedFactor);
      }

      previousMillis = currentMillis;

      clockTicking = clockTicking && (remainingTime > 0);

      /*
         Here comes the update of modules
      */

      update_beeper(remainingTime);

      for (int i = 0; i < MODULE_MAX_COUNT; i++) {
        call_module_update(i, false);
      }

      if (clockTicking) {
        // check_buttons_in_order();
        settleModules();
      }

      update_shift_registers();

    } else {
      // update only output of modules

      for (int i = 0; i < MODULE_MAX_COUNT; i++) {
        call_module_update(i, true); // true for output only
      }

      write_to_output_shift_register();
    }

    // change shown digit of 7segment displays
    segment_digit++;
    if (segment_digit > 3) {
      segment_digit = 0;
    }

  }
}

