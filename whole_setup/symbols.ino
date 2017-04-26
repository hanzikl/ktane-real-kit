#ifdef DEBUGING
#define DEBUGING_SYMBOLS
#endif


/*
  SETUP FOR SYMBOLS
*/

#define SYMBOLS_BUTTON_COUNT 4

#define SYMBOLS_DELAY_SHORT 500
#define SYMBOLS_DELAY_LONG 3000

#define SYMBOLS_WRONG_BLINK_TIME 100

#define INPUT_MASK_SYMBOLS 0b11110000 // must be in reversed order

#define SYMBOLS_DISARM_LED 4

// TODO: blink light under wrong buttons after error
// #define SYMBOLS_LAST_OK 255

// setup data locations
#define SYMBOLS_DATA_OK 0
#define SYMBOLS_DATA_ER 1
#define SYMBOLS_DATA_SQ 2

const byte symbols_input_connection[] = {16, 32, 64, 128};
const byte symbols_output_connection[] = {32, 4, 16, 8, 2};

byte symbols_stage = 0;


/*
   Getting and setting data
*/

byte getSymbolsCorrectButtons(byte module_number) {
  return module_data[module_number][SYMBOLS_DATA_OK] ;
}

byte getSymbolsWrongButtons(byte module_number) {
  return module_data[module_number][SYMBOLS_DATA_ER];
}

byte getSymbolsSequenceByte(byte module_number, byte byte_number) {
  return module_data[module_number][SYMBOLS_DATA_SQ + byte_number];
}

void setSymbolsCorrectButtons(byte module_number, byte value) {
  module_data[module_number][SYMBOLS_DATA_OK] = value;
}

void setSymbolsWrongButtons(byte module_number, byte value) {
  module_data[module_number][SYMBOLS_DATA_ER] = value;
}

void setSymbolsSequenceByte(byte module_number, byte byte_number, byte value) {
  module_data[module_number][SYMBOLS_DATA_SQ + byte_number] = value;
}

/**
  TESTS
*/

void test_symbols_output(byte module_number) {

  int delayTime = 600;

  for (int j = 0; j < 2; j++) {

    byte pos = SRoffsetsOutput[module_number];

    for (int i = 0; i < 4; i++) {
      shift_register_output[pos] = symbols_output_connection[i];
      write_to_output_shift_register();
      delay(delayTime);
    }

    shift_register_output[pos] = symbols_output_connection[SYMBOLS_DISARM_LED];
    write_to_output_shift_register();
    delay(delayTime);

  }

}


void test_symbols_input(byte module_number) {

  byte outPos = SRoffsetsOutput[module_number];

  int delayTime = 50; // ms

  boolean have[5] = {false, false, false, false};

  while (true) {

    update_shift_registers();
    delay(delayTime);

    byte input_value = get_module_input(module_number, INPUT_MASK_SYMBOLS, true);

    // figure out, which button is on
    for (int i = 0; i < 4; i++) {
      if (input_value == symbols_input_connection[i]) {
        have[i] = true;
      }
    }

    byte output_value = 0;
    for (int i = 0; i < 4; i++) {
      if (have[i]) {
        output_value += symbols_output_connection[i];
      }
    }

    if (have[0] && have[1] && have[2] && have[3]) {
      break;
    }

    shift_register_output[outPos] = output_value;

  }

  shift_register_output[outPos] = 0;

}

/**
   MODULE LOGIC
*/


void setup_symbols(byte module_number) {

  module_stage[module_number] = 0;

  setSymbolsCorrectButtons(module_number, 0);
  setSymbolsWrongButtons(module_number, 0);

  setSymbolsSequenceByte(module_number, 0, 0);
  setSymbolsSequenceByte(module_number, 1, 1);
  setSymbolsSequenceByte(module_number, 2, 2);
  setSymbolsSequenceByte(module_number, 3, 3);

}

void update_symbols(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];

  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = symbols_output_connection[SYMBOLS_DISARM_LED];
    return;
  }

  if (module_status[module_number] == MODULE_TESTING) {
    test_symbols_output(module_number);
    test_symbols_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  byte reading = get_module_sanitized_input(module_number, INPUT_MASK_SYMBOLS, true);

  if (module_status[module_number] == MODULE_FAILED_TO_DISARM
      || module_status[module_number] == MODULE_DISARMING_IN_PROGRESS)  {
    if (reading == 0) {
      module_status[module_number] = MODULE_ARMED;
      shift_register_output[pos] = 0;
      // reset waiting for sequence show output

#ifdef DEBUGING_SYMBOLS
      Serial.println(F("RE-ARMING SYMBOLS"));
#endif
    }
    return;
  }


  if (clockTicking) {

    /*
       SYMBOLS OUTPUT
    */

#ifdef DEBUGING_SYMBOLS_OUTPUT
    Serial.print("M");
    Serial.print(module_number);
    Serial.print(" S");
    Serial.print(module_stage[module_number]);
#endif

    byte output_value = getSymbolsCorrectButtons(module_number);
    if (millis() % (SYMBOLS_WRONG_BLINK_TIME * 2) > SYMBOLS_WRONG_BLINK_TIME) {
      // show both correct and wrong buttons
      output_value += getSymbolsWrongButtons(module_number);
    }

    shift_register_output[pos] = output_value;

    /*
       SYMBOLS INPUT
    */
    switch (reading) {
      case READING_ERROR:
        break;
      case 0:
        break;
      default:
        {
          byte base = getSymbolsSequenceByte(module_number, module_stage[module_number]);
#ifdef DEBUGING_SYMBOLS
          Serial.print("M");
          Serial.print(module_number);
          Serial.print(" S");
          Serial.print(module_stage[module_number]);
          Serial.print(" R");
          Serial.print(reading);
          Serial.print(" E");
          Serial.println(symbols_input_connection[base]);
#endif
          if (reading == symbols_input_connection[base]) {

            module_status[module_number] = MODULE_DISARMING_IN_PROGRESS;
            // add to correct buttons
            setSymbolsCorrectButtons(module_number,
                                     getSymbolsCorrectButtons(module_number)
                                     + symbols_output_connection[base]);

            // reset wrong buttons
            setSymbolsWrongButtons(module_number, 0);

            shift_register_output[pos] = getSymbolsCorrectButtons(module_number);

            module_stage[module_number]++;
          } else {

            // figure out, which button is on
            for (int i = 0; i < 4; i++) {
              if (reading == symbols_input_connection[i]) {
                // turn on corresponding LED

                // add to wrong buttons
                // FIXME: do not add if it is already there
                setSymbolsWrongButtons(module_number,
                                       getSymbolsWrongButtons(module_number)
                                       + symbols_output_connection[i]);
                break;
              }
            }

            module_status[module_number] = MODULE_DISARMING_IN_PROGRESS;
            addStrike();
          }
        }
    }
  }


  if (module_stage[module_number] >= SYMBOLS_BUTTON_COUNT) {
    module_status[module_number] = MODULE_DISARMED;
  }
}






