#ifdef DEBUGING
#define DEBUGING_BIGB
#define DEBUGING_BIGB_INPUT
//#define DEBUGING_BIGB_OUTPUT
#endif

/*
  SETUP FOR BIG BUTTON
*/


#define INPUT_MASK_BIGB 0b00010000 // must be in reversed order

#define BIGB_DISARM_LED 0

/**
   3 byte rules - 16xCOLOR + CORRECT_DIGIT for each strike (0-2 strikes)
   color [1-4], digit [0-9]
*/
#define BIGB_DATA_RULES 0
#define BIGB_DATA_PUSHED 3    // if the big button was pushed
#define BIGB_DATA_PT 4        // time of big button push - 4byte variable

#define BIGB_COLOR_COUNT 4

#define BIGB_DELAY_SHORT 300  // millisecond maximum for push and release
#define BIGB_LIGHTUP_DURATION 500 // lightup duration in milliseconds

const byte big_button_output_connection[] = {128, 32, 8, 16, 64}; // DISARM INDICATOR, BLUE, YELLOW, WHITE, RED

/*
   Getting and setting data
*/


byte getBigButtonRules(byte module_number, byte byte_number) {
  return module_data[module_number][BIGB_DATA_RULES + byte_number];
}

byte getBigButtonPushed(byte module_number) {
  return module_data[module_number][BIGB_DATA_PUSHED];
}

unsigned long getBigButtonPushTime(byte module_number) {

  unsigned long value = 0;
  value = (value << 8) + module_data[module_number][BIGB_DATA_PT];
  value = (value << 8) + module_data[module_number][BIGB_DATA_PT + 1];
  value = (value << 8) + module_data[module_number][BIGB_DATA_PT + 2];
  value = (value << 8) + module_data[module_number][BIGB_DATA_PT + 3];

  return value;
}

void setBigButtonRules(byte module_number, byte byte_number, byte value) {
  module_data[module_number][BIGB_DATA_RULES + byte_number] = value;
}

void setBigButtonPushed(byte module_number, byte value) {
  module_data[module_number][BIGB_DATA_PUSHED] = value;
}

void setBigButtonPushTime(byte module_number, unsigned long value) {

  module_data[module_number][BIGB_DATA_PT] = (value >> 24) & 0xFF;
  module_data[module_number][BIGB_DATA_PT + 1] = (value >> 16) & 0xFF;
  module_data[module_number][BIGB_DATA_PT + 2] = (value >> 8) & 0xFF;
  module_data[module_number][BIGB_DATA_PT + 3] = value & 0xFF;

}

/**
   TESTS
*/

void test_big_button_output(byte module_number) {

  int delayTime = 600;
  byte pos = SRoffsetsOutput[module_number];

  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < BIGB_COLOR_COUNT; i++) {
      shift_register_output[pos] = big_button_output_connection[i + 1];
      write_to_output_shift_register();
      delay(delayTime);
    }

    shift_register_output[pos] = big_button_output_connection[BIGB_DISARM_LED];
    write_to_output_shift_register();
    delay(delayTime);

    /*
        shift_register_output[pos] = 0;
        write_to_output_shift_register();
    */
  }

}

void test_big_button_input(byte module_number) {

  byte outPos = SRoffsetsOutput[module_number];

  int delayTime = 50; // ms

  shift_register_output[outPos] = 0;
  while (true) {

    update_shift_registers();
    delay(delayTime);

    byte input_value = get_module_input(module_number, INPUT_MASK_BIGB, true);

    if (input_value > 0) {
      break;
    }

  }

  shift_register_output[outPos] = 0;

}

/**
   MODULE LOGIC
*/

void setup_big_button(byte module_number) {

  module_stage[module_number] = 0;
  setBigButtonRules(module_number, 0, 0);
  setBigButtonRules(module_number, 1, 0);
  setBigButtonRules(module_number, 2, 0);

}

