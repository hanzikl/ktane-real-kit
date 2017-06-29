#ifdef DEBUGING
#define DEBUGING_WIRES
#endif


/*
  SETUP FOR WIRES
*/

#define WIRES_MAX_WIRES_COUNT 6

#define INPUT_MASK_WIRES 0b11111100 // must be in reversed order

#define WIRES_DISARM_LED 0

// setup data locations
#define WIRES_DATA_CURRENT_MASK 0
#define WIRES_DATA_CORRECT_MASK 1

const byte wires_input_connection[] = {4, 8, 16, 32, 64, 128};
const byte wires_output_connection[] = {128};


/*
   Getting and setting data
*/

// TODO:

/**
  TESTS
*/

void test_wires_output(byte module_number) {

  int delayTime = 250;
  byte pos = SRoffsetsOutput[module_number];

  for (int j = 0; j < 2; j++) {
    shift_register_output[pos] = wires_output_connection[WIRES_DISARM_LED];
    write_to_output_shift_register();
    shift_register_output[pos] = 0;
    write_to_output_shift_register();
    delay(delayTime);
  }
}


void test_wires_input(byte module_number) {

  byte outPos = SRoffsetsOutput[module_number];

  // TODO: short / long blink for each input position


}

/**
   MODULE LOGIC
*/


void setup_wires(byte module_number) {

  module_stage[module_number] = 0;

}

void update_wires(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];

  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = wires_output_connection[WIRES_DISARM_LED];
    return;
  }

  if (module_status[module_number] == MODULE_TESTING) {
    test_wires_output(module_number);
    test_wires_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  byte reading = get_module_sanitized_input(module_number, INPUT_MASK_WIRES, true);

  if (module_status[module_number] == MODULE_FAILED_TO_DISARM
      || module_status[module_number] == MODULE_DISARMING_IN_PROGRESS)  {
    if (reading == 0) {
      module_status[module_number] = MODULE_ARMED;
      shift_register_output[pos] = 0;
      // reset waiting for sequence show output

#ifdef DEBUGING_WIRES
      Serial.print(debug_print_char);
      Serial.println(F("RE-ARMING WIRES"));
#endif
    }
    return;
  }

  if (clockTicking) {
    // TODO:
  }

}






