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
#define WIRES_DATA_TARGET_MASK 1

// input is connected directly (1, 2, 4, 8, 16, 32), not using wires_input_connection transformation
const byte wires_output_connection[] = {128};

/*
   Getting and setting data
*/

byte getWiresCurrentMask(byte module_number) {
  return module_data[module_number][WIRES_DATA_CURRENT_MASK];
}

byte getWiresTargetMask(byte module_number) {
  return module_data[module_number][WIRES_DATA_TARGET_MASK];
}

void setWiresCurrentMask(byte module_number, byte value) {
  module_data[module_number][WIRES_DATA_CURRENT_MASK] = value;
}

void setWiresTargetMask(byte module_number, byte value) {
  module_data[module_number][WIRES_DATA_TARGET_MASK] = value;
}

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
  // no setup needed
  // all data should be set through serial comlink
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

  if (clockTicking) {
    byte current_mask = getWiresCurrentMask(module_number);
    byte target_mask = getWiresTargetMask(module_number);

    byte masked_reading = reading & current_mask;

    // check if target wire was cut somehow with error (more wires at a time maybe?) in previous change
    if ((target_mask & current_mask) == 0) {
      // target wire was cut
      module_status[module_number] = MODULE_DISARMED;
    }

    if (masked_reading != current_mask) {

#ifdef DEBUGING_WIRES
      Serial.print(debug_print_char);
      Serial.print("M");
      Serial.print(module_number);
      Serial.print(" R");
      Serial.print(reading);
      Serial.print(" CM");
      Serial.print(current_mask);
      Serial.print(" L");
      Serial.print(masked_reading);
      Serial.print(" T");
      Serial.print(target_mask);
      Serial.println();
#endif

      // input changed on relevant positions
      if (current_mask - masked_reading == target_mask) {
        // target wire was cut in this change
        module_status[module_number] = MODULE_DISARMED;
      } else {
        // wrong wire was cut
        setWiresCurrentMask(module_number, masked_reading);
        addStrike();
      }

    }

  }

}