boolean check_button_release_ok(byte module_number) {

  byte rules = getBigButtonRules(module_number, strikes % 3);
  if (rules / 16 == 0) {
    // push and release expected and executed

#ifdef DEBUGING_BIGB_INPUT
    Serial.print(debug_print_char);
    Serial.print("M:");
    Serial.print(module_number);
    Serial.println(" PR OK");
#endif
    return true;
  }

  byte correct_number = rules % 16;
  for (int i = 0; i < 4; i++) {
    if (correct_number == display_digits[i]) {

#ifdef DEBUGING_BIGB_INPUT
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.print(" R");
      Serial.print(rules);
      Serial.print(" D");
      Serial.print(i);
      Serial.print(" C");
      Serial.println(correct_number);
#endif
      return true;
    }
  }

#ifdef DEBUGING_BIGB_INPUT
  Serial.print(debug_print_char);
  Serial.print("M");
  Serial.print(module_number);
  Serial.print(" C");
  Serial.println(correct_number);
#endif

  return false;

}

boolean check_duration_and_update(byte module_number, byte pos) {

  byte color = getBigButtonRules(module_number, strikes % 3) / 16;

  long held_time = millis() - getBigButtonPushTime(module_number);
  boolean not_short = held_time > BIGB_DELAY_SHORT;

  if (color == 0 && not_short) {
    // push and release was expected, but button is held too long

#ifdef DEBUGING_BIGB_INPUT
    Serial.print(debug_print_char);
    Serial.print("M");
    Serial.print(module_number);
    Serial.println(" TL");
#endif

    return false;
  }

  if (not_short) {

    int output_value = big_button_output_connection[color];

    if (held_time < BIGB_DELAY_SHORT + BIGB_LIGHTUP_DURATION) {
      // light the indicator up gradually
      long light_level_percent = (held_time - BIGB_DELAY_SHORT) * 100 / BIGB_LIGHTUP_DURATION;

      if (millis() % 100 > light_level_percent) {
        output_value = 0;
      }
    }

    if (color <= BIGB_COLOR_COUNT) {
#ifdef DEBUGING_BIGB_OUTPUT
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.print(" C:");
      Serial.println(color);
#endif
      shift_register_output[pos] = output_value;
    } else {
#ifdef DEBUGING_BIGB_OUTPUT
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.print(" C!!");
      Serial.println(color);
#endif
    }
  }

  return true;
}

void update_big_button(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];
  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = big_button_output_connection[BIGB_DISARM_LED];
    return;
  }

  if (module_status[module_number] == MODULE_TESTING) {
    test_big_button_output(module_number);
    test_big_button_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  byte reading = get_module_sanitized_input(module_number, INPUT_MASK_BIGB, true);

  if (module_status[module_number] == MODULE_FAILED_TO_DISARM)  {
    if (reading == 0) {
      module_status[module_number] = MODULE_ARMED;
      shift_register_output[pos] = 0;

      setBigButtonPushed(module_number, 0);

#ifdef DEBUGING_BIGB
      Serial.println(F("RE-ARMING BIGB"));
#endif
    }
    return;
  }

  if (getBigButtonPushed(module_number) == 1) {

    if (reading == 0) {
      // button was released

#ifdef DEBUGING_BIGB_INPUT
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.println(F(" released"));
#endif

      if (check_button_release_ok(module_number)) {
        module_status[module_number] = MODULE_DISARMED;
      } else {
        module_status[module_number] = MODULE_FAILED_TO_DISARM;
        addStrike();
      }

      setBigButtonPushed(module_number, 0);
    } else {
      // button is held

      if (!check_duration_and_update(module_number, pos)) {
        // button is held too long
        module_status[module_number] = MODULE_FAILED_TO_DISARM;
        addStrike();
      }

    }

  } else {

    if (reading == 0) {
      // nothing happens
    } else {
      // button was pushed - write this down

#ifdef DEBUGING_BIGB_INPUT
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.println(F(" pushed"));
#endif

      setBigButtonPushed(module_number, 1);
      setBigButtonPushTime(module_number, millis());
    }

  }

}
