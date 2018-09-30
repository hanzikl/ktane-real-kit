#ifdef DEBUGING
#define DEBUGING_KEYPAD
#endif


/*
  SETUP FOR KEYPAD
*/

#define KEYPAD_BUTTON_COUNT 4

#define KEYPAD_DELAY_SHORT 500
#define KEYPAD_DELAY_LONG 3000

#define KEYPAD_WRONG_BLINK_TIME 100

#define INPUT_MASK_KEYPAD 0b11110000 // must be in reversed order

#define KEYPAD_DISARM_LED 4

// TODO: blink light under wrong buttons after error
// #define KEYPAD_LAST_OK 255

// setup data locations
#define KEYPAD_DATA_OK 0
#define KEYPAD_DATA_ER 1
#define KEYPAD_DATA_SQ 2

const byte keypad_input_connection[] = {16, 32, 64, 128}; // U,R,D,L
const byte keypad_output_connection[] = {32, 8, 16, 64, 128};

byte keypad_stage = 0;


/*
   Getting and setting data
*/

byte getKeypadCorrectButtons(byte module_number) {
  return module_data[module_number][KEYPAD_DATA_OK] ;
}

byte getKeypadWrongButtons(byte module_number) {
  return module_data[module_number][KEYPAD_DATA_ER];
}

byte getKeypadSequenceByte(byte module_number, byte byte_number) {
  return module_data[module_number][KEYPAD_DATA_SQ + byte_number];
}

void setKeypadCorrectButtons(byte module_number, byte value) {
  module_data[module_number][KEYPAD_DATA_OK] = value;
}

void setKeypadWrongButtons(byte module_number, byte value) {
  module_data[module_number][KEYPAD_DATA_ER] = value;
}

void setKeypadSequenceByte(byte module_number, byte byte_number, byte value) {
  module_data[module_number][KEYPAD_DATA_SQ + byte_number] = value;
}

/**
  TESTS
*/

void test_keypad_output(byte module_number) {

  int delayTime = 600;

  for (int j = 0; j < 2; j++) {

    byte pos = SRoffsetsOutput[module_number];

    for (int i = 0; i < 4; i++) {
      shift_register_output[pos] = keypad_output_connection[i];
      write_to_output_shift_register();
      delay(delayTime);
    }

    shift_register_output[pos] = keypad_output_connection[KEYPAD_DISARM_LED];
    write_to_output_shift_register();
    delay(delayTime);

  }

}


void test_keypad_input(byte module_number) {

  byte outPos = SRoffsetsOutput[module_number];

  int delayTime = 50; // ms

  boolean have[5] = {false, false, false, false};

  while (true) {

    update_shift_registers();
    delay(delayTime);

    byte input_value = get_module_input(module_number, INPUT_MASK_KEYPAD, true);

    // figure out, which button is on
    for (int i = 0; i < 4; i++) {
      if (input_value == keypad_input_connection[i]) {
        have[i] = true;
      }
    }

    byte output_value = 0;
    for (int i = 0; i < 4; i++) {
      if (have[i]) {
        output_value += keypad_output_connection[i];
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


void setup_keypad(byte module_number) {

  module_stage[module_number] = 0;

  setKeypadCorrectButtons(module_number, 0);
  setKeypadWrongButtons(module_number, 0);

}

void update_keypad(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];

  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = keypad_output_connection[KEYPAD_DISARM_LED];
    return;
  }

  if (module_status[module_number] == MODULE_TESTING) {
    test_keypad_output(module_number);
    test_keypad_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  byte reading = get_module_sanitized_input(module_number, INPUT_MASK_KEYPAD, true);

  if (module_status[module_number] == MODULE_FAILED_TO_DISARM
      || module_status[module_number] == MODULE_DISARMING_IN_PROGRESS)  {
    if (reading == 0) {
      module_status[module_number] = MODULE_ARMED;
      shift_register_output[pos] = 0;
      // reset waiting for sequence show output

#ifdef DEBUGING_KEYPAD
      Serial.print(debug_print_char);
      Serial.println(F("RE-ARMING KEYPAD"));
#endif
    }
    return;
  }


  if (clockTicking) {

    /*
       KEYPAD OUTPUT
    */

#ifdef DEBUGING_KEYPAD_OUTPUT
    Serial.print(debug_print_char);
    Serial.print("M");
    Serial.print(module_number);
    Serial.print(" S");
    Serial.print(module_stage[module_number]);
#endif

    byte output_value = getKeypadCorrectButtons(module_number);
    if (millis() % (KEYPAD_WRONG_BLINK_TIME * 2) > KEYPAD_WRONG_BLINK_TIME) {
      // show both correct and wrong buttons
      output_value += getKeypadWrongButtons(module_number);
    }

    shift_register_output[pos] = output_value;

    /*
       KEYPAD INPUT
    */
    switch (reading) {
      case READING_ERROR:
        break;
      case 0:
        break;
      default:
        {
          byte base = getKeypadSequenceByte(module_number, module_stage[module_number]);
#ifdef DEBUGING_KEYPAD
          Serial.print(debug_print_char);
          Serial.print("M");
          Serial.print(module_number);
          Serial.print(" S");
          Serial.print(module_stage[module_number]);
          Serial.print(" R");
          Serial.print(reading);
          Serial.print(" E");
          Serial.println(keypad_input_connection[base]);
#endif
          if (reading == keypad_input_connection[base]) {

            module_status[module_number] = MODULE_DISARMING_IN_PROGRESS;
            // add to correct buttons
            setKeypadCorrectButtons(module_number,
                                     getKeypadCorrectButtons(module_number)
                                     + keypad_output_connection[base]);

            // reset wrong buttons
            setKeypadWrongButtons(module_number, 0);

            shift_register_output[pos] = getKeypadCorrectButtons(module_number);

            module_stage[module_number]++;
          } else {

            // figure out, which button is on
            for (int i = 0; i < 4; i++) {
              if (reading == keypad_input_connection[i]) {
                // turn on corresponding LED

                // add to wrong buttons
                byte wrong = getKeypadWrongButtons(module_number);
                byte ok = getKeypadCorrectButtons(module_number);
                byte wrong_plus = keypad_output_connection[i];

                if ((wrong_plus & (ok | wrong)) == 0) {
                  setKeypadWrongButtons(module_number, wrong + wrong_plus);
                }
                break;
              }
            }

            module_status[module_number] = MODULE_DISARMING_IN_PROGRESS;
            addStrike();
          }
        }
    }
  }


  if (module_stage[module_number] >= KEYPAD_BUTTON_COUNT) {
    module_status[module_number] = MODULE_DISARMED;
  }
}






