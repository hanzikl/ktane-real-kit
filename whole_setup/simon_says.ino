#ifdef DEBUGING
#define DEBUGING_SIMON
#define DEBUGING_SIMON_INPUT
#endif

/*
  SETUP FOR SIMON SAYS
*/

byte simon_previous_reading = 0x00;

#define SIMON_STAGE_COUNT 4
#define SIMON_BUTTON_COUNT 4
#define INPUT_MASK_SIMON 0b11110000 // must be inversed
// #define OUTPUT_MASK_SIMON 0b01111100

#define SIMON_DISARM_LED 4

// setup data locations
#define SIMON_DATA_PO 0
#define SIMON_DATA_PI 1
#define SIMON_DATA_SL 2
#define SIMON_DATA_NX 3
#define SIMON_DATA_SQ 7

byte simon_rules[SIMON_BUTTON_COUNT] = {1, 2, 3, 0};

const byte simon_input_connection[] = {16, 32, 64, 128};
const byte simon_output_connection[] = {32, 4, 16, 8, 2};

byte simon_stage = 0;

/*
   Getting and setting data
*/
byte getProgressOut(byte module_number) {
  return module_data[module_number][SIMON_DATA_PO];
}

byte getProgressIn(byte module_number) {
  return module_data[module_number][SIMON_DATA_PI];
}

byte getSeqLength(byte module_number) {
  return module_data[module_number][SIMON_DATA_SL];
}

unsigned long getNextChangeTime(byte module_number) {

  unsigned long value = 0;
  value = (value << 8) + module_data[module_number][SIMON_DATA_NX];
  value = (value << 8) + module_data[module_number][SIMON_DATA_NX + 1];
  value = (value << 8) + module_data[module_number][SIMON_DATA_NX + 2];
  value = (value << 8) + module_data[module_number][SIMON_DATA_NX + 3];

  return value;
}

byte getSequenceByte(byte module_number, byte byte_number) {
  return module_data[module_number][SIMON_DATA_SQ + byte_number];
}

void setProgressOut(byte module_number, byte value) {
  module_data[module_number][SIMON_DATA_PO] = value;
}

void setProgressIn(byte module_number, byte value) {
  module_data[module_number][SIMON_DATA_PI] = value;
}

void setSeqLength(byte module_number, byte value) {
  module_data[module_number][SIMON_DATA_SL] = value;
}

void setNextChangeTime(byte module_number, unsigned long value) {

  module_data[module_number][SIMON_DATA_NX] = (value >> 24) & 0xFF;
  module_data[module_number][SIMON_DATA_NX + 1] = (value >> 16) & 0xFF;
  module_data[module_number][SIMON_DATA_NX + 2] = (value >> 8) & 0xFF;
  module_data[module_number][SIMON_DATA_NX + 3] = value & 0xFF;

}

void setSequenceByte(byte module_number, byte byte_number, byte value) {
  module_data[module_number][SIMON_DATA_SQ + byte_number] = value;
}

void test_simon_output(byte module_number) {

  int delayTime = 600;

  for (int j = 0; j < 2; j++) {

    byte pos = SRoffsetsOutput[module_number];

    for (int i = 0; i < 4; i++) {
      shift_register_output[pos] = simon_output_connection[i];
      write_to_output_shift_register();
      delay(delayTime);
    }

    shift_register_output[pos] = simon_output_connection[SIMON_DISARM_LED];
    write_to_output_shift_register();
    delay(delayTime);

    /*
        shift_register_output[pos] = 0;
        write_to_output_shift_register();
    */
  }

}

void test_simon_input(byte module_number) {

  byte outPos = SRoffsetsOutput[module_number];

  int delayTime = 50; // ms

  boolean have[5] = {false, false, false, false};

  while (true) {

    update_shift_registers();
    delay(delayTime);

    byte input_value = get_module_input(module_number, INPUT_MASK_SIMON, true);

    // figure out, which button is on
    for (int i = 0; i < 4; i++) {
      if (input_value == simon_input_connection[i]) {
        have[i] = true;
      }
    }

    byte output_value = 0;
    for (int i = 0; i < 4; i++) {
      if (have[i]) {
        output_value += simon_output_connection[i];
      }
    }

    if (have[0] && have[1] && have[2] && have[3]) {
      break;
    }

    shift_register_output[outPos] = output_value;

  }

  shift_register_output[outPos] = 0;


}

void test_simon_time(byte module_number) {
  /**
     sanity check, if time (unsigned long) is stored and retrieved correctly
     not used
  */
  unsigned long tt = millis();
  setNextChangeTime(module_number, tt);

  unsigned long ss = getNextChangeTime(module_number);

  Serial.print(F("TT:"));
  if (ss == tt) {
    Serial.print(F("OK"));
  } else {
    Serial.print(F("F"));
  }

}

void setup_simon(byte module_number) {

}

void update_simon(byte module_number) {

  if (module_status[module_number] == MODULE_TESTING) {
    test_simon_output(module_number);
    test_simon_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  if (module_status[module_number] == MODULE_DISARMED) {
    byte pos = SRoffsetsOutput[module_number];
    shift_register_output[pos] = simon_output_connection[SIMON_DISARM_LED];
  }

}

