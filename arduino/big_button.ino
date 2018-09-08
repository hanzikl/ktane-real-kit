#ifdef DEBUGING
#define DEBUGING_BIGB
#define DEBUGING_BIGB_INPUT
#define DEBUGING_BIGB_OUTPUT
#endif

/*
  SETUP FOR BIG BUTTON
*/


#define INPUT_MASK_BIGB 0b00010000 // must be in reversed order

#define BIGB_DISARM_LED 0

#define BIGB_DATA_RULES 0     // 3 byte rules - 16xCOLOR + CORRECT_DIGIT for each strike (0-2 strikes)
#define BIGB_DATA_PUSHED 3    // if the big button was pushed
#define BIGB_DATA_PT 4        // time of big button push - 4byte variable

const byte big_button_output_connection[] = {128, 32, 8, 16, 64}; // DISARM INDICATOR, COLORS (TODO)

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

}

void test_big_button_input(byte module_number) {

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

void update_big_button(byte module_number) {

}
